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

#ifndef __RISCV64_ISA_ALL_INSTR_H__
#define __RISCV64_ISA_ALL_INSTR_H__
#include <cpu/decode.h>
#include "../local-include/rtl.h"

#if defined(CONFIG_DEBUG) || defined(CONFIG_SHARE)
#define AMO_INSTR_BINARY(f) \
  f(lr_w) f(lr_d)
#define AMO_INSTR_TERNARY(f) \
  f(sc_w) f(sc_d) \
  f(amoadd_w) f(amoswap_w) f(amoxor_w) f(amoor_w) f(amoand_w) \
  f(amomin_w) f(amomax_w) f(amominu_w) f(amomaxu_w) \
  f(amoadd_d) f(amoswap_d) f(amoxor_d) f(amoor_d) f(amoand_d) \
  f(amomin_d) f(amomax_d) f(amominu_d) f(amomaxu_d)
#else
#define AMO_INSTR_BINARY(f)
#define AMO_INSTR_TERNARY(f) f(atomic)
#endif

#ifdef CONFIG_RVH
#ifdef CONFIG_RV_SVINVAL
  #define RVH_INST_BINARY(f) f(hfence_vvma) f(hfence_gvma) f(hinval_vvma) f(hinval_gvma) \
    f(hlv_b) f(hlv_bu) f(hlv_h) f(hlv_hu) f(hlvx_hu) f(hlv_w) f(hlvx_wu) f(hlv_wu) f(hlv_d) \
    f(hsv_b) f(hsv_h) f(hsv_w) f(hsv_d)
#else
  #define RVH_INST_BINARY(f) f(hfence_vvma) f(hfence_gvma) \
    f(hlv_b) f(hlv_bu) f(hlv_h) f(hlv_hu) f(hlvx_hu) f(hlv_w) f(hlvx_wu) f(hlv_wu) f(hlv_d) \
    f(hsv_b) f(hsv_h) f(hsv_w) f(hsv_d)
#endif // CONFIG_RV_SVINVAL
#else
  #define RVH_INST_BINARY(f)
#endif // CONFIG_RVH

#if defined(CONFIG_DEBUG) || defined(CONFIG_SHARE)
#ifdef CONFIG_RV_SVINVAL
#define SYS_INSTR_NULLARY(f) \
  f(ecall) f(ebreak) f(c_ebreak) f(mret) f(sret) f(wfi) \
  f(sfence_w_inval) f(sfence_inval_ir)
#define SYS_INSTR_BINARY(f) \
  f(sfence_vma) f(sinval_vma) RVH_INST_BINARY(f)
#else
#define SYS_INSTR_NULLARY(f) \
  f(ecall) f(ebreak) f(c_ebreak) f(mret) f(sret) f(wfi)
#define SYS_INSTR_BINARY(f) \
  f(sfence_vma) RVH_INST_BINARY(f)
#endif

#define SYS_INSTR_TERNARY(f) \
  f(csrrw) f(csrrs) f(csrrc) f(csrrwi) f(csrrsi) f(csrrci)
#else
#ifdef CONFIG_RVH
#define SYS_INSTR_NULLARY(f) f(c_ebreak)
#define SYS_INSTR_BINARY(f)
#define SYS_INSTR_TERNARY(f) f(priv) f(hload) f(hstore)
#else
#define SYS_INSTR_NULLARY(f) f(c_ebreak)
#define SYS_INSTR_BINARY(f)
#define SYS_INSTR_TERNARY(f) f(system)
#endif // CONFIG_RVH
#endif
// TODO: sfence.vma and sinval.vma need two reg operand, only one(addr) now

#ifdef CONFIG_RVV
#define VECTOR_INSTR_TERNARY(f) \
  f(vadd) f(vsub) f(vrsub) f(vminu) f(vmin) \
  f(vmaxu) f(vmax) f(vand) f(vor) f(vxor) \
  f(vrgather) f(vrgatherei16) \
  f(vadc) f(vmadc) f(vsbc) f(vmsbc) \
  f(vmerge) f(vmseq) f(vmsne) f(vmsltu) \
  f(vmslt) f(vmsleu) f(vmsle) f(vmsgtu) \
  f(vmsgt) f(vsaddu) f(vsadd) f(vssubu) \
  f(vssub) f(vaadd) f(vaaddu) f(vsll) f(vasub) \
  f(vasubu) f(vsmul) f(vsrl) f(vsra) \
  f(vssra) f(vnsrl) f(vnsra) f(vnclipu) \
  f(vnclip) f(vssrl) f(vwredsumu) f(vwredsum) \
  f(vdotu) f(vdot) f(vwsmaccu) f(vwsmacc) \
  f(vwsmaccsu) f(vwsmaccus) f(vredsum) \
  f(vredand) f(vredor) f(vredxor) f(vredminu) \
  f(vredmin) f(vredmaxu) f(vredmax) f(vmvsx) \
  f(vmvxs) f(vpopc) f(vfirst) f(vmsbf) f(vmsof) \
  f(vmsif) f(viota) f(vid) f(vcompress) f(vmandnot) \
  f(vmand) f(vmor) f(vmxor) f(vmornot) f(vmnand) \
  f(vmnor) f(vmxnor) f(vdivu) f(vdiv) f(vremu) \
  f(vrem) f(vmulhu) f(vmul) f(vmulhsu) f(vmulh) \
  f(vmadd) f(vnmsub) f(vmacc) f(vnmsac) f(vwaddu) \
  f(vwadd) f(vwsub) f(vwsubu) f(vwaddu_w) f(vwadd_w) \
  f(vwsubu_w) f(vwsub_w) f(vwmulu) f(vwmulsu) \
  f(vwmul) f(vwmaccu) f(vwmacc) f(vwmaccsu) \
  f(vwmaccus) f(vle) f(vlse) f(vlxe) \
  f(vse) f(vlr) f(vlm) f(vsr) f(vsm) \
  f(vlr_mmu) f(vlm_mmu) f(vsr_mmu) f(vsm_mmu) \
  f(vsse) f(vsxe) f(vsetvl) f(vsetvli) f(vsetivli) \
  f(vle_mmu) f(vlse_mmu) f(vlxe_mmu) f(vse_mmu) f(vsse_mmu) f(vsxe_mmu) \
  f(vslideup) f(vslidedown) f(vslide1up) f(vslide1down) f(vmvnr) \
  f(vzextvf8) f(vsextvf8) f(vzextvf4) f(vsextvf4) f(vzextvf2) f(vsextvf2) \
  f(vfadd) f(vfredusum) f(vfsub) f(vfredosum) f(vfmin) f(vfredmin) \
  f(vfmax) f(vfredmax) f(vfsgnj) f(vfsgnjn) f(vfsgnjx) f(vfslide1up) \
  f(vfslide1down) f(vfmvfs) f(vfmvsf) f(vfcvt_xufv) f(vfcvt_xfv) \
  f(vfcvt_fxuv) f(vfcvt_fxv) f(vfcvt_rtz_xufv) f(vfcvt_rtz_xfv) \
  f(vfwcvt_xufv) f(vfwcvt_xfv) f(vfwcvt_fxuv) f(vfwcvt_fxv) \
  f(vfwcvt_ffv) f(vfwcvt_rtz_xufv) f(vfwcvt_rtz_xfv) f(vfncvt_xufw) \
  f(vfncvt_xfw) f(vfncvt_fxuw) f(vfncvt_fxw) f(vfncvt_ffw) \
  f(vfncvt_rod_ffw) f(vfncvt_rtz_xufw) f(vfncvt_rtz_xfw) f(vfsqrt_v) \
  f(vfrsqrt7_v) f(vfrec7_v) f(vfclass_v) f(vfmerge) \
  f(vmfeq) f(vmfle) f(vmflt) f(vmfne) f(vmfgt) f(vmfge) f(vfdiv) f(vfrdiv) \
  f(vfmul) f(vfrsub) f(vfmadd) f(vfnmadd) f(vfmsub) f(vfnmsub) \
  f(vfmacc) f(vfnmacc) f(vfmsac) f(vfnmsac) f(vfwadd) f(vfwredusum) \
  f(vfwsub) f(vfwredosum) f(vfwadd_w) f(vfwsub_w) \
  f(vfwmul) f(vfwmacc) f(vfwnmacc) f(vfwmsac) f(vfwnmsac) \
  f(vandn) f(vbrev_v) f(vbrev8_v) f(vrev8_v) f(vclz_v) f(vctz_v) \
  f(vrol) f(vror) f(vwsll) f(vcpop_v)
#else // CONFIG_RVV
#define VECTOR_INSTR_TERNARY(f)
#endif // CONFIG_RVV

#ifdef CONFIG_RV_CBO
#define CBO_INSTR_TERNARY(f) \
  f(cbo_zero) f(cbo_zero_mmu) f(cbo_inval) f(cbo_flush) f(cbo_clean)
#else // CONFIG_RV_CBO
#define CBO_INSTR_TERNARY(f)
#endif // CONFIG_RV_CBO

#ifdef CONFIG_RVB
#define BITMANIP_INSTR_TERNARY(f) \
  f(andn) f(orn) f(xnor) \
  f(min) f(minu) f(max) f(maxu) \
  f(bclr) f(bset) f(binv) f(bext) \
  f(bclri) f(bseti) f(binvi) f(bexti) \
  f(clmul) f(clmulr) f(clmulh) \
  f(rol) f(rolw) f(ror) f(rori) f(rorw) f(roriw) \
  f(sh1add) f(sh2add) f(sh3add) f(sh1adduw) f(sh2adduw) f(sh3adduw) \
  f(pack) f(packh) f(packw) \
  f(xpermn) f(xpermb) \
  f(adduw) f(slliuw)

#define BITMANIP_INSTR_BINARY(f) \
  f(clz) f(clzw) f(ctz) f(ctzw) f(cpop) f(cpopw) \
  f(orc_b) f(rev8) f(revb) f(sext_b) f(sext_h) f(zext_h)

#else // CONFIG_RVB
#define BITMANIP_INSTR_TERNARY(f)
#define BITMANIP_INSTR_BINARY(f)
#endif // CONFIG_RVB

#ifdef CONFIG_RVK // TODO: RV32 TO BE IMPLIED
#define CRYPTO_INSTR_TERNARY(f) \
  f(aes64es) f(aes64esm) f(aes64ds) f(aes64dsm) \
  f(aes64ks1i) f(aes64ks2) \
  f(sm4ks) f(sm4ed)

#define CRYPTO_INSTR_BINARY(f) \
  f(aes64im) f(sm3p0) f(sm3p1) \
  f(sha256sig0) f(sha256sig1) f(sha256sum0) f(sha256sum1) \
  f(sha512sig0) f(sha512sig1) f(sha512sum0) f(sha512sum1)

#else // CONFIG_RVK
#define CRYPTO_INSTR_TERNARY(f)
#define CRYPTO_INSTR_BINARY(f)
#endif // CONFIG_RVK

#ifdef CONFIG_RV_ZICOND
#define ZICOND_INSTR_TERNARY(f) \
  f(czero_eqz) f(czero_nez)
#else // CONFIG_RV_ZICOND
#define ZICOND_INSTR_TERNARY(f)
#endif // CONFIG_RV_ZICOND

#ifdef CONFIG_RV_ZFH_MIN
#define ZFH_MIN_INSTR_BINARY(f) \
  f(flh) f(fsh) \
  f(fmv_x_h) f(fmv_h_x)  \
  f(fcvt_s_h) f(fcvt_h_s) f(fcvt_d_h) f(fcvt_h_d) 
#else //CONFIG_RV_ZFH_MIN
#define ZFH_MIN_INSTR_BINARY(f)
#endif //CONFIG_RV_ZFH_MIN

#ifdef CONFIG_RV_ZFH
#define ZFH_INSTR_BINARY(f) \
  f(fsqrth) f(fsgnjh) f(fsgnjnh) f(fsgnjxh) \
  f(feqh) f(flth) f(fleh) f(fclassh) \
  f(fcvt_w_h) f(fcvt_wu_h) f(fcvt_h_w) f(fcvt_h_wu) \
  f(fcvt_l_h) f(fcvt_lu_h) f(fcvt_h_l) f(fcvt_h_lu)
#define ZFH_INSTR_TERNARY(f) \
  f(faddh) f(fsubh) f(fmulh) f(fdivh) \
  f(fminh) f(fmaxh) \
  f(fmaddh) f(fmsubh) f(fnmsubh) f(fnmaddh)
#else //CONFIG_RV_ZFH
#define ZFH_INSTR_BINARY(f)
#define ZFH_INSTR_TERNARY(f)
#endif //CONFIG_RV_ZFH

#ifdef CONFIG_RV_ZFA
#define ZFA_INSTR_BINARY(f) \
  f(fli_s) f(fli_d) \
  f(fround_s) f(fround_d) f(froundnx_s) f(froundnx_d) \
  f(fcvtmod_w_d)
#define ZFA_INSTR_TERNARY(f) \
  f(fminm_s) f(fminm_d) f(fmaxm_s) f(fmaxm_d) \
  f(fleq_s) f(fleq_d) f(fltq_s) f(fltq_d)
#else 
#define ZFA_INSTR_BINARY(f)
#define ZFA_INSTR_TERNARY(f)
#endif // CONFIG_RV_ZFA

#ifdef CONFIG_RV_ZFH_ZFA
#define ZFH_ZFA_INSTR_BINARY(f) \
  f(fli_h) f(fround_h) f(froundnx_h)
#define ZFH_ZFA_INSTR_TERNARY(f) \
  f(fminm_h) f(fmaxm_h) f(fleq_h) f(fltq_h)
#else
#define ZFH_ZFA_INSTR_BINARY(f)
#define ZFH_ZFA_INSTR_TERNARY(f)
#endif // CONFIG_RV_ZFH_ZFA

#ifdef CONFIG_FPU_NONE
#define FLOAT_INSTR_BINARY(f)
#define FLOAT_INSTR_TERNARY(f)
#else
#define FLOAT_INSTR_BINARY(f) \
  f(flw) f(fsw) f(flw_mmu) f(fsw_mmu) \
  f(fsqrts) f(fles) f(flts) f(feqs) \
  f(fcvt_s_w) f(fcvt_s_wu) f(fcvt_s_l) f(fcvt_s_lu) \
  f(fcvt_w_s) f(fcvt_wu_s) f(fcvt_l_s) f(fcvt_lu_s) \
  f(fsgnjs) f(fsgnjns) f(fsgnjxs) \
  f(fmv_x_w) f(fmv_w_x) f(fclasss) f(fclassd)\
  f(fld) f(fsd) f(fld_mmu) f(fsd_mmu) \
  f(fsqrtd) f(fled) f(fltd) f(feqd) \
  f(fcvt_d_w) f(fcvt_d_wu) f(fcvt_d_l) f(fcvt_d_lu) \
  f(fcvt_w_d) f(fcvt_wu_d) f(fcvt_l_d) f(fcvt_lu_d) \
  f(fsgnjd) f(fsgnjnd) f(fsgnjxd) \
  f(fmv_x_d) f(fmv_d_x) \
  f(fcvt_d_s) f(fcvt_s_d)
#define FLOAT_INSTR_TERNARY(f) \
  f(fadds) f(fsubs) f(fmuls) f(fdivs) f(fmins) f(fmaxs) \
  f(faddd) f(fsubd) f(fmuld) f(fdivd) f(fmind) f(fmaxd) \
  f(fmadds) f(fmsubs) f(fnmsubs) f(fnmadds) f(fmaddd) f(fmsubd) f(fnmsubd) f(fnmaddd)
#endif // CONFIG_FPU_NONE

#ifdef CONFIG_RV_ZIMOP
#define ZIMOP_INSTR_BINARY(f) \
  f(mop_r_0) \
  f(mop_r_1) \
  f(mop_r_2) \
  f(mop_r_3) \
  f(mop_r_4) \
  f(mop_r_5) \
  f(mop_r_6) \
  f(mop_r_7) \
  f(mop_r_8) \
  f(mop_r_9) \
  f(mop_r_10) \
  f(mop_r_11) \
  f(mop_r_12) \
  f(mop_r_13) \
  f(mop_r_14) \
  f(mop_r_15) \
  f(mop_r_16) \
  f(mop_r_17) \
  f(mop_r_18) \
  f(mop_r_19) \
  f(mop_r_20) \
  f(mop_r_21) \
  f(mop_r_22) \
  f(mop_r_23) \
  f(mop_r_24) \
  f(mop_r_25) \
  f(mop_r_26) \
  f(mop_r_27) \
  f(mop_r_28) \
  f(mop_r_29) \
  f(mop_r_30) \
  f(mop_r_31)

#define ZIMOP_INSTR_TERNARY(f) \
  f(mop_rr_0) \
  f(mop_rr_1) \
  f(mop_rr_2) \
  f(mop_rr_3) \
  f(mop_rr_4) \
  f(mop_rr_5) \
  f(mop_rr_6) \
  f(mop_rr_7)

#else
#define ZIMOP_INSTR_BINARY(f)
#define ZIMOP_INSTR_TERNARY(f)
#endif // CONFIG_RV_ZIMOP

#ifdef CONFIG_RV_ZCMOP
#define ZCMOP_INSTR_NULLARY(f) \
  f(c_mop)
#else
#define ZCMOP_INSTR_NULLARY(f)
#endif

#define INSTR_NULLARY(f) \
  f(inv) f(rt_inv) f(nemu_trap) \
  f(fence_i) f(fence) \
  SYS_INSTR_NULLARY(f) \
  f(p_ret) \
  ZCMOP_INSTR_NULLARY(f)

#define INSTR_UNARY(f) \
  f(p_li_0) f(p_li_1)

#define INSTR_BINARY(f) \
  f(lui) f(auipc) f(jal) \
  f(ld) f(lw) f(lh) f(lb) f(lwu) f(lhu) f(lbu) f(sd) f(sw) f(sh) f(sb) \
  f(c_j) f(p_jal) f(c_jr) f(c_jalr) \
  f(c_beqz) f(c_bnez) f(c_mv) f(p_sext_w) \
  BITMANIP_INSTR_BINARY(f) \
  CRYPTO_INSTR_BINARY(f) \
  AMO_INSTR_BINARY(f) \
  SYS_INSTR_BINARY(f) \
  f(ld_mmu) f(lw_mmu) f(lh_mmu) f(lb_mmu) f(lwu_mmu) f(lhu_mmu) f(lbu_mmu) \
  f(sd_mmu) f(sw_mmu) f(sh_mmu) f(sb_mmu) \
  FLOAT_INSTR_BINARY(f) \
  ZIMOP_INSTR_BINARY(f) \
  ZFH_MIN_INSTR_BINARY(f) \
  ZFH_INSTR_BINARY(f) \
  ZFA_INSTR_BINARY(f) \
  ZFH_ZFA_INSTR_BINARY(f)

#define INSTR_TERNARY(f) \
  f(add) f(sll) f(srl) f(slt) f(sltu) f(xor) f(or) f(sub) f(sra) f(and) \
  f(addi) f(slli) f(srli) f(slti) f(sltui) f(xori) f(ori) f(srai) f(andi) \
  f(addw) f(sllw) f(srlw) f(subw) f(sraw) \
  f(addiw) f(slliw) f(srliw) f(sraiw) \
  f(jalr) f(beq) f(bne) f(blt) f(bge) f(bltu) f(bgeu) \
  f(mul) f(mulh) f(mulhu) f(mulhsu) f(div) f(divu) f(rem) f(remu) \
  f(mulw) f(divw) f(divuw) f(remw) f(remuw) \
  f(c_li) f(c_addi) f(c_slli) f(c_srli) f(c_srai) f(c_andi) f(c_addiw) \
  f(c_add) f(c_and) f(c_or) f(c_xor) f(c_sub) f(c_addw) f(c_subw) \
  f(p_blez) f(p_bgez) f(p_bltz) f(p_bgtz) \
  f(p_inc) f(p_dec) \
  AMO_INSTR_TERNARY(f) \
  FLOAT_INSTR_TERNARY(f) \
  BITMANIP_INSTR_TERNARY(f) \
  CRYPTO_INSTR_TERNARY(f) \
  ZICOND_INSTR_TERNARY(f) \
  VECTOR_INSTR_TERNARY(f) \
  CBO_INSTR_TERNARY(f) \
  ZIMOP_INSTR_TERNARY(f) \
  ZFH_INSTR_TERNARY(f) \
  ZFA_INSTR_TERNARY(f) \
  ZFH_ZFA_INSTR_TERNARY(f)

#define INSTR_TERNARY_CSR(f) \
  SYS_INSTR_TERNARY(f) 

def_all_EXEC_ID();

#endif // __RISCV64_ISA_ALL_INSTR_H__
