//
// Created by WiwilZ on 2022/1/31.
//

#pragma once

#include <bit>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

constexpr size_t sse2_strlen(const char* str) noexcept __attribute_pure__ __nonnull ((1));

constexpr size_t sse4_strlen(const char* str) noexcept __attribute_pure__ __nonnull ((1));

constexpr size_t avx2_strlen(const char* str) noexcept __attribute_pure__ __nonnull ((1));

constexpr size_t avx512_strlen(const char* str) noexcept __attribute_pure__ __nonnull ((1));



constexpr size_t sse2_strlen(const char* str) noexcept {
    if (*str == '\0') {
        return 0;
    }

    const __m128i zero = _mm_setzero_si128();
    for (auto p = str;; p += sizeof(__m128i)) {
        const __m128i b = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(p));
        const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, b));
        if (mask != 0) {
            return p - str + std::countr_zero(mask);
        }
    }
}

constexpr size_t sse4_strlen(const char* str) noexcept {
    if (*str == '\0') {
        return 0;
    }

    const __m128i zero = _mm_setzero_si128();
    for (auto p = str;; p += sizeof(__m128i)) {
        const __m128i b = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(p));
        const unsigned mask = _mm_cmpistri(zero, b, _SIDD_CMP_EQUAL_EACH);
        if (mask != sizeof(__m128i)) {
            return p - str + mask;
        }
    }
}

constexpr size_t avx2_strlen(const char* str) noexcept {
    if (*str == '\0') {
        return 0;
    }

    const __m256i zero = _mm256_setzero_si256();
    for (auto p = str;; p += sizeof(__m256i)) {
        const __m256i b = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(p));
        const unsigned mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(zero, b));
        if (mask != 0) {
            return p - str + std::countr_zero(mask);
        }
    }
}

constexpr size_t avx512_strlen(const char* str) noexcept {
    if (*str == '\0') {
        return 0;
    }

    const __m512i zero = _mm512_setzero_si512();
    for (auto p = str;; p += sizeof(__m512i)) {
        const __m512i b = _mm512_loadu_si512(p);
        const uint64_t mask = _mm512_cmpeq_epi8_mask(zero, b);
        if (mask != 0) {
            return p - str + std::countr_zero(mask);
        }
    }
}


