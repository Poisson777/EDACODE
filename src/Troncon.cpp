#include <vector>
#include <map>
class Troncon{
    map<int,Edge*> edges;
public:
    void addEdge(int netId,Edge* edge){
        edges.insert({netId,edge});
    }
};
