#ifndef _GET_NETWORK_RETURN_H_
#define _GET_NETWORK_RETURN_H_

#include "../class/RoutingGraph.tpp"

namespace Layout {

struct GetNetworkReturn{
    RoutingGraph routing_graph;
    Net *nets;
    Edge *edges;
    Die *dies;
    FPGA *fpgas;
    Node *nodes;
    std::vector<int> adj;
    GetNetworkReturn(int net_count, int edge_count, int die_count, int fpga_count) {
        nets = new Net[net_count];
        edges = new Edge[edge_count];
        dies = new Die[die_count];
        fpgas = new FPGA[fpga_count];
        nodes = new Node[die_count];
    }
    ~GetNetworkReturn() {
//        delete[] nets;
//        delete[] edges;
//        delete[] dies;
//        delete[] fpgas;
//        delete[] nodes;

    }
};

GetNetworkReturn get_network(std::string testcase);

}

#endif
