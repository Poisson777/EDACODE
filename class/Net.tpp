#ifndef _NET_TPP_
#define _NET_TPP_

#include <vector>
#include "Edge.tpp"
#include "Node.tpp"

class Edge;

class Net {
private:
public:
    int netId;
    std::vector<Edge*> edges;
    Node *sourceNode;
    std::vector<Node*> sinkNodes;
    Net() : edges(), sourceNode(), sinkNodes() {}
};

#endif
