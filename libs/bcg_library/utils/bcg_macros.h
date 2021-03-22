//
// Created by Alex on 20.03.21.
//

#ifndef BCG_GRAPHICS_BCG_MACROS_H
#define BCG_GRAPHICS_BCG_MACROS_H

namespace bcg{

/*
if (__cplusplus == 201703L) std::cout << "C++17\n";
else if (__cplusplus == 201402L) std::cout << "C++14\n";
else if (__cplusplus == 201103L) std::cout << "C++11\n";
else if (__cplusplus == 199711L) std::cout << "C++98\n";
else std::cout << "pre-standard C++\n";
*/

#define BCG_GE_CPP17 __cplusplus >= 201703L
#define BCG_GE_CPP14 __cplusplus >= 201402L
#define BCG_GE_CPP11 __cplusplus >= 201103L
#define BCG_GE_CPP98 __cplusplus >= 199711L

#if BCG_GE_CPP17
#define BCG_MAYBE_UNUSED [[maybe_unused]]
#else
#define BCG_MAYBE_UNUSED (void*) &
#endif
}

#endif //BCG_GRAPHICS_BCG_MACROS_H
