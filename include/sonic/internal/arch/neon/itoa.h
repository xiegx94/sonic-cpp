/*
 * Copyright 2022 ByteDance Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <arm_neon.h>
#include <stdint.h>

#include "sonic/macro.h"

namespace sonic_json {

namespace internal {

namespace neon {

static const char kDigits[202] sonic_align(2) =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899";

static const char kVec16xAsc0[16] sonic_align(16) = {
    '0', '0', '0', '0', '0', '0', '0', '0',
    '0', '0', '0', '0', '0', '0', '0', '0',
};

static const uint16_t kVec8x10[8] sonic_align(16) = {
    10, 10, 10, 10, 10, 10, 10, 10,
};

static const uint32_t kVec4x10k[4] sonic_align(16) = {
    10000,
    10000,
    10000,
    10000,
};

static const uint32_t kVec4xDiv10k[4] sonic_align(16) = {
    0xd1b71759,
    0xd1b71759,
    0xd1b71759,
    0xd1b71759,
};

static const uint16_t kVecDivPowers[8] sonic_align(16) = {
    0x20c5, 0x147b, 0x3334, 0x8000, 0x20c5, 0x147b, 0x3334, 0x8000,
};

static const uint16_t kVecShiftPowers[8] sonic_align(16) = {
    0x0080, 0x0800, 0x2000, 0x8000, 0x0080, 0x0800, 0x2000, 0x8000,
};

// Convert num {abcd} to {axxxx, abxxxx, abcxxxx, abcdxxxx}
static sonic_force_inline uint16x8_t Utoa_4_helper(uint16_t num) {
  uint16_t v = num << 2;
  // v00 = vector{abcd * 4, abcd * 4, abcd * 4, abcd * 4}
  uint16x4_t v00 = vdup_n_u16(v);

  uint16x4_t kVecDiv = vreinterpret_u16_u64(vdup_n_u64(0x80003334147b20c5));
  uint32x4_t v01 = vmull_u16(v00, kVecDiv);
  uint16x4_t v02 = vshrn_n_u32(v01, 16);
  uint16x4_t kVecShift = vreinterpret_u16_u64(vdup_n_u64(0x8000200008000080));
  uint32x4_t v03 = vmull_u16(v02, kVecShift);
  return vreinterpretq_u16_u32(v03);
}

// Convert num's each digit as packed 16-bit in a vector.
// num's digits as abcdefgh (high bits is 0 if not enough)
// The converted vector is { a, b, c, d, e, f, g, h }
sonic_force_inline uint16x8_t UtoaNeon(uint32_t num) {
  uint16_t hi = num % 10000; // {efgh}
  uint16_t lo = num / 10000; // {abcd}

  // v10 = {a, ab, abc, abcd, e, ef, efg, efgh}
  uint16x8_t v10 = vuzp2q_u16(Utoa_4_helper(lo), Utoa_4_helper(hi));

  // v12 = {0, a0, ab0, abc0, 0, e0, ef0, efg0}
  uint16x8_t v11 = vmulq_u16(v10, vdupq_n_u16(10));
  uint16x8_t v12 = vreinterpretq_u16_u64(vshlq_n_u64(vreinterpretq_u64_u16(v11), 16));

  // v13 = {a, b, c, d, e, f, g}
  uint16x8_t v13 = vsubq_u16(v10, v12);
  return v13;
}

sonic_force_inline void Copy2Digs(char *dst, const char *src) {
  *(dst) = *(src);
  *(dst + 1) = *(src + 1);
}

sonic_force_inline char *Utoa_1_8(char *out, uint32_t val) {
  uint32_t hi, lo, a, b, c, d, lz;

  if (val < 100) {  // 1 ~ 2 digits
    lz = val < 10;
    Copy2Digs(out, &kDigits[val * 2 + lz]);
    out -= lz;
    return out + 2;
  } else if (val < 10000) {  // 3 ~ 4 digits
    hi = val / 100;
    lo = val % 100;
    lz = hi < 10;
    Copy2Digs(out, &kDigits[hi * 2 + lz]);
    out -= lz;
    Copy2Digs(out + 2, &kDigits[lo * 2]);
    return out + 4;
  } else if (val < 1000000) {  // 5 ~ 6 digits
    hi = val / 10000;
    lo = val % 10000;
    lz = hi < 10;
    Copy2Digs(out, &kDigits[hi * 2 + lz]);
    out -= lz;
    a = lo / 100;
    b = lo % 100;
    Copy2Digs(out + 2, &kDigits[a * 2]);
    Copy2Digs(out + 4, &kDigits[b * 2]);
    return out + 6;
  } else {  // 7 ~ 8 digits
    hi = val / 10000;
    lo = val % 10000;
    a = hi / 100;
    b = hi % 100;
    c = lo / 100;
    d = lo % 100;
    lz = a < 10;
    Copy2Digs(out, &kDigits[a * 2 + lz]);
    out -= lz;
    Copy2Digs(out + 2, &kDigits[b * 2]);
    Copy2Digs(out + 4, &kDigits[c * 2]);
    Copy2Digs(out + 6, &kDigits[d * 2]);
    return out + 8;
  }
}

static sonic_force_inline char *Utoa_8(uint32_t val, char *out) {
  /* convert to digits */
  uint16x8_t v0 = UtoaNeon(val);
  uint16x8_t v1 = vdupq_n_u16(0);

  /* convert to bytes, add '0' */
  uint8x16_t v2 = vcombine_u8(vqmovun_u16(v0), vqmovun_u16(v1));
  uint8x16_t v3 = vaddq_u8(v2, vdupq_n_u8('0'));

  /* store high 64 bits */
  vst1q_u8((uint8_t *)(out), v3);
  return out + 8;
}

static sonic_force_inline char *Utoa_16(uint64_t val, char *out) {
  /* remaining digits */
  uint16x8_t v0 = UtoaNeon((uint32_t)(val / 100000000));
  uint16x8_t v1 = UtoaNeon((uint32_t)(val % 100000000));
  /* convert to bytes, add '0' */
  uint8x16_t v2 = vcombine_u8(vqmovun_u16(v0), vqmovun_u16(v1));
  uint8x16_t v3 = vaddq_u8(v2, vdupq_n_u8('0'));

  vst1q_u8((uint8_t *)(out), v3);
  return out + 16;
}

sonic_force_inline char *U64toa_17_20(char *out, uint64_t val) {
  uint64_t lo = val % 10000000000000000;
  uint32_t hi = (uint32_t)(val / 10000000000000000);

  uint32_t aa, bb, lz;
  if (hi < 100) {  // 2 digits
    lz = hi < 10;
    Copy2Digs(out, &kDigits[hi * 2 + lz]);
    out += 2 - lz;
  } else if (hi < 10000) {  // 4 digits like aabb
    aa = hi / 100;
    bb = hi % 100;
    lz = aa < 10;
    Copy2Digs(out, &kDigits[aa * 2 + lz]);
    out -= lz;
    Copy2Digs(out + 2, &kDigits[bb * 2]);
    out += 4;
  }
  return Utoa_16(lo, out);
}

sonic_force_inline char *U64toa(char *out, uint64_t val) {
  if (sonic_likely(val < 100000000)) {  // 1 ~ 8 digits
    return Utoa_1_8(out, (uint32_t)val);
  } else if (sonic_likely(val < 10000000000000000)) {  // 8 ~ 16 digits
    uint32_t hi, lo;
    hi = ((uint32_t)(val / 100000000));
    lo = ((uint32_t)(val % 100000000));
    return Utoa_8(lo, Utoa_1_8(out, hi));
  } else {  // 17 ~ 20 digits
    return U64toa_17_20(out, val);
  }
}

sonic_force_inline char *I64toa(char *buf, int64_t val) {
  size_t neg = val < 0;
  *buf = '-';
  return U64toa(buf + neg, neg ? (uint64_t)(-val) : (uint64_t)val);
}

}  // namepsace avx2

}  // namespace internal

}  // namespace sonic_json
