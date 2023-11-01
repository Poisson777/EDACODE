#pragma once // 防止头文件被多次包含
#include <iostream>


#include "src/global.h"
#include "src/TDMData.cpp"
#include "src/TDMRefine.cpp"
using namespace std;

int main() {
    //读入数据获取的Graph
    Graph* graph;
    vector<FPGA*> fpgas = graph->FPGAs;
    RoutingGraph* routinggraph;

    // getTDMNets
    map<int,vector<Net*>> crossingCountToNets;
    vector<TDMNet*> TDMNets;
    for(auto net:routinggraph->nets){
        int crossingCount = routinggraph->netFPGAs[net->NetId].size();
        crossingCountToNets[crossingCount].push_back(net);
        set<FPGA*> crossingFPGAs = routinggraph->netFPGAs[net->NetId];
        int count = crossingFPGAs.size();
        // 在同一个FPGA里
        if(count==1){
            TDMNet* tdmNet = new TDMNet();
            tdmNet->isIntraNet = true;
            for(auto edge:net->edges) tdmNet->edges.push_back(edge);
            FPGA* FPGA = *crossingFPGAs.begin();
            tdmNet->fromFPGA = FPGA;
            tdmNet->toFPGA = FPGA;
            TDMNets.push_back(tdmNet);
        }
        else{
            vector<TDMNet *> subNets(count,NULL);
            unordered_map<FPGA*,int > FPGAToSubNets;
            int subNetId = 0;
            for(auto FPGA:crossingFPGAs){
                TDMNet* tdmNet;
                //初始化Net
//                _parentNet = parentNet;
//                _intraNet = false;
//                _interNet = false;
//                _driverIdx = -1;
//                _xdrVar = NULL;
//                _troncon = NULL;
                tdmNet->toFPGA = FPGA;
                FPGAToSubNets[FPGA] = subNetId;
                subNetId++;
            }
            int driverSubnetId = -1;
            int driverFPGAId = -1;

            for(auto pin:net->pins){
                int subNetId = FPGAToSubNets[pin->fromFPGA];
                if(pin->type == 'o'){
                    driverSubnetId = subNetId;
                    driverFPGAId = pin->fromFPGA->FPGAId;
                    subNets[driverSubnetId]->driverPinId = subNets[driverSubnetId]->pins.size();
                }
                subNets[driverSubnetId]->pins.push_back(pin);
            }

            Pin* driverPin = subNets[driverSubnetId]->pins[subNets[driverSubnetId]->driverPinId];
            for(int j=0;j<count;j++){
                subNets[j]->isInterNet = (j==driverSubnetId);
                if(j!=driverSubnetId){
                    subNets[j]->pins.push_back(driverPin);

                }
                FPGA* fpga = graph->FPGAs[driverFPGAId];
                subNets[j]->fromFPGA = fpga;
            }
            for(auto tdmNet:subNets){
                if(tdmNet->pins.size()!=1) TDMNets.push_back(tdmNet);
            }
        }

    }

    //getTroncon Xdrvar
    // 这里省略SLL的赋值
    // Troncon的赋值from to net  Xdrvar赋值 from to Id forward
    // 删除没有用的Troncon
    vector<vector<Troncon*>> troncons=routinggraph->troncons;
    vector<Troncon*> tronconNetUsed = routinggraph->tronconsNetUsed;
    vector<XdrVar*> optXdrvars = routinggraph->optXdrVars;
    int index = 0;
    for(int i=0;i<fpgas.size();i++){
        for(int j=i+1;j<fpgas.size();j++){
            Troncon* troncon = troncons[i][j];
            if(troncon->nets.size()>0){
                troncon->TronconId = index++;
                routinggraph->tronconsNetUsed.push_back(troncon);
            }
        }
    }
    int tronconCount = tronconNetUsed.size();
    //get TDMRatio
    vector<int> TDMRatio ={1};
    for(int i=1;i<1000;i++) TDMRatio.push_back(i*4);
    for(auto troncon:tronconNetUsed){
        if(troncon->nets.size()>troncon->limit){
            for(auto net:troncon->nets){
                optXdrvars.push_back(net->xdrvar);
            }
        }
        else {
            for(auto net:troncon->nets){
                net->xdrvar->TDMRatio = 1;
            }
        }
    }
    vector<bool> isoptXdrVar = routinggraph->isoptXdrvar;
    isoptXdrVar.assign(routinggraph->optXdrVars.size(),false);
    for(auto var:routinggraph->optXdrVars){
        isoptXdrVar[var->XdrVarId] = true;
    }
    double bestCost = 0;
    int bestIter = -1;

    vector<int> bestVal(routinggraph->InterationCount,0);
    vector<vector<Node*>>&levels = routinggraph->levels;
    vector<vector<Node*>>&revlevels = routinggraph->revlevels;
    for(int i=0;i<routinggraph->InterationCount;i++){
        if(i==0) {
            //initMu
            vector<double> nodePrecXdr(routinggraph->nodes.size(),0);
            vector<double> edgePreXdr(routinggraph->edges.size(),0);
            vector<double> &mu = routinggraph->mu;
            vector<double> lambda = routinggraph->lambda;
            for(auto level:levels){
                for(auto node:level){
                    for(auto driverEdge:node->driverEdges){
                        edgePreXdr[driverEdge->edgeId] += isoptXdrVar[driverEdge->tdmNet->xdrvar->XdrVarId];
                        nodePrecXdr[node->NodeId] += edgePreXdr[driverEdge->edgeId];
                    }
                    for(auto fanoutEdge:node->fanoutEdges){
                        edgePreXdr[fanoutEdge->edgeId] = nodePrecXdr[node->NodeId];
                    }
                }
            }
            for(auto revlevel:revlevels){
                for(auto node:revlevel){
                    double fanoutSum = 0;
                    if(node==routinggraph->sinkNode) {
                        fanoutSum=1;
                    }
                    else{
                        for(auto fanoutEdge:node->fanoutEdges){
                            fanoutSum+= mu[fanoutEdge->edgeId];
                        }
                    }
                    if(nodePrecXdr[node->NodeId]==0){
                        for(auto driverEdge:node->driverEdges){
                            mu[driverEdge->edgeId] = fanoutSum/node->driverEdges.size();
                        }
                    }
                    else {
                        for(auto driverEdge:node->driverEdges){
                            mu[driverEdge->edgeId] =
                                    fanoutSum * edgePreXdr[driverEdge->edgeId]/nodePrecXdr[node->NodeId];
                        }
                    }
                }
            }
            //initLambada
            vector<Edge*> &edges = routinggraph->edges;
            for(auto troncon:tronconNetUsed){
                double sum = 0;
                double maxTempSum = 0;
                for(auto xdrvar:troncon->xdrvars){
                    double tempSum =0;
                    for(auto edge:edges){
                        tempSum+= edge->TdmA*mu[edge->edgeId];
                    }
                    sum+= sqrt(tempSum);
                    maxTempSum = max(maxTempSum,tempSum);
                }
                double& lambd = lambda[troncon->TronconId];
                lambd = pow(sum/troncon->limit,2);
                lambd = max(lambd,maxTempSum);
            }
            //isMulegal
            for(auto node:routinggraph->nodes){
                double sum1 = 0;
                double sum2 = 0;
                for(auto driverEdge:node->driverEdges){
                    sum1 += mu[driverEdge->edgeId];
                }
                for(auto fanoutEdge:node->fanoutEdges){
                    sum2 += mu[fanoutEdge->edgeId];
                }
                if(node==routinggraph->sinkNode){
                    if(abs(sum1-1)>0.0001) {
                        //终止执行
                    }
                }
                else if(node==routinggraph->sourceNode){
                    if(abs(sum1-sum2)>0.0001) {
                        //终止执行
                    }
                }
            }
        }
        else{
            //getRatio(iter)
            double ratio = 0;
            double baseRate = 0.2;
            double changeRate = 0.01;
            ratio = baseRate*pow(0.5,changeRate*i);
            //updateMu
            for(auto revlevel:revlevels){
                int minIndex = 0;
                const int batchSize = 20;
                while(true){
                    int idx = minIndex;
                    minIndex += batchSize;
                    if(idx>=revlevel.size()){
                        break;
                    }
                    for(int k = idx;k<min((int)revlevel.size(),minIndex);k++){
                        Node* node = revlevel[k];
                        int driverEdgeSize = node->driverEdges.size();
                        vector<pair<Edge*,double>> gradients;
                        for(int d=0;d<driverEdgeSize;d++){
                            Edge * edge = node->driverEdges[d];
                            gradients.emplace_back(edge,
                                                   edge->arrivalTimeAlongEdge-edge->fanoutNode->arrivalTime);
                        }
                        int numCriMu = 0;
                        for(auto& driverEdge:node->driverEdges){
                            Node* fanoutNode = driverEdge->fanoutNode;
                            Node* driverNode = driverEdge->driverNode;
                            numCriMu +=
                                    fanoutNode->arrivalTime==driverNode->arrivalTime+driverEdge->delay;
                        }
                        double fanoutSum = 0;
                        if(node==routinggraph->sinkNode){
                            fanoutSum=1;
                        }
                        else {
                            for(auto fanoutEdge:node->fanoutEdges){
                                fanoutSum+=routinggraph->mu[fanoutEdge->edgeId];
                            }
                        }
                        double driverSum = 0;
                        for(auto driverEdge:node->driverEdges){
                            driverSum+=routinggraph->mu[driverEdge->edgeId];
                        }
                        if(node==routinggraph->sinkNode){
                            if(numCriMu==driverEdgeSize) {
                                continue;
                            }
                            //sinkFlow(driverSum,fanoutSum,gradients)
                            const double maxDiffratio=0.05;
                            const double maxNumratio = 0.01;
                            const double maxMuIncrTosumratio = 0.002;
                            double gradientSum = 0;
                            for(auto &pair:gradients){
                                gradientSum +=pair.second;
                            }
                            sort(gradients.begin(), gradients.end(), [&](pair<Edge *, double> p1, pair<Edge *, double> p2) {
                                Edge *e1 = p1.first;
                                Edge *e2 = p2.first;
                                return e1->arrivalTimeAlongEdge > e2->arrivalTimeAlongEdge;
                            });
                            Edge *critEdge = gradients.front().first;
                            double threshold = critEdge->arrivalTimeAlongEdge*(1-maxDiffratio);
                            int maxNum = max(1.0,gradients.size()*maxNumratio);
                            int lastIncrId = -1;
                            double curMuSum = 0;
                            double curDeltSum = 0;
                            for(int d=0;d<maxNum&&gradients[d].first->arrivalTimeAlongEdge>=threshold;d++){
                                Edge *edge = gradients[d].first;
                                double mu = routinggraph->mu[edge->edgeId];
                                if(curDeltSum+mu*ratio>(driverSum-curMuSum-mu)*maxMuIncrTosumratio){
                                    if(d==0){
                                        ratio = (driverSum-mu)*maxMuIncrTosumratio/mu;
                                    }
                                    else{
                                        break;
                                    }
                                }
                                lastIncrId =d;
                                curMuSum+=mu;
                                curDeltSum+=mu*ratio;
                            }
                            int nIncr=0;
                            for(int d=0;d<=lastIncrId;d++){
                                Edge* edge = gradients[d].first;
                                double &mu = routinggraph->mu[edge->edgeId];
                                double delta = mu*ratio;
                                if(delta>0) nIncr++;
                                mu = mu+delta;
                                driverSum+=delta;
                                gradientSum-=gradients[d].second;
                            }
                            // decreaseFlow driverSum fanoutSum,gradients,lastIncrIdx+1
                        }
                        else if(numCriMu==driverEdgeSize){
                            //critFlow(node,driverSum,fanouwSum)
                            auto sortedDriverEdges = node->driverEdges;
                            sort(sortedDriverEdges.begin(), sortedDriverEdges.end(), [&](Edge *e1, Edge *e2) {
                                return routinggraph->mu[e1->edgeId] < routinggraph->mu[e2->edgeId];
                            });
                            double diff = fanoutSum-driverSum;
                            for(int d=0;d<driverEdgeSize;d++){
                                double &mu = routinggraph->mu[sortedDriverEdges[d]->edgeId];
                                if(driverSum!=0){
                                    mu=mu+diff*(mu/driverSum);
                                }
                                else {
                                    mu=mu+diff/(driverEdgeSize-d);
                                    diff-=diff/(driverEdgeSize-d);
                                    if(mu<0){
                                        diff+=mu;
                                        mu=0;
                                    }
                                }
                            }
                        }
                        else if(driverSum>fanoutSum){
                            //devreaseFlow(driverSum,fanoutSum,gradients,0,node)
                            double diff = fanoutSum-driverSum;
                            double muSum = 0;

                            for(int d=0;d<driverEdgeSize;d++){
                                muSum+=routinggraph->mu[gradients[d].first->edgeId];
                            }
                            sort(gradients.begin() , gradients.end(), [&](pair<Edge *, double> p1, pair<Edge *, double> p2) {
                                return p1.second < p2.second;
                            });
                            double maxAbsGradient = abs(gradients[0].second);
                            int lastIndex = driverEdgeSize;
                            for(int d=driverEdgeSize-1;d>=0;d--){
                                if(routinggraph->mu[gradients[d].first->edgeId]*gradients[d].second!=0){
                                    break;
                                }
                                else {
                                    lastIndex =d;
                                }
                            }
                            double sum=0;
                            for(int d=0;d<driverEdgeSize;d++){
                                sum+= routinggraph->mu[gradients[d].first->edgeId]*(abs(gradients[d].second)/maxAbsGradient);
                            }
                            if(lastIndex==0){
                                for(int d=0;d<driverEdgeSize;d++){
                                    double&mu = routinggraph->mu[gradients[d].first->edgeId];
                                    mu+=diff*(mu/muSum);
                                    mu = max(mu,0.0);
                                }
                                //return;
                            }
                            else{
                                double ratio = abs(diff)/sum;
                                int curIndex = 0;
                                while(ratio>1){
                                    double& mu = routinggraph->mu[gradients[curIndex].first->edgeId];
                                    diff+=mu;
                                    sum-=mu;
                                    muSum-=mu;
                                    mu=0;
                                    curIndex++;
                                    if(muSum==0) break;
                                    if(curIndex==lastIndex){
                                        for(int d=curIndex;d<driverEdgeSize;d++){
                                            double&mu = routinggraph->mu[gradients[d].first->edgeId];
                                            mu+=diff*(mu/muSum);
                                            mu=max(mu,0.0);
                                        }
                                        break;
                                    }
                                    sum*= maxAbsGradient;
                                    maxAbsGradient =abs(gradients[curIndex].second);
                                    sum/=maxAbsGradient;
                                    ratio=abs(diff)/sum;
                                }
                                if(muSum!=0&&curIndex!=lastIndex){
                                    for(int d = curIndex;d<driverEdgeSize;d++){
                                        double& mu = routinggraph->mu[gradients[d].first->edgeId];
                                        mu+=mu*ratio*(gradients[d].second/maxAbsGradient);
                                        mu = max(mu,0.0);
                                    }
                                }
                            }
                        }
                        else if(driverSum<fanoutSum){
                            //increaseFlow(node,driverSum,fanoutSum)
                            double diff = fanoutSum-driverSum;
                            double critSum=0;
                            int numCritMu = 0;
                            sort(node->driverEdges.begin(), node->driverEdges.end(), [&](Edge *edge1, Edge *edge2) {
                                return edge1->arrivalTimeAlongEdge > edge2->arrivalTimeAlongEdge;
                            });
                            const double maxDiffRatio = 0.05;

                            double threshold = node->driverEdges[0]->arrivalTimeAlongEdge*(1-maxDiffRatio);
                            for(auto driverEdge:node->driverEdges){
                                if(driverEdge->arrivalTimeAlongEdge>=threshold){
                                    critSum += routinggraph->mu[driverEdge->edgeId];
                                    numCriMu++;
                                }
                                else{
                                    break;
                                }
                            }
                            for(auto driverEdge:node->driverEdges){
                                if(driverEdge->arrivalTimeAlongEdge>=threshold){
                                    critSum += routinggraph->mu[driverEdge->edgeId];
                                    numCriMu++;
                                }
                                else{
                                    break;
                                }
                            }
                        }
                        //removeAccIssue(node)
                        double driverSum1 = 0;
                        double fanoutSum1 = 0;
                        if(node== routinggraph->sinkNode){
                            fanoutSum1=  1;
                        }
                        else if(node != routinggraph->sourceNode){
                            for(auto driverEdge:node->driverEdges){
                                driverSum1 = routinggraph->mu[driverEdge->edgeId];
                            }
                            for(auto fanoutEdge:node->fanoutEdges){
                                fanoutSum1 = routinggraph->mu[fanoutEdge->edgeId];
                            }
                            if(driverSum1!=fanoutSum1){
                                double diff = fanoutSum1-driverSum1;
                                double maxDriverMu = -1;
                                Edge *maxDriver = NULL;
                                for(auto driverEdge:node->driverEdges){
                                    double val = abs(routinggraph->mu[driverEdge->edgeId]);
                                    if(val>maxDriverMu){
                                        maxDriverMu = val;
                                        maxDriver = driverEdge;
                                    }
                                }
                                double &mu = routinggraph->mu[maxDriver->edgeId];
                                mu+=diff;
                                if(mu<0) mu=0;
                            }
                        }

                    }

                }
            }
            //updateLambda
            for(auto&troncon:routinggraph->tronconsNetUsed){
                vector<double> muVec;
                double sum =0;
                for(auto var:troncon->xdrvars){
                    double tmpSum =0;
                    for(auto edge:var->edges){
                        tmpSum+=edge->TdmA*routinggraph->mu[edge->edgeId];
                    }
                    muVec.push_back(sqrt(tmpSum));
                    sum+=muVec.back();
                }
                sort(muVec.begin(), muVec.end());
                if(muVec.back()/muVec.front()>routinggraph->maxChoice){
                    break;
                }
                double &lambda = routinggraph->lambda[troncon->TronconId];
                lambda = pow(sum/troncon->limit,2);
                lambda= max(lambda,muVec.back());

            }
        }
        //solveLRS
        int cnt1=0;
        int cnt2=0;
        int cnt3=0;
        for(auto var:routinggraph->optXdrVars){
            Troncon* troncon = var->troncon;
            double &lambda = routinggraph->lambda[troncon->TronconId];
            double sum = 0;
            for(auto edge:var->edges){
                sum+=edge->TdmA*routinggraph->mu[edge->edgeId];
            }
            if(sum!=0){
                double newVal = sqrt(lambda/sum);
                if(newVal<1||newVal>routinggraph->maxChoice){
                    int nCrit = 0;
                    for(auto edge:var->edges){
                        nCrit+=edge->fanoutNode->arrivalTime==edge->driverNode->arrivalTime+edge->delay;
                    }
                }
                if(newVal<1){
                    var->TDMRatio=1;
                    cnt1++;
                }
                else if(newVal>routinggraph->maxChoice){
                    var->TDMRatio =routinggraph->maxChoice;
                }
                else {
                    var->TDMRatio = newVal;
                }
            }
        }
        routinggraph->updateTime();

//        double dualVal = computeDual(setting.computeDual);
        double result = 0;
            result+=routinggraph->sinkNode->arrivalTime;
            for(auto &troncon:routinggraph->tronconsNetUsed){
                double usage = 0;
                for(auto net:troncon->nets){
                    double ratio = net->xdrvar->TDMRatio;
                    usage += 1/ratio;
                }
                result +=routinggraph->lambda[troncon->TronconId]*(usage-troncon->limit);
            }
            for(Edge* edge:routinggraph->edges){
                double&mu = routinggraph->mu[edge->edgeId];
                result += mu*(edge->arrivalTimeAlongEdge-edge->fanoutNode->arrivalTime);
            }
            double dualVal = result;
            double primVal = routinggraph->sinkNode->arrivalTime;
            const int interval = 70;
            if(i-interval>=0&&bestVal[i-interval]-bestCost<1){
                break;
            }
            if(routinggraph->sinkNode->arrivalTime<bestCost){
                bestIter = i;
                //
                bestCost = routinggraph->sinkNode->arrivalTime;
            }
            bestVal[i] = bestCost;
    }
    // double dual = computeDual(true)
    double result =0;
    result += routinggraph->sinkNode->arrivalTime;
    for(auto &troncon:routinggraph->tronconsNetUsed){
        double usage = 0;
        for(auto net:troncon->nets){
            double ratio = net->xdrvar->TDMRatio;
            usage += 1/ratio;
        }
        result +=routinggraph->lambda[troncon->TronconId]*(usage-troncon->limit);
    }
    for(Edge* edge:routinggraph->edges){
        double&mu = routinggraph->mu[edge->edgeId];
        result += mu*(edge->arrivalTimeAlongEdge-edge->fanoutNode->arrivalTime);
    }
    double dual = result;
    //updateArrivalTime
    double primal = routinggraph->sinkNode->arrivalTime;
//    dual=max(dual,computeDual(true));
    // greedyRefine
        //updateTiming
            // updateArrivalTime
            //update RequireTime
        Node*sink = routinggraph->sinkNode;
        vector<Edge*> criticalPath;
        while(sink->driverEdges.size()>0){
            for(auto driverEdge:sink->driverEdges){
                if(driverEdge->fanoutNode->arrivalTime==driverEdge->driverNode->arrivalTime+driverEdge->delay){
                    sink = driverEdge->driverNode;
                    criticalPath.push_back(driverEdge);
                }
            }
        }
        int iter = 0;
        double bestCost1 = 1000000;
        TDMRefine tdmRefine(routinggraph);
        while(tdmRefine.optimizePath(criticalPath)){
            if(iter%10==0){
                if(bestCost1<=routinggraph->sinkNode->arrivalTime){
                    break;
                }
                else {
                    bestCost1=routinggraph->sinkNode->arrivalTime;
                }
            }
            vector<Edge*> path;
            Node* sinkNode = routinggraph->sinkNode;
            while(sinkNode->driverEdges.size()>0){
                for(auto driverEdge:sinkNode->driverEdges){
                    if(driverEdge->fanoutNode->arrivalTime==driverEdge->delay+driverEdge->driverNode->arrivalTime){
                        sinkNode = driverEdge->driverNode;
                        path.push_back(driverEdge);
                        break;
                    }
                }
            }
            criticalPath=path;
            iter++;

        }

    return 0;
}