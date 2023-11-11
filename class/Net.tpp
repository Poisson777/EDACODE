#ifndef _NET_TPP_
#define _NET_TPP_

#include <vector>
#include "Edge.tpp"
#include "Node.tpp"

class Net {
private:
public:
    int netId;
    std::vector<Edge*> edges;
    Node *sourceNode;
    std::vector<Node*> sinkNode;
    Net() : edges(), sourceNode(), sinkNode() {}
};

#endif
