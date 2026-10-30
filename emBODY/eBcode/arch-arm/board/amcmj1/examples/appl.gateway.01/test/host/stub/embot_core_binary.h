// stub for the host tests (test/host): only what amcmj1_gwlink, amcmj1_motdriver and dictionary.amcmj1 need
#pragma once
#include <cstdint>
namespace embot::core::binary::mask { template<typename T> constexpr T pos2mask(uint8_t p) { return static_cast<T>(1) << p; } }
