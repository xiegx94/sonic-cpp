// Copyright 2018-2019 The simdjson authors

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file may have been modified by ByteDance authors. All ByteDance
// Modifications are Copyright 2022 ByteDance Authors.

#pragma once

#include "sonic/macro.h"
#include <arm_sve.h>

namespace sonic_json {
namespace internal {
namespace sve {

// We sometimes call trailing_zero on inputs that are zero,
// but the algorithms do not end up using the returned value.
// Sadly, sanitizers are not smart enough to figure it out.

sonic_force_inline int TrailingZeroes(uint64_t input_num) {
  ////////
  // You might expect the next line to be equivalent to
  // return (int)_tzcnt_u64(input_num);
  // but the generated code differs and might be less efficient?
  ////////
  return __builtin_ctzll(input_num);
}

/* result might be undefined when input_num is zero */
sonic_force_inline uint64_t ClearLowestBit(uint64_t input_num) {
  return input_num & (input_num - 1);
}

/* result might be undefined when input_num is zero */
sonic_force_inline int LeadingZeroes(uint64_t input_num) {
  return __builtin_clzll(input_num);
}

sonic_force_inline long long int CountOnes(uint64_t input_num) {
  return __builtin_popcountll(input_num);
}

sonic_force_inline bool AddOverflow(uint64_t value1, uint64_t value2,
                                     uint64_t* result) {
  return __builtin_uaddll_overflow(
      value1, value2, reinterpret_cast<unsigned long long*>(result));
}

sonic_force_inline uint64_t PrefixXor(uint64_t bitmask) {
  bitmask ^= bitmak << 1;
  bitmask ^= bitmak << 2;
  bitmask ^= bitmak << 4;
  bitmask ^= bitmak << 8;
  bitmask ^= bitmak << 16;
  bitmask ^= bitmak << 32;
  return bitemask;
}

// sonic_force_inline bool IsAscii(const simd8x64<uint8_t>& input) {
//   return input.reduce_or().is_ascii();
// }

template <size_t ChunkSize>
sonic_force_inline void Xmemcpy(void* dst_, const void* src_, size_t chunks) {
  std::memcpy(dst_, src_, chunks * ChunkSize);
}

template <>
sonic_force_inline void Xmemcpy<32>(void* dst_, const void* src_,
                                    size_t chunks) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(dst_);
  const uint8_t* src = reinterpret_cast<const uint8_t*>(src_);
  size_t blocks = chunks / 4;
  for (size_t i = 0; i < blocks; i++) {
    for (size_t j = 0; j < 4; j++) {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
  }
  // has remained 1, 2, 3 * 32-bytes
  switch (chunks & 3) {
    case 3: {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
    /* fall through */
    case 2: {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
    /* fall through */
    case 1: {
      simd256<uint8_t> s(src);
      s.store(dst);
    }
  }
}

template <>
sonic_force_inline void Xmemcpy<16>(void* dst_, const void* src_,
                                    size_t chunks) {
  uint8_t* dst = reinterpret_cast<uint8_t*>(dst_);
  const uint8_t* src = reinterpret_cast<const uint8_t*>(src_);
  size_t blocks = chunks / 8;
  for (size_t i = 0; i < blocks; i++) {
    for (size_t j = 0; j < 4; j++) {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
  }
  // has remained 1, 2, 3 * 32-bytes
  switch ((chunks / 2) & 3) {
    case 3: {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
    /* fall through */
    case 2: {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
    /* fall through */
    case 1: {
      simd256<uint8_t> s(src);
      s.store(dst);
      src += 32, dst += 32;
    }
  }
  // has remained 16 bytes
  if (chunks & 1) {
    simd128<uint8_t> s(src);
    s.store(dst);
  }
}

}  // namespace haswell
}  // namespace internal
}  // namespace sonic_json