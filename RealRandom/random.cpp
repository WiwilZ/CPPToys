#include <bit>
#include <stdexcept>
#include <concepts>
#include <memory>
#include <x86intrin.h>
#include <fcntl.h>
#include <unistd.h>


void __Random(void* buffer, size_t size) {
    if (int fd = open("/dev/urandom", O_RDONLY); read(fd, buffer, size) == size) {
        close(fd);
        return;
    }
    throw std::runtime_error("`/dev/urandom` read error");
}


template<typename T>
T random_integer() requires(std::is_arithmetic_v<T>) {
    T res;
    __Random(&res, sizeof(T));
    return res;
}


std::shared_ptr<std::byte> random_buffer(size_t size) {
    if (size == 0) {
        return nullptr;
    }

    auto buffer = new std::byte[size];
    __Random(buffer, size);
    return std::shared_ptr<std::byte>{ buffer };
}


std::string random_string(size_t size) {
    if (size == 0) {
        return std::string{};
    }

    constexpr size_t alignment = alignof(__m512i);
    size_t alignedSize = size <= alignment ? alignment : std::bit_ceil(size);
    auto buffer = new(std::align_val_t{ alignment }) char[alignedSize];
    __Random(buffer, alignedSize);
    const __m512i mask = _mm512_set1_epi8(0b01111111);
    for (auto p = buffer; p < buffer + alignedSize; p += alignment) {
        _mm512_store_si512(p, _mm512_and_si512(_mm512_load_si512(p), mask));
    }
    std::string res(buffer, buffer + size);
    ::operator delete[](buffer, std::align_val_t{ alignment });
    return res;
}

