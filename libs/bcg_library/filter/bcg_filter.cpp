//
// Created by alex on 20.05.21.
//

#include "bcg_filter.h"

namespace bcg{

filter::iterator_t filter::begin(){
    return filtered.begin();
}

filter::iterator_t filter::end(){
    return filtered.end();
}

filter::const_iterator_t filter::begin() const{
    return filtered.begin();
}

filter::const_iterator_t filter::end() const{
    return filtered.end();
}

void filter::clear() {
    filtered.clear();
}

}