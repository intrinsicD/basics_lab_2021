//
// Created by alex on 09.10.20.
//

#ifndef BCG_GRAPHICS_BCG_PROPERTY_H
#define BCG_GRAPHICS_BCG_PROPERTY_H

#include <string>
#include <utility>
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

    virtual ~base_handle() = default;

    virtual base_handle &operator=(const base_handle &other) {
        idx = other.idx;
        return *this;
    }

    [[nodiscard]] inline operator size_t() const {
        return idx;
    }

    [[nodiscard]] inline bool is_valid() const {
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

    vertex_handle &operator=(const base_handle &other) override {
        idx = other.idx;
        return *this;
    }
};

struct halfedge_handle : public base_handle {
    using base_handle::base_handle;

    halfedge_handle &operator=(const base_handle &other) override {
        idx = other.idx;
        return *this;
    }
};

struct edge_handle : public base_handle {
    using base_handle::base_handle;

    edge_handle &operator=(const base_handle &other) override {
        idx = other.idx;
        return *this;
    }
};

struct face_handle : public base_handle {
    using base_handle::base_handle;

    face_handle &operator=(const base_handle &other) override {
        idx = other.idx;
        return *this;
    }
};

struct property_container;

struct base_property {
    base_property() = default;

    virtual ~base_property() = default;

    [[nodiscard]] virtual const std::string &name() const = 0;

    [[nodiscard]] virtual size_t dims() const = 0;

    [[nodiscard]] virtual size_t size() const = 0;

    [[nodiscard]] virtual size_t size_bytes() const = 0;

    [[nodiscard]] virtual size_t capacity() const = 0;

    [[nodiscard]] virtual bool empty() const = 0;

    [[nodiscard]] virtual const void *void_ptr() const = 0;

    [[nodiscard]] virtual bool is_dirty() const = 0;

    virtual void set_dirty() = 0;

    virtual void set_clean() = 0;

    virtual void swap(size_t i0, size_t i1) = 0;

    virtual void clear() = 0;

    virtual void free_unused_memory() = 0;

    virtual void reserve(size_t n) = 0;

    virtual void resize(size_t n) = 0;

    virtual void push_back() = 0;
};

inline std::ostream &operator<<(std::ostream &stream, const base_property &property) {
    stream << property.name() << " dims:" << property.dims() << " dirty:" << (property.is_dirty() ? "true" : "false")
           << "\n";
    return stream;
}

template<typename T, int N>
struct property_vector : public base_property {
    using iterator_t = typename std::vector<T>::iterator;
    using const_iterator_t = typename std::vector<T>::const_iterator;
    using reference_t = typename std::vector<T>::reference;
    using const_reference_t = typename std::vector<T>::const_reference;

    explicit property_vector(std::string name, T t = T()) : base_property(),
                                                            property_name(std::move(name)),
                                                            default_value(t),
                                                            dirty(false),
                                                            container() {

    };

    ~property_vector() override = default;

    const std::string &name() const override {
        return property_name;
    }

    inline void swap(size_t i0, size_t i1) override {
        T tmp = container[i0];
        container[i0] = container[i1];
        container[i1] = tmp;
        set_dirty();
    }

    inline void clear() override {
        container.clear();
        set_dirty();
    }

    inline void free_unused_memory() override {
        container.shrink_to_fit();
    }

    inline void reserve(size_t n) override {
        container.reserve(n);
    }

    inline void resize(size_t n) override {
        container.resize(n, default_value);
        set_dirty();
    }

    inline void push_back() override {
        push_back(default_value);
    }

    inline void push_back(const T &value) {
        container.push_back(value);
        set_dirty();
    }

    inline void set_dirty() override { dirty = true; }

    inline void set_clean() override { dirty = false; }

    [[nodiscard]] inline size_t dims() const override { return N; }

    [[nodiscard]] inline size_t size() const override { return container.size(); }

    [[nodiscard]] inline size_t size_bytes() const override { return size() * sizeof(decltype(*data())); }

    [[nodiscard]] inline size_t capacity() const override { return container.capacity(); }

    [[nodiscard]] inline bool empty() const override { return container.empty(); }

    [[nodiscard]] inline const void *void_ptr() const override { return (const void *) data(); }

    [[nodiscard]] inline bool is_dirty() const override { return dirty; }

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

    property(std::shared_ptr<property_vector<T, N>> p = nullptr) : sptr(std::move(p)) {}

    inline operator bool() const {
        return sptr != nullptr;
    }

    bool set(const std::vector<T> &vector) {
        if (*this) {
            sptr->vector() = vector;
            sptr->set_dirty();
            return true;
        }
        return false;
    }

    bool append(const std::vector<T> &vector) {
        if (*this) {
            sptr->vector().insert(sptr->end(), vector.begin(), vector.end());
            sptr->set_dirty();
            return true;
        }
        return false;
    }

    [[nodiscard]] inline bool is_dirty() const {
        return sptr->is_dirty();
    }

    inline void set_dirty() {
        sptr->set_dirty();
    }

    inline void set_clean() {
        sptr->set_clean();
    }

    [[nodiscard]] const std::string &name() const {
        return sptr->name();
    }

    void reset(T value) {
        sptr->vector() = std::vector<T>(sptr->size(), value);
        sptr->set_dirty();
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
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->size();
    }

    [[nodiscard]] inline size_t size_bytes() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->size_bytes();
    }

    [[nodiscard]] inline size_t capacity() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->capacity();
    }

    [[nodiscard]] inline bool empty() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->empty();
    }

    inline void reset_ptr() {
        sptr.reset();
    }

    [[nodiscard]] const void *void_ptr() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->void_ptr();
    }

    [[nodiscard]] inline T *data() {
        return sptr->data();
    }

    [[nodiscard]] inline const T *data() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->data();
    }

    [[nodiscard]] inline reference_t operator[](size_t i) {
        return sptr->operator[](i);
    }

    [[nodiscard]] inline const_reference_t operator[](size_t i) const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->operator[](i);
    }

    [[nodiscard]] inline reference_t operator[](base_handle handle) {
        return sptr->operator[](handle.idx);
    }

    [[nodiscard]] inline const_reference_t operator[](base_handle handle) const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->operator[](handle.idx);
    }

    [[nodiscard]] inline T &back() {
        return sptr->back();
    }

    [[nodiscard]]  inline const T &back() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->back();
    }

    [[nodiscard]] inline iterator_t begin() {
        return sptr->begin();
    }

    [[nodiscard]] inline iterator_t end() {
        return sptr->end();
    }

    [[nodiscard]] inline const_iterator_t begin() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->begin();
    }

    [[nodiscard]] inline const_iterator_t end() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->end();
    }

    [[nodiscard]] inline std::vector<T> &vector() {
        return sptr->vector();
    }

    [[nodiscard]] inline const std::vector<T> &vector() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr)->vector();
    }

    [[nodiscard]] inline std::shared_ptr<base_property> shared_ptr() {
        return sptr;
    }

    [[nodiscard]] inline const std::shared_ptr<const property_vector<T, N>> &shared_ptr() const {
        return std::const_pointer_cast<const property_vector<T, N>>(sptr);
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
operator<<(std::ostream &stream, const property <T, N> &property) {
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

template<typename Derived, typename Handle, typename Container>
struct property_iterator {
    explicit property_iterator(Handle handle = Handle(),
                               const Container *container = nullptr,
                               property<bool, 1> deleted = {}) : handle(handle),
                                                                 deleted(deleted),
                                                                 container(container) {
        if (container) {
            if (deleted) {
                while (container->is_valid(handle) && deleted[handle]) {
                    ++handle;
                }
            }
        }
    }

    inline Handle operator*() const {
        return handle;
    }

    inline bool operator==(const Derived &rhs) const {
        return handle == rhs.handle;
    }

    inline bool operator!=(const Derived &rhs) const {
        return handle != rhs.handle;
    }

    inline property_iterator &operator++() {
        ++handle;
        while (deleted && container->is_valid(handle) && deleted[handle]) {
            ++handle;
        }
        return *this;
    }

    inline property_iterator &operator--() {
        --handle;
        while (deleted && handle.idx > 0 && container->is_valid(handle) &&
               deleted[handle]) {
            --handle;
        }
        return *this;
    }

private:
    Handle handle;
    property<bool, 1> deleted;
    const Container *container;
};

struct property_container {
    std::string name;
    std::unordered_map<std::string, std::shared_ptr<base_property>> container;

    property_container() : name("property_container") {}

    explicit property_container(std::string name) : name(std::move(name)) {}

    struct Iterator : public property_iterator<Iterator, base_handle, property_container> {
        explicit Iterator(base_handle v = base_handle(), property<bool, 1> deleted = {},
                          const property_container *container = nullptr) : property_iterator(v, container,
                                                                                             std::move(deleted)) {}
    };

    [[nodiscard]] inline Iterator begin() const {
        return Iterator(base_handle(0), get<bool, 1>("deleted"), this);
    }

    [[nodiscard]] inline Iterator end() const {
        return Iterator(base_handle(size()), get<bool, 1>("deleted"), this);
    }

    template<typename T, int N>
    bool has(property<T, N> &other) const {
        return has(other.name());
    }

    bool has(const std::string &name) const {
        return container.find(name) != container.end();
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
            return property<T, N>();
        }
        auto sptr = std::make_shared<property_vector<T, N>>(name, t);
        auto n = size();
        if (n > 0) {
            sptr->reserve(n);
            while (sptr->size() < n) {
                sptr->push_back();
            }
        }
        container[name] = sptr;
        return std::dynamic_pointer_cast<property_vector<T, N>>(sptr);
    }

    template<typename T, int N>
    property<T, N> add(property<T, N> &other) {
        auto iter = container.find(other.name());
        if (iter != container.end()) {
            return std::dynamic_pointer_cast<property_vector<T, N>>(iter->second);
        }
        container[other.name()] = other.shared_ptr();
        return std::dynamic_pointer_cast<property_vector<T, N>>(container[other.name()]);
    }

    template<typename T, int N>
    inline property<T, N> get_or_add(const std::string &name, T t = T()) {
        auto d = get<T, N>(name);
        return d ? d : add<T, N>(name, t);
    }

    base_property *get_base_ptr(const std::string &name) const {
        auto iter = container.find(name);
        if (iter == container.end()) {
            return nullptr;
        }
        return iter->second.get();
    }

    bool any_dirty(const std::vector<std::string> &names) const {
        for (const auto &name : names) {
            if (has(name) && get_base_ptr(name)->is_dirty()) {
                return true;
            }
        }
        return false;
    }

    inline void remove(const std::string &name) {
        auto iter = container.find(name);
        if (iter != container.end()) {
            container.erase(iter);
        }
    }

    template<typename T, int N>
    inline void remove(property<T, N> &prop) {
        if (prop) {
            remove(prop.name());
            prop.reset_ptr();
        }
    }

    inline void remove_all() {
        container.clear();
    }

    inline void swap(size_t i0, size_t i1) {
        std::for_each(container.begin(), container.end(), [i0, i1](auto &p) {
            p.second->swap(i0, i1);
        });
    }

    inline void clear() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p.second->clear();
        });
    }

    inline void free_unused_memory() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p.second->free_unused_memory();
        });
    }

    inline void reserve(size_t n) {
        std::for_each(container.begin(), container.end(), [&n](auto &p) {
            p.second->reserve(n);
        });
    }

    inline void resize(size_t n) {
        std::for_each(container.begin(), container.end(), [&n](auto &p) {
            p.second->resize(n);
        });
    }

    inline void push_back() {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p.second->push_back();
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
            return p.second->is_dirty();
        });
    }

    [[nodiscard]] inline bool is_valid(const base_handle &handle) const {
        return handle.idx < size();
    }

    inline void set_dirty() const {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p.second->set_dirty();
        });
    }

    inline void set_clean() const {
        std::for_each(container.begin(), container.end(), [](auto &p) {
            p.second->set_clean();
        });
    }

    inline std::unordered_map<std::string, std::shared_ptr<base_property>> &properties() {
        return container;
    }

    [[nodiscard]] inline const std::unordered_map<std::string, std::shared_ptr<base_property>> &properties() const {
        return container;
    }

    [[nodiscard]] inline std::vector<std::string>
    properties_names(std::vector<int> filter_dims = {}, bool only_dirty = false) const {
        std::vector<std::string> names;
        std::for_each(container.begin(), container.end(), [&names, &filter_dims, &only_dirty](auto &p) {
            bool add = false;
            if (!filter_dims.empty()) {
                for (const auto filter_dim : filter_dims) {
                    if (p.second->dims() == filter_dim) {
                        add = true;
                    }
                }
            } else {
                add = true;
            }
            if (only_dirty) {
                add = false;
                if (p.second->is_dirty()) {
                    add = true;
                }
            }
            if (add) {
                names.push_back(p.second->name());
            }
        });
        return names;
    }

    [[nodiscard]] inline std::vector<std::string> dirty_properties_names() const {
        std::vector<std::string> names;
        std::for_each(container.begin(), container.end(), [&names](auto &p) {
            if (p.second->is_dirty()) {
                names.push_back(p.second->name());
            }
        });
        return names;
    }

    [[nodiscard]] inline size_t num_properties() const {
        return container.size();
    }
};

inline std::ostream &operator<<(std::ostream &stream, const property_container &container) {
    stream << "num properties: " << container.num_properties() << " size: " << container.size() << "\n";
    for (const auto &p : container.container) {
        stream << *p.second;
    }
    return stream;
}

struct vertex_container : public property_container {
    /*using property_container::property_container;

    */
    vertex_container() : property_container("vertices") {}

    struct vertex_iterator : public property_iterator<vertex_iterator, vertex_handle, vertex_container> {
        explicit vertex_iterator(vertex_handle v = vertex_handle(),
                                 property<bool, 1> deleted = {},
                                 const vertex_container *container = nullptr) : property_iterator(std::move(v),
                                                                                                  container,
                                                                                                  std::move(deleted)) {}
    };

    inline vertex_iterator begin() const { return vertex_iterator(0, get<bool, 1>("v_deleted"), this); }

    inline vertex_iterator end() const { return vertex_iterator(size(), get<bool, 1>("v_deleted"), this); }
};

struct halfedge_container : public property_container {
    /*using property_container::property_container;

    */
    halfedge_container() : property_container("halfedges") {}

    struct halfedge_iterator : public property_iterator<halfedge_iterator, halfedge_handle, halfedge_container> {
        explicit halfedge_iterator(halfedge_handle h = halfedge_handle(),
                                   property<bool, 1> deleted = {},
                                   const halfedge_container *container = nullptr) : property_iterator(h, container,
                                                                                                      deleted) {}
    };

    inline halfedge_iterator begin() const { return halfedge_iterator(0, get<bool, 1>("h_deleted"), this); }

    inline halfedge_iterator end() const { return halfedge_iterator(size(), get<bool, 1>("h_deleted"), this); }
};

struct edge_container : public property_container {
    /*using property_container::property_container;

    */
    edge_container() : property_container("edges") {}

    struct edge_iterator : public property_iterator<edge_iterator, edge_handle, edge_container> {
        explicit edge_iterator(edge_handle e = edge_handle(),
                               property<bool, 1> deleted = {},
                               const edge_container *container = nullptr) : property_iterator(std::move(e), container,
                                                                                              std::move(deleted)) {}
    };

    inline edge_iterator begin() const { return edge_iterator(0, get<bool, 1>("e_deleted"), this); }

    inline edge_iterator end() const { return edge_iterator(size(), get<bool, 1>("e_deleted"), this); }
};

struct face_container : public property_container {
    /*using property_container::property_container;

    */
    face_container() : property_container("faces") {}

    struct face_iterator : public property_iterator<face_iterator, face_handle, face_container> {
        explicit face_iterator(face_handle f = face_handle(),
                               property<bool, 1> deleted = {},
                               const face_container *container = nullptr) : property_iterator(std::move(f), container,
                                                                                              std::move(deleted)) {}
    };

    inline face_iterator begin() const { return face_iterator(0, get<bool, 1>("f_deleted"), this); }

    inline face_iterator end() const { return face_iterator(size(), get<bool, 1>("f_deleted"), this); }
};

}

#endif //BCG_GRAPHICS_BCG_PROPERTY_H
