//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_DYNAMIC_BITSET_H
#define BCG_GRAPHICS_BCG_DYNAMIC_BITSET_H

#include <vector>
#include <limits>
#include <string>
#include <algorithm>

namespace bcg {

class DynamicBitset {
public:
    struct reference_t {
        size_t pos;
        DynamicBitset *dbs;

        reference_t(size_t pos, DynamicBitset *dbs) : pos(pos), dbs(dbs) {}

        reference_t &operator=(bool val) {
            val ? dbs->set(pos) : dbs->reset(pos);
            return *this;
        }

        operator bool() const noexcept {
            return dbs->test(pos);
        }
    };

    struct const_reference_t {
        size_t pos;
        const DynamicBitset *dbs;

        const_reference_t(size_t pos, const DynamicBitset *dbs) : pos(pos), dbs(dbs) {}

        operator bool() const noexcept {
            return dbs->test(pos);
        }
    };

    DynamicBitset() : m_set_bits(0), m_storage() {};

    DynamicBitset(size_t number) : m_set_bits(0), m_storage() {
        for (size_t i = 0; i < 64; ++i) {
            if (CHECK_BIT(number, i)) {
                set(i);
            }
        }
    };

    DynamicBitset(std::string number, char one = '1') : m_set_bits(0), m_storage() {
        for (size_t i = 0; i < number.size(); ++i) {
            if (number[i] == one) {
                set(i);
            }
        }
    }

    void clear() {
        m_set_bits = 0;
        m_storage.clear();
    }

    [[nodiscard]] std::string to_string() const {
        std::string number;
        size_t is = item_size();
        for (const auto &item : m_storage) {
            for (size_t i = 0; i < is; ++i) {
                if (CHECK_BIT(item, i)) {
                    number += '1';
                } else {
                    number += '0';
                }
            }
        }
        number.erase(std::find_if(number.rbegin(), number.rend(), [](int ch) {
            return ch != '0';
        }).base(), number.end());
        return number;
    }

    bool operator==(const DynamicBitset &other) const {
        size_t cap = std::min(capacity(), other.capacity());

        for (size_t i = 0; i < cap; ++i) {
            if ((*this)[i] != other[i]) return false;
        }

        if (other.capacity() > cap) {
            return other.is_zero_from(cap);
        }
        return is_zero_from(cap);
    }

    bool operator!=(const DynamicBitset &other) const {
        size_t cap = std::min(capacity(), other.capacity());

        for (size_t i = 0; i < cap; ++i) {
            if ((*this)[i] != other[i]) return true;
        }

        if (other.capacity() > cap) {
            return other.is_not_zero_from(cap);
        }
        return is_not_zero_from(cap);
    }

    [[nodiscard]] bool is_zero_from(size_t pos) const {
        size_t cap = capacity();
        for (size_t i = pos; i < cap; ++i) {
            if ((*this)[i] == 1) return false;
        }
        return true;
    }

    [[nodiscard]] bool is_not_zero_from(size_t pos) const {
        size_t cap = capacity();
        for (size_t i = pos; i < cap; ++i) {
            if ((*this)[i] == 1) return true;
        }
        return false;
    }

    [[nodiscard]] bool all() const {
        return std::all_of(m_storage.begin(), m_storage.end(), [&](const auto i){
            return ~i == 0;
        });
    }

    [[nodiscard]] bool all_of(const std::vector<size_t> &indices) const {
        size_t cap = capacity();
        return std::all_of(indices.begin(), indices.end(), [&](const auto i){
            return i < cap && test(i);
        });
    }

    [[nodiscard]] bool any() const {
        return std::all_of(m_storage.begin(), m_storage.end(), [](const auto i){
            return i != 0;
        });
    }

    [[nodiscard]] bool none() const {
        return std::all_of(m_storage.begin(), m_storage.end(), [](const auto i){
           return i == 0;
        });
    }

    [[nodiscard]] size_t count() const {
        size_t count = 0;
        for (const auto i : m_storage) {
            count += COUNTSETBITS(i);
        }
        return count;
    }

    void resize(size_t s) {
        while (capacity() <= s) {
            m_storage.push_back(0);
        }
        m_storage.shrink_to_fit();
    }

    bool set(size_t i) {
        if (pos_too_large(i)) {
            resize(i);
        }
        size_t vi = vector_index(i);
        size_t ii = item_index(i);
        if (test(vi, ii)) {
            return false;
        }
        SET_BIT(m_storage[vi], ii);
        ++m_set_bits;
        return true;
    }

    bool reset(size_t i) {
        size_t vi = vector_index(i);
        size_t ii = item_index(i);
        if (test(vi, ii)) {
            RESET_BIT(m_storage[vi], ii);
            --m_set_bits;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool test(size_t i) const {
        if (i >= capacity()) return false;
        return test(vector_index(i), item_index(i));
    }

    reference_t operator[](size_t i) { return reference_t(i, this); }

    const_reference_t operator[](size_t i) const { return const_reference_t(i, this); }

    static size_t item_size() {
        return std::numeric_limits<item_t>::digits;
    }

    [[nodiscard]] size_t num_set_bits() const { return m_set_bits; }

    [[nodiscard]] size_t size() const {
        return m_storage.size();
    }

    [[nodiscard]] size_t capacity() const {
        return size() * item_size();
    }

    DynamicBitset &operator&=(const DynamicBitset &other) {
        if (other.size() > size()) {
            resize(other.capacity());
        }
        size_t s = std::min(size(), other.size());
        for (size_t i = 0; i < s; ++i) {
            m_storage[i] &= other.m_storage[i];
        }
        return *this;
    }

    DynamicBitset &operator|=(const DynamicBitset &other) {
        if (other.size() > size()) {
            resize(other.capacity());
        }
        size_t s = std::min(size(), other.size());
        for (size_t i = 0; i < s; ++i) {
            m_storage[i] |= other.m_storage[i];
        }
        return *this;
    }

    DynamicBitset &operator^=(const DynamicBitset &other) {
        if (other.size() > size()) {
            resize(other.capacity());
        }
        size_t s = std::min(size(), other.size());
        for (size_t i = 0; i < s; ++i) {
            m_storage[i] ^= other.m_storage[i];
        }
        return *this;
    }

    DynamicBitset operator&(const DynamicBitset &other) const {
        if (size() < other.size()) {
            DynamicBitset new_bitset = *this;
            return new_bitset.operator&=(other);
        } else {
            DynamicBitset new_bitset = other;
            return new_bitset.operator&=(*this);
        }
    }

    DynamicBitset operator|(const DynamicBitset &other) const {
        if (size() < other.size()) {
            DynamicBitset new_bitset = *this;
            return new_bitset.operator|=(other);
        } else {
            DynamicBitset new_bitset = other;
            return new_bitset.operator|=(*this);
        }
    }

    DynamicBitset operator^(const DynamicBitset &other) const {
        if (size() < other.size()) {
            DynamicBitset new_bitset = *this;
            return new_bitset.operator^=(other);
        } else {
            DynamicBitset new_bitset = other;
            return new_bitset.operator^=(*this);
        }
    }

    DynamicBitset operator~() const {
        DynamicBitset new_bitset = *this;
        return new_bitset.flip();
    }

    DynamicBitset operator<<(size_t pos) const {
        size_t cap = capacity();
        DynamicBitset new_bitset;
        new_bitset.resize(cap - pos);

        for (size_t i = 0; i < new_bitset.capacity() - pos; ++i) {
            new_bitset[i] = test(i + pos);
        }

        return new_bitset;
    }

    DynamicBitset &operator<<=(size_t pos) {
        *this = operator<<(pos);
        return *this;
    }

    DynamicBitset operator>>(size_t pos) const {
        DynamicBitset new_bitset;
        new_bitset.resize(capacity() + pos);

        for (size_t i = 0; i < capacity() - pos; ++i) {
            new_bitset[i + pos] = test(i);
        }
        return new_bitset;
    }

    DynamicBitset &operator>>=(size_t pos) {
        *this = operator>>(pos);
        return *this;
    }

    DynamicBitset &flip() {
        for (auto &i : m_storage) {
            i = ~i;
        }
        return *this;
    }

    DynamicBitset &reset() {
        for (auto &i : m_storage) {
            i = 0;
        }
        return *this;
    }

    DynamicBitset &flip(size_t pos) {
        reference_t(pos, this) = !reference_t(pos, this);
        return *this;
    }

private:
    using item_t = unsigned char;

    template<typename T>
    [[nodiscard]] inline T BIT(T pos) const {
        return 1 << pos;
    }

    template<typename T, typename U>
    inline void SET_BIT(T &var, U pos) {
        var |= BIT(pos);
    }

    template<typename T, typename U>
    [[nodiscard]] inline T DROP(T var, U pos) const {
        return var >> pos;
    }

    template<typename T, typename U>
    [[nodiscard]] inline bool CHECK_BIT(T var, U pos) const {
        return DROP(var, pos) & 1;
    }

    template<typename T, typename U>
    inline void RESET_BIT(T &var, U pos) {
        var &= ~BIT(pos);
    }

    static size_t COUNTSETBITS(size_t var) {
        var = var - ((var >> 1) & 0x5555555555555555UL);
        var = (var & 0x3333333333333333UL) + ((var >> 2) & 0x3333333333333333UL);
        return (size_t) ((((var + (var >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
    }

    [[nodiscard]] inline bool pos_too_large(size_t pos) const {
        return pos >= capacity();
    }

    [[nodiscard]] inline bool test(size_t vi, size_t ii) const {
        return CHECK_BIT(m_storage[vi], ii);
    }

    static inline size_t vector_index(size_t pos) { return pos / item_size(); }

    static inline size_t item_index(size_t pos) { return pos % item_size(); }

    size_t m_set_bits;
    std::vector<item_t> m_storage;
};

using dynamic_bitset = DynamicBitset;

}

#endif //BCG_GRAPHICS_BCG_DYNAMIC_BITSET_H
