#ifndef _ROUTING_GRAPH_TPP_
#define _ROUTING_GRAPH_TPP_

#include "Net.tpp"
#include "Edge.tpp"
#include "Node.tpp"
#include "Die.tpp"

class RoutingGraph {
private:
public:
    std::vector<Net*> nets;
    std::vector<Edge*> edges;
    std::vector<Node*> nodes;
    std::vector<Die*> dies;

    RoutingGraph() : nets(), edges(), nodes(), dies() {}
};

#endif
