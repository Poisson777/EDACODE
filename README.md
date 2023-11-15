# EDACODE

due to the large size of testcases, add TestCases manually

file dirctory:

```
.
├── Makefile
├── README.md
├── ReadFileTest.cpp
├── Test.cpp
├── TestCase20231027
│   ├── testcase1/
│   ├── testcase10/
│   ├── testcase2/
│   ├── testcase3/
│   ├── testcase4/
│   ├── testcase5/
│   ├── testcase6/
│   ├── testcase7/
│   ├── testcase8/
│   └── testcase9/
├── abc.cpp
├── class
│   ├── Die.tpp
│   ├── Edge.tpp
│   ├── FPGA.tpp
│   ├── Net.tpp
│   ├── Node.tpp
│   ├── RoutingGraph.tpp
│   └── all.h
├── layout
│   ├── GetNetworkReturn.h
│   ├── LayoutReturn.h
│   ├── Trie.tpp
│   ├── easy_vector_pair_output.h
│   ├── get_networks.cpp
│   ├── head.h
│   └── layout.cpp
├── main.cpp
├── src
│   ├── FileManager.cpp
│   ├── TDMData.cpp
│   ├── TDMRefine.cpp
│   ├── global.h
│   └── global_head.h
└── test.cpp
```

## Test the code

```
make tests
```

## layout usage

```cpp
#include "layout/GetNetworkReturn.h"

void your_func() {
    GetNetworkReturn ret = Layout::get_network("2");
//    ^                                         ^~~ testcase_index, std::string
//    |~~ see ./layout/GetNetworkReturn.h for details
}
```
