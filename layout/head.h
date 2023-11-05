#ifndef _HEAD_H_
#define _HEAD_H_
#include <fstream>
#include <cassert>
#include <vector>
#include <iostream>
#include <functional>
#include <queue>
#include <map>
#include <string>

void layout();
int sqrt(int);
template<class _Ty>
class Trie {
    struct Node {
        _Ty belonging;
        std::map<char,int> next;
        Node() : belonging(), next() {}
        Node(_Ty val) : belonging(val), next() {}
    };
private:
    std::vector<Node> node;
    int new_node();
public:
    Trie();
    void add(std::string s, _Ty val);
    _Ty query(std::string s);
};
#include "Trie.tpp"

#endif
