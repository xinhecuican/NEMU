#include "spill.h"
#include "tran.h"
#include "rv64-backend/rv_ins_def.h"
#include "rtl/rtl.h"

typedef struct {
  uint32_t rvidx;
  uint32_t spmidx;
  bool used, dirty;
} Tmp_reg;
static Tmp_reg tmp_regs[TMP_REG_NUM];

void tmp_regs_init() {
  assert(TMP_REG_NUM == 2);
  tmp_regs[0].rvidx = TMP_REG_1;
  tmp_regs[1].rvidx = TMP_REG_2;
}

void tmp_regs_reset() {
  tmp_regs[0].spmidx = 0;
  tmp_regs[0].used = 0;
  tmp_regs[0].dirty = 0;
  tmp_regs[1].spmidx = 0;
  tmp_regs[1].used = 0;
  tmp_regs[1].dirty = 0;
}

int spmidx2tmpidx(uint32_t spmidx) {
  for (int i = 0; i < TMP_REG_NUM; i++) {
    if (tmp_regs[i].spmidx == spmidx) return i;
  }
  return -1;
}

int rvidx2tmpidx(uint32_t rvidx) {
  for (int i = 0; i < TMP_REG_NUM; i++) {
    if (tmp_regs[i].rvidx == rvidx) return i;
  }
  return -1;
}

uint32_t spmidx2rvidx(uint32_t spmidx) {
  int tmpidx = spmidx2tmpidx(spmidx);
  if (tmpidx == -1) return 0;
  tmp_regs[tmpidx].used = 1;
  return tmp_regs[tmpidx].rvidx;
}

uint32_t varidx2rvidx(uint32_t varidx) {
  if (varidx & ~SPMIDX_MASK) return varidx;
  int tmpidx = spmidx2tmpidx(varidx);
  assert(tmpidx != -1);
  return tmp_regs[tmpidx].rvidx;
}

void spill_writeback(uint32_t i) {
  if (tmp_regs[i].spmidx != 0 && tmp_regs[i].dirty) {
    spm(sw, tmp_regs[i].rvidx, 4 * (tmp_regs[i].spmidx & ~SPMIDX_MASK));
    tmp_regs[i].dirty = false;
  }
}

void spill_writeback_all() {  // can be 0/1/2 inst
  for (int i = 0; i < TMP_REG_NUM; i++) {
    spill_writeback(i);
  }
}

void spill_flush(uint32_t spmidx) {
  for (int i = 0; i < TMP_REG_NUM; i++) {
    if (tmp_regs[i].spmidx == spmidx) {
      tmp_regs[i].used = 0;
      return;
    }
  }
}

void spill_flush_all() {
  for (int i = 0; i < TMP_REG_NUM; i++) {
    tmp_regs[i].used = 0;
  }
}

// replace tmp_regs[tmpidx] with spmidx
void spill_replace(uint32_t tmpidx, uint32_t spmidx, int load_val) {
  spill_writeback(tmpidx);
  if (load_val) spm(lw, tmp_regs[tmpidx].rvidx, 4 * (spmidx & ~SPMIDX_MASK));

  tmp_regs[tmpidx].spmidx = spmidx;
  tmp_regs[tmpidx].used = 1;
  tmp_regs[tmpidx].dirty = false;
}

// find a clean tmpreg and map it to spmidx
uint32_t spill_alloc(uint32_t spmidx, int load_val) {
  uint32_t tmpidx = (tmp_regs[1].dirty ? 0 : 1);
  spill_replace(tmpidx, spmidx, load_val);
  return tmpidx;
}

uint32_t rtlreg2varidx(DecodeExecState *s, const rtlreg_t* dest);

static uint32_t rtlreg2rvidx_internal(DecodeExecState *s, const rtlreg_t *r, int is_dest) {
  uint32_t varidx = rtlreg2varidx(s, r);
  if (varidx & SPMIDX_MASK) {
    uint32_t tmpidx = spmidx2tmpidx(varidx);
    if (tmpidx == -1) tmpidx = spill_alloc(varidx, !is_dest);
    varidx = tmp_regs[tmpidx].rvidx;
    tmp_regs[tmpidx].dirty = is_dest;
  }
  return varidx;
}

uint32_t src2rvidx(DecodeExecState *s, const rtlreg_t *src) {
  return rtlreg2rvidx_internal(s, src, false);
}

uint32_t dest2rvidx(DecodeExecState *s, const rtlreg_t *dest) {
  return rtlreg2rvidx_internal(s, dest, true);
}

uint32_t rtlreg2rvidx_pair(DecodeExecState *s,
    const rtlreg_t *src1, int load_src1, const rtlreg_t *src2, int load_src2) {
  uint32_t src1_varidx = rtlreg2varidx(s, src1);
  uint32_t src2_varidx = rtlreg2varidx(s, src2);

  if ((src1_varidx & SPMIDX_MASK) && (src2_varidx & SPMIDX_MASK)) {
    // check whether they are already mapped
    uint32_t src1_tmpidx = spmidx2tmpidx(src1_varidx);
    uint32_t src2_tmpidx = spmidx2tmpidx(src2_varidx);

    if (src1_tmpidx == -1 && src2_tmpidx != -1) {
      src1_tmpidx = !src2_tmpidx;
      spill_replace(src1_tmpidx, src1_varidx, load_src1);
    } else if (src1_tmpidx != -1 && src2_tmpidx == -1) {
      src2_tmpidx = !src1_tmpidx;
      spill_replace(src2_tmpidx, src2_varidx, load_src2);
    } else if (src1_tmpidx == -1 && src2_tmpidx == -1) {
      src1_tmpidx = 0;
      src2_tmpidx = 1;
      spill_replace(src1_tmpidx, src1_varidx, load_src1);
      spill_replace(src2_tmpidx, src2_varidx, load_src2);
    }

    src1_varidx = tmp_regs[src1_tmpidx].rvidx;
    src2_varidx = tmp_regs[src2_tmpidx].rvidx;
  } else if (src1_varidx & SPMIDX_MASK) {
    uint32_t src1_tmpidx = spmidx2tmpidx(src1_varidx);
    if (src1_tmpidx == -1) src1_tmpidx = spill_alloc(src1_varidx, load_src1);
    src1_varidx = tmp_regs[src1_tmpidx].rvidx;
  } else if (src2_varidx & SPMIDX_MASK) {
    uint32_t src2_tmpidx = spmidx2tmpidx(src2_varidx);
    if (src2_tmpidx == -1) src2_tmpidx = spill_alloc(src2_varidx, load_src2);
    src2_varidx = tmp_regs[src2_tmpidx].rvidx;
  }

  return (src1_varidx << 16) | src2_varidx;
}

uint32_t spill_out_and_remap(DecodeExecState *s, uint32_t spmidx) {
  int tmpidx;
  for (tmpidx = 0; tmpidx < TMP_REG_NUM; tmpidx ++) {
    if (tmp_regs[tmpidx].used == 0) break;
  }
  Assert(tmpidx < TMP_REG_NUM, "no clean tmp_regs!\nalready used:%u %u, req: %u\n",
      tmp_regs[0].spmidx, tmp_regs[1].spmidx, spmidx);

  spill_writeback(tmpidx);
  spm(lw, tmp_regs[tmpidx].rvidx, 4 * (spmidx & ~SPMIDX_MASK));

  tmp_regs[tmpidx].spmidx = spmidx;
  tmp_regs[tmpidx].used = 1;
  tmp_regs[tmpidx].dirty = false;

  return tmp_regs[tmpidx].rvidx;
}

void spill_set_spmidx(uint32_t tmpidx, uint32_t new_spmidx) {
  tmp_regs[tmpidx].spmidx = new_spmidx;
}

void spill_set_dirty(uint32_t tmpidx) {
  tmp_regs[tmpidx].dirty = true;
}

void spill_set_dirty_rvidx(uint32_t rvidx) {
  int tmpidx = rvidx2tmpidx(rvidx);
  if (tmpidx != -1) spill_set_dirty(tmpidx);
}
