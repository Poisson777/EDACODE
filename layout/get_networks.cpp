#include "LayoutReturn.h"
#include "global_head.h"
#include "../class/all.h"
GetNetworkReturn get_network(std::string testcase) {
    // get_network("2");
    auto [die_count, FPGA_count, adjacent, die_to_FPGA, networks] = layout(testcase);
    GetNetworkReturn res;
    RoutingGraph routing_graph;
    std::vector<FPGA> fpgas(FPGA_count);
    std::vector<Die> dies(die_count);
    std::vector<Edge> edges;
    for(int i = 0; i < FPGA_count; ++i) fpgas[i].FPGAId = i;
    for(int i = 0; i < die_count; ++i) dies[i].DieId = i;
    
    

    for(auto &[net_id, network] : networks) {
        // TODO
    }
}