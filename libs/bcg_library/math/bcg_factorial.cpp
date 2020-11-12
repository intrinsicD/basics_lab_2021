//
// Created by alex on 12.11.20.
//

#include "bcg_factorial.h"

namespace bcg{

std::vector<unsigned int> factorial::cache = {1, 1};

factorial::factorial(unsigned int n) {
    value(n);
}

factorial::~factorial(){
    clear();
}

unsigned int factorial::operator()(int n) {
    if(n < 0) return 1;
    return value(n);
}

unsigned int factorial::value(int n){
    if(n < 0) return 1;
    if(n < cache.size()){
        return cache[n];
    }

    unsigned int last = cache.empty() ? 1 : cache.back();
    unsigned int i_start = cache.empty() ? 0 : cache.size() - 1;
    for(unsigned int i = i_start; i < n; ++i){
        last *= (i + 1);
        cache.push_back(last);
    }
    return cache[n];
}

void factorial::clear() {
    cache = {1, 1};
}

}