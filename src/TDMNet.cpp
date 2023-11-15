
#include "../class/all.h"
#include "global_head.h"
using namespace std;
class TDMNet;
class RoutingNode;
class RoutingEdge;
class RoutingNode{
    vector<RoutingEdge*> _driverEdges;
    vector<RoutingEdge*> _fanoutEdges;
    double weight = 0;
};
class RoutingEdge{
    RoutingNode* _driverNode;
    RoutingNode* _fanoutNode;
    bool _isInter = false;
    double _delay = 0;
};
class TDMNet{
    Node* _sourceNode;
    Node* _sinkNode;
    vector<Edge*> _edges;
    Net * _net;
    double delay = 0;
    const double A = 2,B=1;
    TDMNet(Net* net,Node* sinkNode){
        _sourceNode = net->sourceNode;
        _sinkNode = sinkNode;
        _net = net;
    }
    void addEdge(Edge* edge){
        _edges.push_back(edge);
    }
    Node* getSink(){return _sinkNode;}
    Node* getSource(){return _sourceNode;}
};
