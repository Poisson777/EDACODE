#ifndef _LAYOUT_RETURN_H_
#define _LAYOUT_RETURN_H_
#include <vector>
#include <string>
struct LayoutReturn {
    int die_count;
    int FPGA_count;
    std::vector<int> adjacent;
    std::vector<int> die_to_FPGA;
    std::vector<std::pair<int, std::vector<std::pair<int,int>>>> networks;
};
LayoutReturn layout(std::string testcase);
#endif
