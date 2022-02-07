#pragma once


#include <bit>
#include <stdexcept>
#include <concepts>
#include <memory_resource>
#include <memory>
#include <x86intrin.h>
#include <fcntl.h>
#include <unistd.h>


void random_buffer(void* buffer, size_t size) {
    if (int fd = open("/dev/urandom", O_RDONLY); read(fd, buffer, size) == size) {
        close(fd);
        return;
    }
    throw std::runtime_error("`/dev/urandom` read error");
}

template<typename T>
T random_integer() requires(std::is_arithmetic_v<T>) {
    T res;
    random_buffer(&res, sizeof(T));
    return res;
}


template<typename T, typename Container = std::pmr::vector<T>>
Container random_integer(size_t n) requires(std::is_arithmetic_v<T>) {
    constexpr size_t alignment = alignof(T);
    auto buffer = new(std::align_val_t{ alignment }) T[n];
    random_buffer(buffer, n * sizeof(T));
    std::pmr::vector<T> res(buffer, buffer + n);
    ::operator delete[](buffer, std::align_val_t{ alignment });
    return res;
}


std::pmr::string random_string(size_t size) {
    if (size == 0) {
        return {};
    }

    constexpr size_t alignment = alignof(__m512i);
    size_t newSize = size <= alignment ? alignment : std::bit_ceil(size);
    auto buffer = new(std::align_val_t{ alignment }) char[newSize];
    random_buffer(buffer, newSize);
    const __m512i mask = _mm512_set1_epi8(0b01111111);
    for (auto p = buffer; p < buffer + newSize; p += alignment) {
        _mm512_store_si512(p, _mm512_and_si512(_mm512_load_si512(p), mask));
    }
    std::pmr::string res(buffer, buffer + size);
    ::operator delete[](buffer, std::align_val_t{ alignment });
    return res;
}

