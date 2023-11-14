// temporary test .cpp file
#include "layout/head.h"
#include "layout/GetNetworkReturn.h"

extern int sqrt(int);

int main() {
    assert(sqrt(16384) == 128);
    std::cout << "sqrt() test passed.\n";

    Trie<int> trie;
    
    trie.add("abc123", 1);
    std::string s1 = "abc123";
    assert(trie.query(s1) == 1);
    s1.pop_back();
    assert(trie.query(s1) == 0);
    std::cout << "Trie class test passed.\n";

    auto res = Layout::get_network("2");
    std::cout << "Layout::get_network() test passed.\n";
}