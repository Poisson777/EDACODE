#ifndef _GLOBAL_HEAD_H_
#define _GLOBAL_HEAD_H_
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <math.h>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <fstream>

#include "class/RoutingGraph.tpp";

struct GetNetworkReturn{
    RoutingGraph routing_graph;
    Net *nets;
    Edge *edges;
    Die *dies;
    FPGA *fpgas;
    Node *nodes;
    GetNetworkReturn(int net_count, int edge_count, int die_count, int fpga_count) {
        nets = new Net[net_count];
        edges = new Edge[edge_count];
        dies = new Die[die_count];
        fpgas = new FPGA[fpga_count];
        nodes = new Node[die_count];
    }
    ~GetNetworkReturn() {
        delete[] nets;
        delete[] edges;
        delete[] dies;
        delete[] fpgas;
        delete[] nodes;
    }
};
#endif