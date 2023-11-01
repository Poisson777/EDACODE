#include "global.h"

class FPGA;
class Die;
class Troncon;
class Edge;
class Node;
class Graph;
class RoutingGraph;
class Net;
class TDMNet;
class Pin;
class XdrVar;

class FPGA{
public:
    int FPGAId;
    vector<Die*> Dies;
};
class Die{
public:
    int DieId;
    FPGA* FPGA;
    vector<Node*> Nodes;
};
class Troncon{
public :
    int TronconId;
    int limit;
    pair<Die*,Die*> TwoDie;
    vector<TDMNet*> nets;
    vector<XdrVar*> xdrvars;
};
class XdrVar{
public:
    bool forward;
    int XdrVarId;
    double TDMRatio;
    Troncon* troncon;
    vector <Edge*> edges;
};
class Node{
public:
    int NodeId;
    Die* die;
    vector<Edge*> driverEdges;
    vector<Edge*> fanoutEdges;
    double arrivalTime;
    double requireTime;
};
class Net{
public:
    int NetId;
    Node* sourceNode;
    vector<Node*> sinkNodes;
    vector<Edge*> edges;
    vector<Pin*> pins;
};
class Pin{
public:
    Net* net;
    int PinId;
    FPGA* fromFPGA;
    FPGA* toFPGA;
    Die* fromDie;
    Die* toDie;
    char type;
};
class Edge{
public :
    FPGA* fromFPGA;
    FPGA* toFPGA;
    Die* fromDie;
    Die* toDie;
    Node* driverNode;
    Node* fanoutNode;
    char direction;
    int edgeId;
    TDMNet* tdmNet;
    const double TdmA=2;
    const double TdmB=1;
    double delay;
    double constDelay;
    double arrivalTimeAlongEdge;
};
class TDMNet{
public :
    Net* parentNet;
    int TDMNetId;
    bool isIntraNet;
    bool isInterNet;
    Die* fromDie;
    Die* toDie;
    FPGA* fromFPGA;
    FPGA* toFPGA;
    vector<Edge*> edges;
    vector<Pin*> pins;
    Node* sourceNode;
    int driverPinId;
    XdrVar* xdrvar;

};
class RoutingGraph{
public:
    Node* sourceNode;
    Node* sinkNode;
    Graph* graph;
    vector<Net*> nets;
    vector<TDMNet*> tdmNets;
    vector<set<Die*>> netDies;
    vector<set<FPGA*>> netFPGAs;
    vector<double> mu;
    vector<double> lambda;
    vector<Node*> nodes;
    vector<Edge*> edges;
    vector<vector<Node*>>levels;
    vector<vector<Node*>> revlevels;
    vector<vector<Troncon*>> troncons;
    vector<Troncon*> tronconsNetUsed;
    vector< XdrVar*> optXdrVars;
    vector<XdrVar*> xdrvars;
    vector<bool> isoptXdrvar;
    int InterationCount=0;
    double maxChoice;

    void updateTime(){
        //updateArrivalTime
        //resetArrivaltime
        for(auto node:nodes){
            node->requireTime = -1;
        }
        //source->updateArrivalTime
        sourceNode->arrivalTime = max(sourceNode->arrivalTime,0.0);
        //forwardPropagateMT
        for(auto& level:levels){
//                std::mutex idx_mutex;
            int vIdx = 0;
            const int batchSize = 20;
            while(true){
                int startIndex;
                startIndex = vIdx;
                vIdx +=batchSize;
                if(startIndex>level.size()){
                    break;
                }
                for(int j=startIndex;j<min((int)level.size(),startIndex+batchSize);j++){
                    Node* node = level[j];
                    for(auto driverEdge:node->driverEdges){
                        // node->updateArrivalTime(driverEdge->arrivalTimeAlongEdge()
                        node->arrivalTime = max(node->arrivalTime,driverEdge->arrivalTimeAlongEdge);
                    }
                    for(auto fanoutEdge:node->fanoutEdges){
                        //fanoutEdge->updateDelay()
                        fanoutEdge->delay = fanoutEdge->constDelay;
                        if(fanoutEdge->tdmNet->isInterNet){
                            fanoutEdge->delay+= fanoutEdge->TdmA*fanoutEdge->tdmNet->xdrvar->TDMRatio+fanoutEdge->TdmB;
                        }
                        fanoutEdge->arrivalTimeAlongEdge = fanoutEdge->delay+node->arrivalTime;
                    }
                }
            }
        }
        // updateRequireTime
        for(auto node:nodes){
            node->requireTime = 10000000;
        }
//        sinkNode->updateRequireTime(getSinkAT())
        sinkNode->requireTime = min(sinkNode->requireTime,sinkNode->arrivalTime);
        // backPropagateMT
        for(auto revlevel:revlevels){
            for(auto node:revlevel){
                for(auto edge:node->driverEdges){
                    edge->delay= edge->constDelay;
                    if(edge->tdmNet->isInterNet){
                        XdrVar* xdrvar = edge->tdmNet->xdrvar;
                        edge->delay += xdrvar->TDMRatio*edge->TdmA+edge->TdmB;
                    }
                    edge->arrivalTimeAlongEdge = edge->delay+edge->driverNode->arrivalTime;

//                    edge->driverNode->updateRequireTime(requireTime-delay)
                    edge->driverNode->requireTime=min(edge->driverNode->requireTime,edge->driverNode->requireTime-edge->delay);
                }
            }
        }
    }
};
class Graph{
public :
    vector<FPGA*> FPGAs;
    vector<Die*> Dies;
};

