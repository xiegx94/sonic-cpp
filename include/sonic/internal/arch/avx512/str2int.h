#pragma once

#include "../sse/str2int.h"

namespace sonic_json {
namespace internal {
namespace avx512 {

  using sse::simd_str2int;

} // namepsace avx512
} // namespace internal
} // namespace sonic_json
