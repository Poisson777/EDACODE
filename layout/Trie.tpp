#ifndef _TRIE_TPP_
#define _TRIE_TPP_
#include "head.h"

template<class _Ty>
int Trie<_Ty>::new_node() {
    // return the index of the new node
    int ret = node.size();
    node.emplace_back(Node());
    return ret;
}

template<class _Ty>
Trie<_Ty>::Trie() {
    node.emplace_back(Node());
}

template<class _Ty>
void Trie<_Ty>::add(const std::string &s, _Ty val) {
    Trie<_Ty>::add(s, 0, s.length(), val);
}

template<class _Ty>
void Trie<_Ty>::add(const std::string &s, int l, int r, _Ty val) {
    // add string to tire, with val in final node
    int now = 0;
    for(int i = l; i < r; ++i) {
        int &next = node[now].next[s[i]];
        if(next == 0) {
            // warning: may init value with int(0), which also refers to die0
            next = new_node();
        }
        now = next;
    }
    assert(node[now].belonging == 0);
    node[now].belonging = val;
}

template<class _Ty>
_Ty Trie<_Ty>::query(const std::string &s) {
    return Trie<_Ty>::query(s, 0, s.length());
}

template<class _Ty>
_Ty Trie<_Ty>::query(const std::string &s, int l, int r) {
    // warning: same as add()
    if(r == -1) r = s.length();
    int now = 0;
    for(int i = l; i < r; ++i) {
        int &next = node[now].next[s[i]];
        assert(next != 0);
        now = next;
    }
    return node[now].belonging;
}
#endif
