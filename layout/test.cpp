// temporary test .cpp file
extern void layout();
#include "head.h"

int main() {
    assert(sqrt(16384) == 128);
    std::cout << "sqrt() test passed.\n";

    Trie<int> trie;
    trie.add("abc123", 1);
    assert(trie.query("abc123" == 1));
    assert(trie.query("abc12" == 0));
    std::cout << "Trie class test passed.\n";

    // layout();
}