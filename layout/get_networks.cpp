#include "LayoutReturn.h"
#include "../class/all.h"
#include <algorithm>

#include "GetNetworkReturn.h"

namespace Layout {

GetNetworkReturn get_network(std::string testcase) {
    // return a GetNetworkReturn instance
    // get_network("2");
    auto [die_count, FPGA_count, adjacent, die_to_FPGA, networks] = layout(testcase);
    int net_count = networks.size(), edge_count = 0;
    for(auto &[net_id, die_s, dies_l, network] : networks) {
        edge_count += network.size();
    }

    GetNetworkReturn res(net_count, edge_count, die_count, FPGA_count);
    auto &[routingGraph, nets, edges, dies, fpgas, nodes] = res;
    
    int edges_cur = 0;

    for(int i = 0; i < die_count; ++i) {
        dies[i].DieId = i;
        nodes[i].die = dies + i;
        routingGraph.dies.emplace_back(&dies[i]);
    }

    for(int i = 0; i < FPGA_count; ++i) {
        fpgas[i].FPGAId = i;
    }

    for(size_t i = 0; i < die_to_FPGA.size(); ++i) {
        int &j = die_to_FPGA[i];
        dies[i].fpga = &fpgas[j];
        fpgas[j].Dies.emplace_back(&dies[i]);
    }

    for(int i = 0; i < net_count; ++i) {
        auto &[net_id, die_s, dies_l, network] = networks[i];
        // nets
        nets[i].netId = net_id;
        // routingGraph
        routingGraph.nets.emplace_back(nets + i);
        
        for(auto [u,v] : network) {
            // nets
            edges[edges_cur] = Edge(nets + i, dies + u, dies + v);
            nets[i].edges.emplace_back(edges + edges_cur);

            // routingGraph
            routingGraph.edges.emplace_back(edges + edges_cur);

            ++edges_cur;
        }

        // nets
        nets[i].sourceNode = nodes + die_s;
        // routingGraph
        routingGraph.nodes.emplace_back(nodes + die_s);
        for(int die_l : dies_l) {
            // nets
            nets[i].sinkNodes.emplace_back(nodes + die_l);
            // routingGraph
            routingGraph.nodes.emplace_back(nodes + die_l);
        }
    }

    std::sort(routingGraph.nodes.begin(), routingGraph.nodes.end());
    routingGraph.nodes.erase(std::unique(routingGraph.nodes.begin(), routingGraph.nodes.end()), routingGraph.nodes.end());

    return res;
}

}
