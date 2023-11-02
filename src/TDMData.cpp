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
    int FPGAId{};
    vector<Die*> Dies;

};
class Die{
public:
    int DieId{};
    FPGA* fpga{};
    vector<Node*> Nodes;

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
class Graph{
public :
    map<int,FPGA*> FPGAs;
    map<int,Die*> Dies;
    map<int,Node*> Nodes;
    void constructGraph(const map<int,vector<int>>& die_position,
                        const map<int,vector<int>>& fpga_die){
        for(const auto& pair:die_position){
            int die_id = pair.first;
            Die* die = new Die();
            die->DieId = die_id;
            for(auto node_id:pair.second){
                Node *node = new Node();
                node->NodeId = node_id;
                node->die = die;
                Nodes.insert(make_pair(node_id,node));
                die->Nodes.push_back(node);
            }
            Dies.insert(make_pair(die_id,die));
        }
        for(const auto& pair:fpga_die){
            int fpga_id = pair.first;
            FPGA* fpga = new FPGA();
            fpga->FPGAId = fpga_id;
            for(auto die_id:pair.second){

                Die* die = Dies[die_id];
                die->fpga = fpga;
                fpga->Dies.push_back(die);
            }
            FPGAs.insert(make_pair(fpga_id,fpga));
        }
    }
    void toString(){
//        for(auto pair:FPGAs){
//            cout<<"FPGAID"<<pair.first<<endl;
//            for(auto die:pair.second->Dies){
//                cout<<"DieID"<<die->DieId<<endl;
//                for(auto node:die->Nodes){
//                    cout<<" Node"<<node->NodeId;
//                }
//                cout<<endl;
//            }
//        }
    }
};

class RoutingGraph{
public:
    Graph* graph;
    vector<Net*> nets;
    vector<TDMNet*> tdmNets;
    Node* sourceNode;
    Node* sinkNode;
    vector<set<Die*>> netDies;
    vector<set<FPGA*>> netFPGAs;
    vector<double> mu;
    vector<double> lambda;
    vector<Node*> nodes;
    vector<Edge*> edges;
    vector<vector<Node*>>levels;
    vector<vector<Node*>> revlevels;
    vector<vector<Troncon*>> troncon_array;
    vector<Troncon*> tronconsNetUsed;
    vector< XdrVar*> optXdrVars;
    vector<XdrVar*> xdrvars;
    vector<bool> isoptXdrvar;
    int InterationCount=0;
    double maxChoice;
    void initRoutingGraph(Graph* g,vector<pair<int,vector<int>>> source_sink,
                          vector<vector<int>> wires){
        this->graph = g;
        for(auto pair:source_sink){
            Net* net = new Net();
            net->NetId = nets.size();
            net->sourceNode = graph->Nodes[pair.first];
            for(auto sink_node_id:pair.second){
                Node* sink_node = graph->Nodes[sink_node_id];
                net->sinkNodes.push_back(sink_node);
            }
            nets.push_back(net);
        }
        for(int i=0;i<wires.size();i++){
            vector<Troncon*> trs;
            for(int j=i;j<wires.size();j++) {
                if (wires[i][j] != 0) {
                    auto *tr = new Troncon();
                    tr->TronconId = 0;
                    tr->limit = wires[i][j];
                    tr->TwoDie =
                            make_pair(graph->Dies[i], graph->Dies[j]);
                    trs.push_back(tr);
                }
            }
            if(!trs.empty()) {
                for(auto tr:trs){
                    tronconsNetUsed.push_back(tr);
                }
            }
            troncon_array.push_back(trs);
        }
    }
    void net_toString(){
        cout<<"------------------Nets---------------"<<endl;
        for(auto net:nets){
            cout<<net->NetId<<endl;
            cout<<"SourceNode"<<net->sourceNode->NodeId<<" Die"<<net->sourceNode->die->DieId<<endl;
            for(auto sinkNode1:net->sinkNodes){
                cout<<"SinkNode"<<sinkNode1->NodeId<<" Die"<<sinkNode1->die->DieId<<endl;
            }
        }
        cout<<"--------------------SLL---------------"<<endl;
        for(auto troncons:troncon_array){
            for(auto troncon:troncons){
                cout<<troncon->limit;
                cout<< " Die"<<troncon->TwoDie.first->DieId;
                cout<< " Die"<<troncon->TwoDie.second->DieId<<endl;
            }
        }
    }
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

