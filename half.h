#pragma once

#include <bit>
#include <cstdint>
#include <type_traits>
#include <limits>

namespace half {

  // --- float32 <-> float16 conversions (IEEE 754) ---
  // Returns IEEE-754 binary16 bit pattern.
  constexpr std::uint16_t float_to_half_bits(float f) noexcept {
    const std::uint32_t x = std::bit_cast<std::uint32_t>(f);

    const std::uint32_t sign = (x >> 31) & 0x1u;
    const std::uint32_t exp  = (x >> 23) & 0xFFu;
    std::uint32_t mant       =  x        & 0x7FFFFFu;

    const std::uint16_t hs = static_cast<std::uint16_t>(sign << 15);

    // NaN or Inf
    if (exp == 0xFFu) {
      if (mant == 0) {
        return static_cast<std::uint16_t>(hs | 0x7C00u); // Inf
      }
      // quiet NaN with payload
      std::uint16_t payload = static_cast<std::uint16_t>(mant >> 13);
      if (payload == 0) payload = 1;
      return static_cast<std::uint16_t>(hs | 0x7C00u | payload);
    }

    // Zero / float32 subnormal -> signed zero (common choice; fast and usually acceptable)
    if (exp == 0) {
      return hs;
    }

    // Re-bias exponent: 127 -> 15
    int e = static_cast<int>(exp) - 127 + 15;

    // Underflow -> half subnormal or zero
    if (e <= 0) {
      if (e < -10) {
        return hs; // too small even for half subnormal
      }

      // Mantissa with implicit leading 1
      mant |= 0x800000u;

      // Shift into half subnormal mantissa (10 bits)
      const int shift = 1 - e; // 1..10
      std::uint32_t mant10 = mant >> (shift + 13);

      // Round-to-nearest-even
      const std::uint32_t round_bit = (mant >> (shift + 12)) & 1u;
      const std::uint32_t sticky    = mant & ((1u << (shift + 12)) - 1u);
      if (round_bit && (sticky || (mant10 & 1u))) {
        mant10++;
      }

      return static_cast<std::uint16_t>(hs | static_cast<std::uint16_t>(mant10));
    }

    // Overflow -> Inf
    if (e >= 31) {
      return static_cast<std::uint16_t>(hs | 0x7C00u);
    }

    // Normal half
    std::uint16_t he = static_cast<std::uint16_t>(e << 10);

    std::uint32_t mant10 = mant >> 13;        // top 10 bits
    const std::uint32_t round = mant & 0x1FFFu; // low 13 bits

    // Round-to-nearest-even
    if (round > 0x1000u || (round == 0x1000u && (mant10 & 1u))) {
      mant10++;
      if (mant10 == 0x400u) { // mantissa overflow
        mant10 = 0;
        e++;
        if (e >= 31) return static_cast<std::uint16_t>(hs | 0x7C00u);
        he = static_cast<std::uint16_t>(e << 10);
      }
    }

    return static_cast<std::uint16_t>(hs | he | static_cast<std::uint16_t>(mant10));
  }

  // Takes IEEE-754 binary16 bit pattern and returns float32 value.
  constexpr float half_bits_to_float(std::uint16_t h) noexcept {
    const std::uint32_t sign = (h >> 15) & 0x1u;
    std::uint32_t exp        = (h >> 10) & 0x1Fu;
    std::uint32_t mant       =  h        & 0x3FFu;

    const std::uint32_t fs = sign << 31;

    if (exp == 0) {
      if (mant == 0) {
        return std::bit_cast<float>(fs); // signed zero
      }
      // subnormal half -> normalize
      int e = -14;
      while ((mant & 0x400u) == 0) {
        mant <<= 1;
        --e;
      }
      mant &= 0x3FFu;
      const std::uint32_t fe = static_cast<std::uint32_t>(e + 127) << 23;
      const std::uint32_t fm = mant << 13;
      return std::bit_cast<float>(fs | fe | fm);
    }

    if (exp == 31) {
      // Inf/NaN
      const std::uint32_t fe = 0xFFu << 23;
      const std::uint32_t fm = mant ? (mant << 13) : 0u;
      return std::bit_cast<float>(fs | fe | fm);
    }

    // normal
    const std::uint32_t fe = static_cast<std::uint32_t>(exp - 15 + 127) << 23;
    const std::uint32_t fm = mant << 13;
    return std::bit_cast<float>(fs | fe | fm);
  }

  // --- Half type ---
  struct Half {
    using storage_type = std::uint16_t;
    storage_type bits{0};

    constexpr Half() noexcept = default;

    static constexpr Half from_bits(storage_type b) noexcept {
      Half h;
      h.bits = b;
      return h;
    }

    constexpr storage_type to_bits() const noexcept { return bits; }

    // Explicit conversions in (safer than implicit)
    constexpr explicit Half(float f) noexcept : bits(float_to_half_bits(f)) {}
    constexpr explicit Half(double d) noexcept : bits(float_to_half_bits(static_cast<float>(d))) {}
    constexpr explicit Half(long double d) noexcept : bits(float_to_half_bits(static_cast<float>(d))) {}

    constexpr explicit operator float() const noexcept { return half_bits_to_float(bits); }
  };

  // --- Traits / concept: treat Half as floating-like ---
  template <class U>
  inline constexpr bool is_half_v =
  std::is_same_v<std::remove_cvref_t<U>, Half>;

  template <class U>
  inline constexpr bool is_float_like_v =
  std::is_floating_point_v<std::remove_cvref_t<U>> || is_half_v<U>;

  template <class U>
  concept FloatLike = is_float_like_v<U>;

}

// numeric_limits specialization is permitted for user-defined types.
namespace std {
  template <>
  class numeric_limits<half::Half> {
    public:
    static constexpr bool is_specialized = true;
    static constexpr bool is_signed = true;
    static constexpr bool is_integer = false;
    static constexpr bool is_exact = false;
    static constexpr int radix = 2;

    static constexpr int digits = 11;
    static constexpr int digits10 = 3;
    static constexpr int max_digits10 = 5;

    static constexpr bool has_infinity = true;
    static constexpr bool has_quiet_NaN = true;
    static constexpr bool has_signaling_NaN = false;
    static constexpr float_denorm_style has_denorm = denorm_present;
    static constexpr bool has_denorm_loss = false;

    static constexpr bool is_iec559 = true;
    static constexpr bool is_bounded = true;
    static constexpr bool is_modulo = false;

    static constexpr int min_exponent = -13;
    static constexpr int min_exponent10 = -4;
    static constexpr int max_exponent = 16;
    static constexpr int max_exponent10 = 4;

    static constexpr bool traps = false;
    static constexpr bool tinyness_before = false;
    static constexpr float_round_style round_style = round_to_nearest;

    static constexpr half::Half min() noexcept { return half::Half::from_bits(0x0400u); }
    static constexpr half::Half lowest() noexcept { return half::Half::from_bits(0xFBFFu); }
    static constexpr half::Half max() noexcept { return half::Half::from_bits(0x7BFFu); }

    static constexpr half::Half epsilon() noexcept { return half::Half::from_bits(0x1400u); }
    static constexpr half::Half round_error() noexcept { return half::Half::from_bits(0x3800u); }

    static constexpr half::Half infinity() noexcept { return half::Half::from_bits(0x7C00u); }
    static constexpr half::Half quiet_NaN() noexcept { return half::Half::from_bits(0x7E00u); }
    static constexpr half::Half signaling_NaN() noexcept { return half::Half::from_bits(0x7E00u); }

    static constexpr half::Half denorm_min() noexcept { return half::Half::from_bits(0x0001u); }
  };
}

/*
 Copyright (c) 2026 Computer Graphics and Visualization Group, University of
 Duisburg-Essen

 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in the
 Software without restriction, including without limitation the rights to use, copy,
 modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and
 to permit persons to whom the Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be included in all copies
 or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
