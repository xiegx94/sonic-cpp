#pragma once


#include <arm_neon.h>
#include "sonic/macro.h"

namespace sonic_json {
namespace internal {
namespace neon {

sonic_force_inline uint64_t to_bitmask(uint8x16_t v) {
  return vget_lane_u64(vreinterpret_u64_u8(vshrn_n_u16(v,4)), 0);
}

}  // namespace neon
}  // namespace internal
}  // namespace sonic_json
