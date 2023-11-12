#ifndef _EDGE_TPP_
#define _EDGE_TPP_

#include "Net.tpp"
#include "Die.tpp"

class Net;

class Edge {
private:
public:
    Net *net;
    Die *fromDie, *toDie;
    Edge() : net(), fromDie(), toDie() {}
    Edge(Net *net, Die *fromDie, Die *toDie) : net(net), fromDie(fromDie), toDie(toDie) {}
};

#endif
