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

#include <cstring>
#include <vector>

#include "sonic/error.h"
#include "sonic/internal/arch/avx2/base.h"
#include "sonic/internal/arch/avx2/unicode.h"
#include "sonic/internal/arch/avx2/simd.h"
#include "sonic/macro.h"

#include "../common/quote_common.h"
#include "../common/quote_tables.h"

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

namespace sonic_json {
namespace internal {
namespace avx2 {

using namespace simd;

sonic_force_inline size_t parseStringInplace(uint8_t *&src, SonicError &err) {
#define SONIC_REPEAT8(v) {v v v v v v v v}

  uint8_t *dst = src;
  uint8_t *sdst = src;
  while (1) {
  find:
    auto block = StringBlock::Find(src);
    if (block.HasQuoteFirst()) {
      int idx = block.QuoteIndex();
      src += idx;
      *src++ = '\0';
      return src - sdst - 1;
    }
    if (block.HasUnescaped()) {
      err = kParseErrorUnEscaped;
      return 0;
    }
    if (!block.HasBackslash()) {
      src += 32;
      goto find;
    }

    /* find out where the backspace is */
    auto bs_dist = block.BsIndex();
    src += bs_dist;
    dst = src;
  cont:
    uint8_t escape_char = src[1];
    if (sonic_unlikely(escape_char == 'u')) {
      if (!handle_unicode_codepoint(const_cast<const uint8_t **>(&src), &dst)) {
        err = kParseErrorEscapedUnicode;
        return 0;
      }
    } else {
      *dst = kEscapedMap[escape_char];
      if (sonic_unlikely(*dst == 0u)) {
        err = kParseErrorEscapedFormat;
        return 0;
      }
      src += 2;
      dst += 1;
    }
    // fast path for continous escaped chars
    if (*src == '\\') {
      bs_dist = 0;
      goto cont;
    }

  find_and_move:
    // Copy the next n bytes, and find the backslash and quote in them.
    simd256<uint8_t> v(src);
    block = StringBlock{
        static_cast<uint32_t>((v == '\\').to_bitmask()),  // bs_bits
        static_cast<uint32_t>((v == '"').to_bitmask()),   // quote_bits
        static_cast<uint32_t>((v <= '\x1f').to_bitmask()),
    };
    // If the next thing is the end quote, copy and return
    if (block.HasQuoteFirst()) {
      // we encountered quotes first. Move dst to point to quotes and exit
      while (1) {
        SONIC_REPEAT8(if (sonic_unlikely(*src == '"')) break;
                      else { *dst++ = *src++; });
      }
      *dst = '\0';
      src++;
      return dst - sdst;
    }
    if (block.HasUnescaped()) {
      err = kParseErrorUnEscaped;
      return 0;
    }
    if (!block.HasBackslash()) {
      /* they are the same. Since they can't co-occur, it means we
       * encountered neither. */
      v.store(dst);
      src += 32;
      dst += 32;
      goto find_and_move;
    }
    while (1) {
      SONIC_REPEAT8(if (sonic_unlikely(*src == '\\')) break;
                    else { *dst++ = *src++; });
    }
    goto cont;
  }
  sonic_assert(false);
#undef SONIC_REPEAT8
}

static sonic_force_inline int CopyAndGetEscapMask128(const char *src,
                                                     char *dst) {
  simd128<uint8_t> v(reinterpret_cast<const uint8_t *>(src));
  v.store(reinterpret_cast<uint8_t *>(dst));
  return ((v < '\x20') | (v == '\\') | (v == '"')).to_bitmask();
}

static sonic_force_inline int CopyAndGetEscapMask256(const char *src,
                                                     char *dst) {
  simd256<uint8_t> v(reinterpret_cast<const uint8_t *>(src));
  v.store(reinterpret_cast<uint8_t *>(dst));
  return ((v < '\x20') | (v == '\\') | (v == '"')).to_bitmask();
}

sonic_static_inline char *Quote(const char *src, size_t nb, char *dst) {
  *dst++ = '"';
  sonic_assert(nb < (1ULL << 32));
  uint32_t mm;
  int cn;

  /* 32-byte loop */
  while (nb >= 32) {
    /* check for matches */
    // TODO: optimize: exploit the simd bitmask in the escape block.
    if ((mm = CopyAndGetEscapMask256(src, dst)) != 0) {
      cn = __builtin_ctz(mm);
      MOVE_N_CHARS(src, cn);
      DoEscape(src, dst, nb);
    } else {
      /* move to next block */
      MOVE_N_CHARS(src, 32);
    }
  }

  if (nb > 0) {
    char tmp_src[64];
    const char *src_r;
#ifdef SONIC_USE_SANITIZE
    if (0) {
#else
    /* This code would cause address sanitizer report heap-buffer-overflow. */
    if (((size_t)(src) & (PAGE_SIZE - 1)) <= (PAGE_SIZE - 64)) {
      src_r = src;
#endif
    } else {
      std::memcpy(tmp_src, src, nb);
      src_r = tmp_src;
    }
    while (nb > 0) {
      mm = CopyAndGetEscapMask256(src_r, dst) & (0xFFFFFFFF >> (32 - nb));
      if (mm) {
        cn = __builtin_ctz(mm);
        MOVE_N_CHARS(src_r, cn);
        DoEscape(src_r, dst, nb);
      } else {
        dst += nb;
        nb = 0;
      }
    }
  }

  *dst++ = '"';
  return dst;
}

}  // namespace avx2
}  // namespace internal
}  // namespace sonic_json
