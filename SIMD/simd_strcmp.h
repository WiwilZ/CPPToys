//
// Created by WiwilZ on 2022/2/2.
//

#pragma once

#include <bit>
#include <numeric>
#include <compare>
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <x86intrin.h>
#endif


std::strong_ordering sse2_strcmp(const char* s1, const char* s2) noexcept __attribute_pure__ __nonnull ((1, 2));

std::strong_ordering sse4_strcmp(const char* s1, const char* s2) noexcept __attribute_pure__ __nonnull ((1, 2));

std::strong_ordering avx2_strcmp(const char* s1, const char* s2) noexcept __attribute_pure__ __nonnull ((1, 2));

std::strong_ordering avx512_strcmp(const char* s1, const char* s2) noexcept __attribute_pure__ __nonnull ((1, 2));

std::strong_ordering sse2_strcmp(const char* s1, const char* s2) noexcept {
    const __m128i zero = _mm_setzero_si128();
    for (size_t r = 0;; r += sizeof(__m128i)) {
        const __m128i x = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(s1 + r));
        const __m128i y = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(s2 + r));
        const unsigned mask = _mm_movemask_epi8(_mm_cmpeq_epi8(x, y));
        const unsigned terminals1 = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, x));
        if (mask == ((1 << sizeof(__m128i)) - 1)) { //sizeof(__m128i)个字符都相等
            if (terminals1 != 0) { //s1中有一个字符为'\0'（故s2此位置也为'\0'），说明s1、s2字符串已读取完
                return std::strong_ordering::equal;
            }
        } else {  //判断s1、s2是否结束
            const unsigned terminals2 = _mm_movemask_epi8(_mm_cmpeq_epi8(zero, y));
            const size_t lastEqIdx = std::countr_one(mask);
            if (terminals1 == 0 || terminals2 == 0) {  //s1、s2都没读取完
                const size_t pos = r + lastEqIdx;
                return s1[pos] <=> s2[pos];
            }
            const size_t terminals1Idx = std::countr_zero(terminals1);
            const size_t terminals2Idx = std::countr_zero(terminals2);
            if (terminals1Idx == terminals2Idx && lastEqIdx >= terminals1Idx) {
                return std::strong_ordering::equal;
            }
            const size_t pos = r + lastEqIdx;
            return s1[pos] <=> s2[pos];
        }
    }
}

std::strong_ordering sse4_strcmp(const char* s1, const char* s2) noexcept {
    const __m128i zero = _mm_setzero_si128();
    for (size_t r = 0;; r += sizeof(__m128i)) {
        const __m128i x = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(s1 + r));
        const __m128i y = _mm_lddqu_si128(reinterpret_cast<const __m128i*>(s2 + r));
        const unsigned mask = _mm_cmpistri(x, y, _SIDD_CMP_EQUAL_EACH | _SIDD_NEGATIVE_POLARITY);
        if (mask == sizeof(__m128i)) { //16个字符都相等
            if (_mm_cmpistrs(x, y, _SIDD_UBYTE_OPS)) { //s1中有一个字符为'\0'（故s2此位置也为'\0'），说明s1、s2字符串已读取完
                return std::strong_ordering::equal;
            }
        } else {  //判断s1、s2是否结束
            const unsigned mask1 = _mm_cmpistri(zero, x, _SIDD_CMP_EQUAL_EACH);
            const unsigned mask2 = _mm_cmpistri(zero, y, _SIDD_CMP_EQUAL_EACH);
            if (mask1 == sizeof(__m128i) || mask2 == sizeof(__m128i)) { //s1、s2都没读取完
                const size_t pos = r + mask;
                return s1[pos] <=> s2[pos];
            }
            if (mask1 == mask2 && mask >= mask1) {
                return std::strong_ordering::equal;
            }
            const size_t pos = r + mask;
            return s1[pos] <=> s2[pos];
        }
    }
}


std::strong_ordering avx2_strcmp(const char* s1, const char* s2) noexcept {
    const __m256i zero = _mm256_setzero_si256();
    for (size_t r = 0;; r += sizeof(__m256i)) {
        const __m256i x = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(s1 + r));
        const __m256i y = _mm256_lddqu_si256(reinterpret_cast<const __m256i*>(s2 + r));
        const unsigned mask = _mm256_movemask_epi8(_mm256_cmpeq_epi8(x, y));
        const unsigned terminals1 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(zero, x));
        if (mask == ((1ull << sizeof(__m256i)) - 1)) { //sizeof(__m128i)个字符都相等
            if (terminals1 != 0) { //s1中有一个字符为'\0'（故s2此位置也为'\0'），说明s1、s2字符串已读取完
                return std::strong_ordering::equal;
            }
        } else {  //判断s1、s2是否结束
            const unsigned terminals2 = _mm256_movemask_epi8(_mm256_cmpeq_epi8(zero, y));
            const size_t lastEqIdx = std::countr_one(mask);
            if (terminals1 == 0 || terminals2 == 0) {  //s1、s2都没读取完
                const size_t pos = r + lastEqIdx;
                return s1[pos] <=> s2[pos];
            }
            const size_t terminals1Idx = std::countr_zero(terminals1);
            const size_t terminals2Idx = std::countr_zero(terminals2);
            if (terminals1Idx == terminals2Idx && lastEqIdx >= terminals1Idx) {
                return std::strong_ordering::equal;
            }
            const size_t pos = r + lastEqIdx;
            return s1[pos] <=> s2[pos];
        }
    }
}


std::strong_ordering avx512_strcmp(const char* s1, const char* s2) noexcept {
    const __m512i zero = _mm512_setzero_si512();
    for (size_t r = 0;; r += sizeof(__m512i)) {
        const __m512i x = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(s1 + r));
        const __m512i y = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(s2 + r));
        const uint64_t mask = _mm512_cmpeq_epi8_mask(x, y);
        const uint64_t terminals1 = _mm512_cmpeq_epi8_mask(zero, x);
        if (mask == std::numeric_limits<uint64_t>::max()) { //sizeof(__m512i)个字符都相等
            if (terminals1 != 0) { //s1中有一个字符为'\0'（故s2此位置也为'\0'），说明s1、s2字符串已读取完
                return std::strong_ordering::equal;
            }
        } else {  //判断s1、s2是否结束
            const uint64_t terminals2 = _mm512_cmpeq_epi8_mask(zero, y);
            const size_t lastEqIdx = std::countr_one(mask);
            if (terminals1 == 0 || terminals2 == 0) {  //s1、s2都没读取完
                const size_t pos = r + lastEqIdx;
                return s1[pos] <=> s2[pos];
            }
            const size_t terminals1Idx = std::countr_zero(terminals1);
            const size_t terminals2Idx = std::countr_zero(terminals2);
            if (terminals1Idx == terminals2Idx && lastEqIdx >= terminals1Idx) {
                return std::strong_ordering::equal;
            }
            const size_t pos = r + lastEqIdx;
            return s1[pos] <=> s2[pos];
        }
    }
}