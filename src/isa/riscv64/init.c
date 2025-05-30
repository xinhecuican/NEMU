/***************************************************************************************
* Copyright (c) 2014-2021 Zihao Yu, Nanjing University
* Copyright (c) 2020-2022 Institute of Computing Technology, Chinese Academy of Sciences
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>
#include <memory/paddr.h>
#include <memory/sparseram.h>
#include "local-include/csr.h"

#ifndef CONFIG_SHARE
static const uint32_t img [] = {
  0x800002b7,  // lui t0,0x80000
  0x0002a023,  // sw  zero,0(t0)
  0x0002a503,  // lw  a0,0(t0)
  0x0000006b,  // nemu_trap
};
#endif

void init_csr();
#ifdef CONFIG_RV_SDTRIG
void init_trigger();
#endif // CONFIG_RV_SDTRIG

void init_clint();
void init_device();

#define CSR_ZERO_INIT(name, addr) name->val = 0;

void init_isa() {
  // NEMU has some cached states and some static variables in the source code.
  // They are assumed to have initialized states every time when the dynamic lib is loaded.
  // However, if we link NEMU as a static library, we have to manually initialize them.
  static bool is_second_call = false;
  if (is_second_call) {
    memset(csr_array, 0, sizeof(csr_array));
  }
  init_csr();

#ifndef CONFIG_RESET_FROM_MMIO
  cpu.pc = RESET_VECTOR;
#else
  cpu.pc = CONFIG_MMIO_RESET_VECTOR;
#endif
  cpu.lr_valid = 0;

  cpu.gpr[0]._64 = 0;

  cpu.mode = MODE_M;
  // For RV64 systems, the SXL and UXL fields are WARL fields that
  // control the value of XLEN for S-mode and U-mode, respectively.
  // For RV64 systems, if S-mode is not supported, then SXL is hardwired to zero.
  // For RV64 systems, if U-mode is not supported, then UXL is hardwired to zero.
  mstatus->val = 0xaUL << 32;
  // initialize the value fs and vs to 0
  mstatus->fs = 0;
  mstatus->vs = 0;

#ifdef CONFIG_RV_PMP_ENTRY_16
  pmpcfg0->val = 0;
  pmpcfg2->val = 0;
#endif // CONFIG_RV_PMP_ENTRY_16
#ifdef CONFIG_RV_PMP_ENTRY_64
  pmpcfg0->val = 0;
  pmpcfg2->val = 0;
  pmpcfg4->val = 0;
  pmpcfg6->val = 0;
  pmpcfg8->val = 0;
  pmpcfg10->val = 0;
  pmpcfg12->val = 0;
  pmpcfg14->val = 0;
#endif // CONFIG_RV_PMP_ENTRY_64

#ifndef CONFIG_SIM32
#define ext(e) (1 << ((e) - 'a'))
  misa->extensions = ext('i') | ext('m') | ext('a') | ext('c') | ext('s') | ext('u');
#ifndef CONFIG_FPU_NONE
  misa->extensions |= ext('d') | ext('f');
#endif // CONFIG_FPU_NONE
#endif
#ifdef CONFIG_RVH
  misa->extensions |= ext('h');
  hstatus->vsxl = 2; // equal to max len (spike)
  vsstatus->val = mstatus->val & SSTATUS_RMASK;
  mideleg->val |= ((1 << 12) | (1 << 10) | (1 << 6) | (1 << 2));
#endif // CONFIG_RVH
#ifdef CONFIG_RVB
  misa->extensions |= ext('b');
#endif // CONFIG_RVB
#ifdef CONFIG_RV_IMSIC
  miselect->val = 0;
  siselect->val = 0;
  vsiselect->val = 0;
  mireg->val = 0;
  sireg->val = 0;
  vsireg->val = 0;
  mtopi->val = 0;
  stopi->val = 0;
  vstopi->val = 0;
  mvien->val = 0;
  mvip->val = 0;
  hvien->val = 0;
  hvictl->val = 0;
  hviprio1->val = 0;
  hviprio2->val = 0;
  mtopei->val = 0;
  stopei->val = 0;
  vstopei->val = 0;
#endif // CONFIG_RV_IMSIC

#ifndef CONFIG_SIM32
  misa->mxl = 2; // XLEN = 64
#endif

#ifdef CONFIG_SIM32
  misa->val = 0x40141125;
#endif

#ifdef CONFIG_RVV
  // vector
  misa->extensions |= ext('v');
  vl->val = 0;
  vtype->val = (uint64_t) 1 << 63; // actually should be 1 << 63 (set vill bit to forbidd)
  vlenb->val = VLEN/8;
#endif // CONFIG_RVV

  // mcycle and minstret record :
  // - the difference between the absolute number and the write value, when the bit of mcountinhibit is clear;
  // - the inhibited number, when the bit of mcountinhibit is set.
  mcycle->val = 0;
  minstret->val = 0;

#ifdef CONFIG_RV_CSR_MCOUNTINHIBIT
  mcountinhibit->val = 0; 
#endif // CONFIG_RV_CSR_MCOUNTINHIBIT

  // All hpm counters are read-only zero in NEMU
  MAP(CSRS_UNPRIV_HPMCOUNTER, CSR_ZERO_INIT);
  MAP(CSRS_M_HPMCOUNTER, CSR_ZERO_INIT);
  MAP(CSRS_M_HPMEVENT, CSR_ZERO_INIT);

#ifdef CONFIG_USE_XS_ARCH_CSRS
  mvendorid->val = 0;
  marchid->val = 25;
  mimpid->val = 0;
#else
  mvendorid->val = CONFIG_MVENDORID_VALUE;
  marchid->val = CONFIG_MARCHID_VALUE;
  mimpid->val = CONFIG_MIMPID_VALUE;
#endif // CONFIG_USE_XS_ARCH_CSRS

#ifdef CONFIG_RV_SDTRIG
  init_trigger();
#endif // CONFIG_RV_SDTRIG

#define MSTATEEN0_RESET  0xdc00000000000001ULL
#define HSTATEEN0_RESET  0xdc00000000000001ULL
#define SSTATEEN0_RESET  0x0000000000000001ULL
#ifdef CONFIG_RV_SMSTATEEN
  mstateen0->val = MSTATEEN0_RESET;
  hstateen0->val = HSTATEEN0_RESET;
  sstateen0->val = SSTATEEN0_RESET;
#endif // CONFIG_RV_SMSTATEEN

#ifndef CONFIG_SHARE
  extern char *cpt_file;
  extern bool checkpoint_restoring;
  if (cpt_file == NULL && !checkpoint_restoring) {
    #ifdef CONFIG_USE_SPARSEMM
    sparse_mem_write(get_sparsemm(), RESET_VECTOR, sizeof(img), img);
    #else
    memcpy(guest_to_host(RESET_VECTOR), img, sizeof(img));
    #endif
  }
#endif

  init_clint();

  if (!is_second_call) {
    IFDEF(CONFIG_SHARE, init_device());
  }

#ifndef CONFIG_SHARE
  Log("NEMU will start from pc 0x%lx", cpu.pc);
#endif

  csr_prepare();

  is_second_call = true;
}
