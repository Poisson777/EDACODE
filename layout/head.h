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
#include "easy_vector_pair_output.h"

struct LayoutReturn {
    int die_count;
    int FPGA_count;
    std::vector<int> adjacent;
    std::vector<int> die_to_FPGA;
    std::vector<std::pair<int, std::vector<std::pair<int,int>>>> networks;
};

LayoutReturn layout(std::string testcase);
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
    void add(const std::string &s, _Ty val);
    void add(const std::string &s, int l, int r, _Ty val); // [l,r)
    _Ty query(const std::string &s);
    _Ty query(const std::string &s, int l, int r); // [l,r)
};
#include "Trie.tpp"

#endif
