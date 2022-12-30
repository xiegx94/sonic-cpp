#pragma once

#include "simd_dispatch.h"

#ifdef SONIC_STATIC_DISPATCH
#include INCLUDE_ARCH_FILE(itoa.h)
#endif

namespace sonic_json {
namespace internal {

SONIC_USING_ARCH_FUNC(U64toa);
SONIC_USING_ARCH_FUNC(I64toa);

} // namespace internal
} // namespace sonic_json
