#ifndef _LAYOUT_RETURN_H_
#define _LAYOUT_RETURN_H_

#include <vector>
#include <string>

struct LayoutReturn {
    int die_count;
    int FPGA_count;
    std::vector<int> adjacent; // adjacent matrix
    // usage: adjacent[i * die_count + j]
    std::vector<int> die_to_FPGA; // die_index -> FPGA_index
    // usage: die_to_FPGA[die_index]
    std::vector<std::pair<int, std::vector<std::pair<int,int>>>> networks;
/*  vector<                              // networks
        pair< network_id,                // network < net_id, edges >
            vector<                      // edges
                pair< from_die, to_die > // edge
            >
        >
    > */
};
LayoutReturn layout(std::string testcase);

#endif
