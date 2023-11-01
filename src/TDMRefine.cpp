#include "global.h"
#include "TDMData.cpp"
class SwapHist;
class XdrVar;
class SwapHist{
public :
    XdrVar *u;
    XdrVar *v;
    double u_ratio;
    double v_ratio;
    SwapHist(XdrVar *u,XdrVar *v){

    }

};

class TDMRefine{
public:
    vector<SwapHist> hist;
    RoutingGraph* routingGraph;
    TDMRefine(RoutingGraph* routingGraph1){
        routingGraph = routingGraph1;
    }
    bool optimizePath(vector<Edge*>&criticalPath){
        unordered_set<XdrVar*> vars;
        for(auto edge:criticalPath){
            if (!(edge->tdmNet->isIntraNet)){
                vars.insert(edge->tdmNet->xdrvar);
            }
        }
        unordered_map<Troncon*,vector<XdrVar*>> tronconToForwardVars;
        unordered_map<Troncon*,vector<XdrVar*>> tronconToBackVars;
        for(auto var:vars){
            if(var->forward){
                tronconToForwardVars[var->troncon].push_back(var);
            }
            else{
                tronconToBackVars[var->troncon].push_back(var);
            }
        }
        auto getRelateVars = [&](Troncon* troncon,vector<XdrVar *> vars,bool isForward){
            for(auto net:troncon->nets){
                if(net->isInterNet&&net->xdrvar->forward == isForward){
                    bool sameVar = false;
                    for(auto var:vars){
                        if(net->xdrvar==var){
                            sameVar = true;
                            break;
                        }
                    }
                    if(!sameVar) vars.push_back(net->xdrvar);
                }
            }
        };
        for(auto&pair:tronconToForwardVars){
            getRelateVars(pair.first,pair.second,true);
            if (optimizeTronconSort(pair.second, pair.second.size()))
                return true;
        }
        for(auto&pair:tronconToBackVars){
            getRelateVars(pair.first,pair.second,false);
            if (optimizeTronconSort(pair.second, pair.second.size()))
                return true;
        }
        return false;
    }

    bool optimizeTronconSort(vector<XdrVar*>&vars,int numOptVar){
        for(int i=0;i<numOptVar;i++){
            double ratio = vars[i]->TDMRatio;
            unordered_map<double,vector<XdrVar*>> candiateMap;
            for(unsigned j=numOptVar;j<vars.size();j++){
                if(vars[j]->TDMRatio<ratio){
                    candiateMap[vars[j]->TDMRatio].push_back(vars[j]);
                }
            }
            vector<pair<double,XdrVar*>> sortedCandiates;
            for(auto &pair:candiateMap){
                int candiate_ratio = pair.first;
                for(auto var:pair.second){
                    bool canChange = true;
                    vector<Edge*>relateEdges = var->edges;
                    for(auto edge:relateEdges){
                        double slack = edge->fanoutNode->requireTime-edge->fanoutNode->arrivalTime;
                        double delay = edge->constDelay;
                        if(edge->tdmNet->isInterNet) delay+=edge->TdmA*ratio+edge->TdmB;
                        double diffDelay = delay-edge->delay;
                        if(slack<diffDelay){
                            canChange = false;
                            break;
                        }
                    }
                    if(canChange){
                        SwapHist swap1(vars[i],var);
                        if(1) continue;
                        hist.push_back(swap1);
                        vars[i]->TDMRatio = candiate_ratio;
                        var->TDMRatio = ratio;
                        double orginal_arrivalTime ;
                        routingGraph->updateTime();
                        if(routingGraph->sinkNode->arrivalTime>orginal_arrivalTime){
                            vars[i]->TDMRatio = ratio;
                            var ->TDMRatio = candiate_ratio;
                            routingGraph->updateTime();
                            hist.pop_back();
                        }
                        else {
                            return true;
                        }
                    }
                }
            }

        }return false;
    }

};