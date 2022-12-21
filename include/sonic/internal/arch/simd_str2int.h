#pragma once

#include "simd_dispatch.h"

#ifdef SONIC_STATIC_DISPATCH
#include INCLUDE_ARCH_FILE(str2int.h)
#endif

namespace sonic_json {
namespace internal {

SONIC_USING_ARCH_FUNC(simd_str2int);

// sonic_force_inline uint64_t simd_str2int(const char* c, int& man_nd) {
//   return DISPATCH(str2int, c, man_nd);
// }

} // namespace internal
} // namespace sonic_json
