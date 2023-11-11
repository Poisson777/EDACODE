#ifndef _EASY_VECTOR_PAIR_OUTPUT_H_
#define _EASY_VECTOR_PAIR_OUTPUT_H_
#include <iostream>
#include <vector>
template<class A, class B>
std::ostream& operator <<(std::ostream& out, const std::pair<A, B> &p) {
    return out << "(" << p.first << ", " << p.second << ")";
}
template<class A>
std::ostream& operator <<(std::ostream& out, const std::vector<A> &v) {
    out << "[";
    for(int i = 0; i < (int)v.size(); ++i) {
        if(i) out << ", ";
        out << v[i];
    }
    return out << "]";
}
#endif
