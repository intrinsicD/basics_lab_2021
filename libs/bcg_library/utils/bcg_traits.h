//
// Created by alex on 19.04.21.
//

#ifndef BCG_GRAPHICS_BCG_TRAITS_H
#define BCG_GRAPHICS_BCG_TRAITS_H
namespace bcg {


struct NO {
};

template<typename T, typename Arg>
NO operator==(const T &, const Arg &);

template<typename T, typename Arg = T>
struct EqualExists {
    enum {
        value = !std::is_same<decltype(std::declval<T>() < std::declval<Arg>()), NO>::value
    };
};

template<typename T, typename Arg>
NO operator+=(const T &, const Arg &);

template<typename T, typename Arg = T>
struct PlusEqualExists {
    enum {
        value = !std::is_same<decltype(std::declval<T>() < std::declval<Arg>()), NO>::value
    };
};

template<typename T, typename Arg>
NO operator-=(const T &, const Arg &);

template<typename T, typename Arg = T>
struct MinusEqualExists {
    enum {
        value = !std::is_same<decltype(std::declval<T>() < std::declval<Arg>()), NO>::value
    };
};

template<typename T, typename Arg>
NO operator*=(const T &, const Arg &);

template<typename T, typename Arg = T>
struct MultEqualExists {
    enum {
        value = !std::is_same<decltype(std::declval<T>() < std::declval<Arg>()), NO>::value
    };
};

template<typename T, typename Arg>
NO operator/=(const T &, const Arg &);

template<typename T, typename Arg = T>
struct DivEqualExists {
    enum {
        value = !std::is_same<decltype(std::declval<T>() < std::declval<Arg>()), NO>::value
    };
};
}

#endif //BCG_GRAPHICS_BCG_TRAITS_H
