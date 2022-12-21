#pragma once

#include "../sse/str2int.h"

#include <sonic/macro.h>
#include <cstdint>

namespace sonic_json {
namespace internal {
namespace avx2 {

using sse::simd_str2int;

} // namespace avx2
} // namespace internal
} // namespace sonic_json
