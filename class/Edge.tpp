#ifndef _EDGE_TPP_
#define _EDGE_TPP_

#include "Net.tpp"
#include "Die.tpp"

class Edge {
private:
public:
    Net *net;
    Die *fromDie, *toDie;
    Edge() : net(), fromDie(), toDie() {}
};

#endif
