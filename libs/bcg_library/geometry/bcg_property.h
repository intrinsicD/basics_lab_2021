//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_H
#define BCG_GRAPHICS_BCG_PROPERTY_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <limits>
#include <iostream>
#include <sstream>
#include <algorithm>

namespace bcg {

constexpr size_t BCG_INVALID_ID = std::numeric_limits<size_t>::max();

struct base_handle {
    size_t idx;

    base_handle(size_t idx = BCG_INVALID_ID) : idx(idx) {

    }

    base_handle &operator=(const base_handle &other) {
        idx = other.idx;
    }

    [[nodiscard]] inline operator bool() const {
        return idx != BCG_INVALID_ID;
    }

    [[nodiscard]] inline bool operator==(const base_handle &other) const {
        return idx == other.idx;
    }

    [[nodiscard]] inline bool operator!=(const base_handle &other) const {
        return idx != other.idx;
    }

    [[nodiscard]] inline bool operator<(const base_handle &rhs) const {
        return idx < rhs.idx;
    }

    [[nodiscard]] inline bool operator>(const base_handle &rhs) const {
        return idx > rhs.idx;
    }

    inline void operator++() {
        ++idx;
    }

    inline void operator--() {
        idx == 0 ? idx = 0 : --idx;
    }
};

struct vertex_handle : public base_handle {
    using base_handle::base_handle;

    vertex_handle(base_handle handle) : base_handle(handle) {

    }

    vertex_handle &operator=(const base_handle &other) {
        idx = other.idx;
    }
};

struct halfedge_handle : public base_handle {
    using base_handle::base_handle;

    halfedge_handle(base_handle handle) : base_handle(handle) {

    }

    halfedge_handle &operator=(const base_handle &other) {
        idx = other.idx;
    }
};

struct edge_handle : public base_handle {
    using base_handle::base_handle;

    edge_handle(base_handle handle) : base_handle(handle) {

    }

    edge_handle &operator=(const base_handle &other) {
        idx = other.idx;
    }
};

struct face_handle : public base_handle {
    using base_handle::base_handle;

    face_handle(base_handle handle) : base_handle(handle) {

    }

    face_handle &operator=(const base_handle &other) {
        idx = other.idx;
    }
};

struct property_container;

struct base_property {
    enum class SignedType {
        Signed,
        Unsigned,
        Other
    };

    enum class FundamentalType {
        Bool,
        Char,
        Integer,
        Long,
        FloatingPoint,
        Other
    };

    std::pair<SignedType, FundamentalType> scalar_type;

    template<typename T>
    struct to_type {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Other, FundamentalType::Other};
    };

    template<>
    struct to_type<char> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Signed, FundamentalType::Char};
    };

    template<>
    struct to_type<unsigned char> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Unsigned, FundamentalType::Char};
    };

    template<>
    struct to_type<int> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Signed, FundamentalType::Integer};
    };

    template<>
    struct to_type<unsigned int> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Unsigned, FundamentalType::Integer};
    };

    template<>
    struct to_type<long> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Signed, FundamentalType::Long};
    };

    template<>
    struct to_type<unsigned long> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Unsigned, FundamentalType::Long};
    };

    template<>
    struct to_type<float> {
        std::pair<SignedType, FundamentalType> type_t = {SignedType::Other, FundamentalType::FloatingPoint};
    };

    explicit base_property(const std::pair<SignedType, FundamentalType> &scalar_type) : scalar_type(scalar_type) {

    };

    virtual ~base_property() = default;

    [[nodiscard]] virtual const std::string &name() const = 0;

    [[nodiscard]] virtual size_t dims() const = 0;

    [[nodiscard]] virtual size_t size() const = 0;

    [[nodiscard]] virtual size_t size_bytes() const = 0;

    [[nodiscard]] virtual size_t capacity() const = 0;

    [[nodiscard]] virtual bool empty() const = 0;

    [[nodiscard]] virtual const void *void_ptr() const = 0;

    [[nodiscard]] virtual bool is_dirty() const = 0;

    virtual void set_clean() = 0;

    virtual void swap(size_t i0, size_t i1) = 0;

    virtual void clear() = 0;

    virtual void free_unused_memory() = 0;

    virtual void reserve(size_t n) = 0;

    virtual void resize(size_t n) = 0;

    virtual void push_back() = 0;
};

template<typename T>
std::string to_string(const base_property &property) {
    std::stringstream ss;
    T *ptr = (T *) property.void_ptr();

    for (size_t i = 0; i < property.size(); ++i) {
        for (size_t j = 0; j < property.dims(); ++j) {
            ss << *(ptr + i * property.dims() + j) << " ";
        }
        ss << "\n";
    }
    return ss.str();
}


template<typename T, int N>
struct property_vector : public base_property {
    using iterator_t = typename std::vector<T>::iterator;
    using const_iterator_t = typename std::vector<T>::const_iterator;
    using reference_t = typename std::vector<T>::reference;
    using const_reference_t = typename std::vector<T>::const_reference;

    explicit property_vector(const std::string &name, T t = T()) : base_property(base_property::to_type<T>::type_t),
                                                                   property_name(name),
                                                                   default_value(t),
                                                                   dirty(false),
                                                                   container() {

    };

    ~property_vector() override = default;

    inline void swap(size_t i0, size_t i1) override {
        std::swap(container[i0], container[i1]);
        dirty = true;
    }

    inline void clear() override {
        container.clear();
        dirty = true;
    }

    inline void free_unused_memory() override {
        container.shrink_to_fit();
    }

    inline void reserve(size_t n) override {
        container.reserve(n);
    }

    inline void resize(size_t n) override {
        container.resize(n, default_value);
        dirty = true;
    }

    inline void push_back() override {
        push_back(default_value);
    }

    inline void push_back(const T &value) {
        container.push_back(value);
        dirty = true;
    }

    [[nodiscard]] inline size_t dims() const override { return N; }

    [[nodiscard]] inline size_t size() const override { return container.size(); }

    [[nodiscard]] inline size_t size_bytes() const override { return size() * sizeof(decltype(*data())); }

    [[nodiscard]] inline size_t capacity() const override { return container.capacity(); }

    [[nodiscard]] inline bool empty() const override { return container.empty(); }

    [[nodiscard]] inline const void *void_ptr() const override { return (const void *) data(); }

    [[nodiscard]] inline T *data() { return container.data(); }

    [[nodiscard]] inline const T *data() const { return container.data(); }

    [[nodiscard]] inline reference_t operator[](size_t i) { return container[i]; }

    [[nodiscard]] inline const_reference_t operator[](size_t i) const { return container[i]; }

    [[nodiscard]] inline T &back() { return container.back(); }

    [[nodiscard]] inline const T &back() const { return container.back(); }

    [[nodiscard]] inline iterator_t begin() { return container.begin(); }

    [[nodiscard]] inline iterator_t end() { return container.end(); }

    [[nodiscard]] inline const_iterator_t begin() const { return container.begin(); }

    [[nodiscard]] inline const_iterator_t end() const { return container.end(); }

    [[nodiscard]] inline std::vector<T> &vector() { return container; }

    [[nodiscard]] inline const std::vector<T> &vector() const { return container; }

protected:
    std::string property_name;
    T default_value;
    bool dirty;
    std::vector<T> container;
};

// specialization for bool properties
template<>
[[nodiscard]] inline const bool *property_vector<bool, 1>::data() const {
    return nullptr;
}

template<typename T, int N>
struct property {
    using reference_t = typename property_vector<T, N>::reference_t;
    using const_reference_t = typename property_vector<T, N>::const_reference_t;
    using iterator_t = typename property_vector<T, N>::iterator_t;
    using const_iterator_t = typename property_vector<T, N>::const_iterator_t;

    property(std::shared_ptr<property_vector<T, N>> p = nullptr) : sptr(p) {}

    inline operator bool() const {
        return sptr != nullptr;
    }

    bool set(const std::vector<T> &vector) {
        if (*this) {
            sptr->vector() = vector;
            sptr->dirty = true;
            return true;
        }
        return false;
    }

    bool append(const std::vector<T> &vector) {
        if (*this) {
            sptr->vector().insert(sptr->end(), vector.begin(), vector.end());
            sptr->dirty = true;
            return true;
        }
        return false;
    }

    inline bool is_dirty() const {
        return sptr->dirty;
    }

    inline void set_dirty() {
        sptr->dirty = true;
    }

    inline void set_clean() {
        sptr->set_clean();
    }

    const std::string &name() const {
        return sptr->name();
    }

    void reset(T value) {
        sptr->vector() = std::vector<T>(sptr->size(), value);
        sptr->dirty = true;
    }

    inline void swap(size_t i0, size_t i1) {
        sptr->swap(i0, i1);
    }

    inline void clear() {
        sptr->clear();
    }

    inline void free_unused_memory() {
        sptr->free_unused_memory();
    }

    inline void reserve(size_t n) {
        sptr->reserve(n);
    }

    [[nodiscard]] inline size_t dims() const {
        return N;
    }

    [[nodiscard]] inline size_t size() const {
        return std::const_pointer_cast<const base_property>(sptr)->size();
    }

    [[nodiscard]] inline size_t size_bytes() const {
        return std::const_pointer_cast<const base_property>(sptr)->size_bytes();
    }

    [[nodiscard]] inline size_t capacity() const {
        return std::const_pointer_cast<const base_property>(sptr)->capacity();
    }

    [[nodiscard]] inline bool empty() const {
        return std::const_pointer_cast<const base_property>(sptr)->empty();
    }

    inline void reset_ptr() {
        sptr.reset();
    }

    [[nodiscard]] const void *void_ptr() const {
        return std::const_pointer_cast<const base_property>(sptr)->void_ptr();
    }

    [[nodiscard]] inline T *data() {
        return sptr->data();
    }

    [[nodiscard]] inline const T *data() const {
        return std::const_pointer_cast<const base_property>(sptr)->data();
    }

    [[nodiscard]] inline reference_t operator[](size_t i) {
        return sptr->operator[](i);
    }

    [[nodiscard]] inline const_reference_t operator[](size_t i) const {
        return std::const_pointer_cast<const base_property>(sptr)->operator[](i);
    }

    [[nodiscard]] inline reference_t operator[](base_handle handle) {
        return sptr->operator[](handle.idx);
    }

    [[nodiscard]] inline const_reference_t operator[](base_handle handle) const {
        return std::const_pointer_cast<const base_property>(sptr)->operator[](handle.idx);
    }

    [[nodiscard]] inline T &back() {
        return sptr->back();
    }

    [[nodiscard]]  inline const T &back() const {
        return std::const_pointer_cast<const base_property>(sptr)->back();
    }

    [[nodiscard]] inline iterator_t begin() {
        return sptr->begin();
    }

    [[nodiscard]] inline iterator_t end() {
        return sptr->end();
    }

    [[nodiscard]] inline const_iterator_t begin() const {
        return std::const_pointer_cast<const base_property>(sptr)->begin();
    }

    [[nodiscard]] inline const_iterator_t end() const {
        return std::const_pointer_cast<const base_property>(sptr)->end();
    }

    [[nodiscard]] inline std::vector<T> &vector() {
        return sptr->vector();
    }

    [[nodiscard]] inline const std::vector<T> &vector() const {
        return std::const_pointer_cast<const base_property>(sptr)->vector();
    }

    [[nodiscard]] inline std::shared_ptr<base_property> &shared_ptr() {
        return sptr;
    }

    [[nodiscard]] inline const std::shared_ptr<const base_property> &shared_ptr() const {
        return std::const_pointer_cast<const base_property>(sptr);
    }

    inline void resize(size_t n) {
        sptr->resize(n);
    }

private:
    friend property_container;

    inline void push_back() {
        sptr->push_back();
    }

    std::shared_ptr<property_vector<T, N>> sptr;
};

template<typename T, int N>
typename std::enable_if<N == 1, std::ostream>::type &
operator<<(std::ostream &stream, const property<T, N> &property) {
    if (property) {
        stream << property.name() << "\n";
        if (property.size() < 5) {
            for (const auto &item : property) {
                stream << item << "\n";
            }
        } else {
            stream << property[0] << "\n";
            stream << property[1] << "\n";
            stream << "..." << "\n";
            stream << property[property.size() - 2] << "\n";
            stream << property[property.size() - 1] << "\n";
        }
    } else {
        stream << "property " << property.name() << " is not bound!\n";
    }

    return stream;
}

template<typename T, int N>
typename std::enable_if<N >= 2, std::ostream>::type &
operator<<(std::ostream &stream, const property <T, N> &property) {
    if (property) {
        stream << property.name() << "\n";
        if (property.size() < 5) {
            for (const auto &item : property) {
                for (size_t j = 0; j < N; ++j) {
                    stream << item[j] << " ";
                }
                stream << "\n";
            }
        } else {
            for (size_t j = 0; j < N; ++j) {
                stream << property[0][j] << " ";
            }
            stream << "\n";
            for (size_t j = 0; j < N; ++j) {
                stream << property[1][j] << " ";
            }
            stream << "\n..." << "\n";
            for (size_t j = 0; j < N; ++j) {
                stream << property[property.size() - 2][j] << " ";
            }
            stream << "\n";
            for (size_t j = 0; j < N; ++j) {
                stream << property[property.size() - 1][j] << " ";
            }
            stream << "\n";
        }
    } else {
        stream << "property " << property.name() << " is not bound!\n";
    }

    return stream;
}

struct property_container {
    std::unordered_map<std::string, std::shared_ptr<base_property>> container;

    struct Iterator {
        explicit Iterator(base_handle handle = base_handle(), const property_container *container = nullptr) : m_handle(
                handle),
                                                                                                               m_container(
                                                                                                                       container) {
            if (m_container && deleted) {
                deleted = m_container->get<bool, 1>("deleted");
                if (deleted) {
                    while (m_container->is_valid(m_handle) && deleted[m_handle]) {
                        ++m_handle;
                    }
                }
            }
        }

        inline base_handle operator*() const {
            return m_handle;
        }

        inline bool operator==(const Iterator &rhs) const {
            return m_handle == rhs.m_handle;
        }

        inline bool operator!=(const Iterator &rhs) const {
            return m_handle != rhs.m_handle;
        }

        inline Iterator &operator++() {
            ++m_handle;
            while (deleted && m_container->is_valid(m_handle) && deleted[m_handle]) {
                ++m_handle;
            }
            return *this;
        }

        inline Iterator &operator--() {
            --m_handle;
            while (deleted && m_handle.idx > 0 && m_container->is_valid(m_handle) &&
                   deleted[m_handle]) {
                --m_handle;
            }
            return *this;
        }

    private:
        base_handle m_handle;
        property<bool, 1> deleted;
        const property_container *m_container;
    };

    [[nodiscard]] inline Iterator begin() const {
        return Iterator(base_handle(0), this);
    }

    [[nodiscard]] inline Iterator end() const {
        return Iterator(base_handle(size()), this);
    }

    template<typename T, int N>
    property<T, N> get(const std::string &name) const {
        auto iter = container.find(name);
        if (iter == container.end()) {
            return property<T, N>();
        }
        return std::dynamic_pointer_cast<property_vector<T, N>>(iter->second);
    }

    template<typename T, int N>
    property<T, N> add(const std::string &name, T t = T()) {
        auto iter = container.find(name);
        if (iter != container.end()) {
            return nullptr;
        }
        container[name] = std::make_shared<property_vector<T, N>>(name, t);
        return std::dynamic_pointer_cast<property_vector<T, N>>(container[name]);
    }

    template<typename T, int N>
    property<T, N> add(property<T, N> &other) {
        auto iter = container.find(other.name);
        if (iter != container.end()) {
            return nullptr;
        }
        container[other.name] = other.shared_ptr();
        return std::dynamic_pointer_cast<property_vector<T, N>>(container[other.name]);
    }

    template<typename T, int N>
    inline property<T, N> get_or_add(const std::string &name, T t = T()) {
        auto d = get<T, N>(name);
        return d ? d : add<T, N>(name, t);
    }

    const base_property *get_base_ptr(const std::string &name) const {
        auto iter = container.find(name);
        if (iter == container.end()) {
            return nullptr;
        }
        return iter->second.get();
    }

    inline void remove(const std::string &name) {
        auto iter = container.find(name);
        if (iter != container.end()) {
            container.erase(iter);
        }
    }

    template<typename T, int N>
    inline void remove(const property<T, N> &prop) {
        remove(prop.name());
    }

    inline void remove_all() {
        container.clear();
    }

    inline void swap(size_t i0, size_t i1) {
        std::for_each(container.begin(), container.end(), [i0, i1](auto &p) {
            p->swap(i0, i1);
        });
    }

    inline void clear() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p->clear();
        });
    }

    inline void free_unused_memory() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p->free_unused_memory();
        });
    }

    inline void reserve(size_t n) {
        std::for_each(container.begin(), container.end(), [&n](auto &p) {
            p->reserve(n);
        });
    }

    inline void resize(size_t n) {
        std::for_each(container.begin(), container.end(), [&n](auto &p) {
            p->resize(n);
        });
    }

    inline void push_back() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p->push_back();
        });
    }

    [[nodiscard]] inline bool empty() const {
        return container.empty();
    }

    [[nodiscard]] inline size_t size() const {
        if (!empty()) {
            return container.begin()->second->size();
        }
        return 0;
    }

    [[nodiscard]] inline bool is_dirty() const {
        return std::any_of(container.begin(), container.end(), [](const auto &p) {
            return p->is_dirty();
        });
    }

    [[nodiscard]] inline bool is_valid(base_handle handle) const {
        return handle.idx < size();
    }

    inline void set_dirty() const {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p->set_dirty();
        });
    }

    inline void set_clean() const {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p->set_clean();
        });
    }

    inline std::unordered_map<std::string, std::shared_ptr<base_property>> &properties() {
        return container;
    }

    [[nodiscard]] inline const std::unordered_map<std::string, std::shared_ptr<base_property>> &properties() const {
        return container;
    }

    [[nodiscard]] inline std::vector<std::string> properties_names() const {
        std::vector<std::string> names;
        std::transform(container.begin(), container.end(), std::back_inserter(names),
                       [](const auto &p) {
                           return p->name();
                       }
        );
        return names;
    }

    [[nodiscard]] inline std::vector<std::string> dirty_properties_names() const {
        std::vector<std::string> names;
        std::for_each(container.begin(), container.end(), [&names](auto &p) {
            if (p->is_dirty()) {
                names.push_back(p->name());
            }
        });
        return names;
    }

    [[nodiscard]] inline size_t num_properties() const {
        return container.size();
    }
};

template<>
std::string to_string<bool>(const base_property &prop) {
    std::stringstream ss;
    std::shared_ptr<const base_property> p(&prop);
    property<bool, 1> P(std::dynamic_pointer_cast<property_vector<bool, 1>>(p));

    for (size_t i = 0; i < prop.size(); ++i) {
        ss << P[i] << "\n";
    }
    return ss.str();
}

std::ostream &operator<<(std::ostream &stream, const base_property &prop) {
    if (prop.scalar_type.second == base_property::FundamentalType::Char) {
        if (prop.scalar_type.first == base_property::SignedType::Unsigned) {
            stream << to_string<unsigned char>(prop);
        } else {
            stream << to_string<char>(prop);
        }
        return stream;
    }
    if (prop.scalar_type.second == base_property::FundamentalType::Integer) {
        if (prop.scalar_type.first == base_property::SignedType::Unsigned) {
            stream << to_string<unsigned int>(prop);
        } else {
            stream << to_string<int>(prop);
        }
        return stream;
    }
    if (prop.scalar_type.second == base_property::FundamentalType::Long) {
        if (prop.scalar_type.first == base_property::SignedType::Unsigned) {
            stream << to_string<unsigned long>(prop);
        } else {
            stream << to_string<long>(prop);
        }
        return stream;
    }
    if (prop.scalar_type.second == base_property::FundamentalType::FloatingPoint) {
        stream << to_string<float>(prop);
        return stream;
    }
    if (prop.scalar_type.second == base_property::FundamentalType::Bool) {
        stream << to_string<bool>(prop);
        return stream;
    }
    return stream;
}

std::ostream &operator<<(std::ostream &stream, const property_container &container) {
    stream << "num properties: " << container.num_properties() << "\n";
    for (const auto &p : container.container) {
        stream << p.second << "\n";
    }
    return stream;
}


}

#endif //BCG_GRAPHICS_BCG_PROPERTY_H
