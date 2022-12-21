#pragma once

#include "../sse/str2int.h"

#include <sonic/macro.h>
#include <cstdint>

namespace sonic_json {
namespace internal {

sonic_force_inline uint64_t str2int_avx2(const char* c, int& man_nd) {
  return str2int_sse(c, man_nd);
}

} // namespace internal
} // namespace sonic_json
