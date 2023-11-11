#include "head.h"
#include "../src/TDMData.cpp"
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
    for(size_t i = 0; i < die_to_FPGA.size(); ++i) {
        const int &j = die_to_FPGA[i];
        dies[i].fpga = &(fpgas[j]);
        fpgas[j].Dies.emplace_back(i);
    }

    std::map<std::pair<int,int>, Edge*> mp;
    for(int i = 0; i < die_count; ++i) {
        for(int j = i + 1; j < die_count; ++j) {
            if(adjacent[i * die_count + j] > 0) {
                // TODO: emplace Edge instances to edges
            }
        }
    }
    for(auto &[net_id, network] : networks) {
        // TODO
    }
}