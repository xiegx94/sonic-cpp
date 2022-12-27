#pragma once

#include "simd_dispatch.h"

#ifdef SONIC_STATIC_DISPATCH
#include INCLUDE_ARCH_FILE(skip.h)
#endif

namespace sonic_json {
namespace internal {

SONIC_USING_ARCH_FUNC(SkipString);
SONIC_USING_ARCH_FUNC(SkipContainer);
SONIC_USING_ARCH_FUNC(SkipArray);
SONIC_USING_ARCH_FUNC(SkipObject);
SONIC_USING_ARCH_FUNC(SkipLiteral);
SONIC_USING_ARCH_FUNC(SkipNumber);
SONIC_USING_ARCH_FUNC(SkipScanner);
SONIC_USING_ARCH_FUNC(EqBytes4);
SONIC_USING_ARCH_FUNC(GetOnDemand);

} // namespace internal
} // namespace sonic_json
