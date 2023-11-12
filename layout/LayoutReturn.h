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
    struct NetAttr{
        int net_id;
        int src_die;
        std::vector<int> sink_die;
        std::vector<std::pair<int,int>> edges;
        NetAttr(int net_id, int src_die, std::vector<int> sink_die, std::vector<std::pair<int,int>> edges) : 
            net_id(net_id), src_die(src_die), sink_die(sink_die), edges(edges) {}
    };
    std::vector<NetAttr> networks;
/*  vector<                              // networks
        NetAttr<                         // network
            net_id,                      // network's id
            src_die,                     // source die node
            vector< sink_die >,          // sink die nodes
            vector<                      // edges
                pair< from_die, to_die > // edge
            >
        >
    > */
};

LayoutReturn layout(std::string testcase);

#endif
