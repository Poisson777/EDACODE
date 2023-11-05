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
void Trie<_Ty>::add(std::string s, _Ty val) {
    // add string to tire, with val in final node
    int now = 0, n = s.length();
    for(int i = 0; i < n; ++i) {
        int &next = node[now].next[s[i]];
        if(next == 0) {
            // warning: may init value with int(0), which also refers to die0
            next = new_node();
        }
        now = next;
    }
    node[now].belonging = val;
}

template<class _Ty>
_Ty Trie<_Ty>::query(std::string s) {
    // warning: same as add()
    int now = 0, n = s.length();
    for(int i = 0; i < n; ++i) {
        int &next = node[now].next[s[i]];
        assert(next != 0);
        now = next;
    }
    return node[now].belonging;
}
