# EDACODE

due to the large size of testcases, add TestCases manually

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
