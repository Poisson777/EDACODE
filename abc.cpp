#pragma once // 防止头文件被多次包含
#include <iostream>

#include <vector>
#include "src/global.h"
#include "../untitled/TDMD.cpp"

using namespace std;

int main() {
    //读入数据获取的Graph
    Graph* graph = new Graph();
    //布线图
    RoutingGraph* routinggraph = new RoutingGraph();
    //TDMNets
    vector<TDMNet*> TDMNets;
    //这边要在初始化函数中初始化对象数组大小
    for(int i=0;i<routinggraph->routingNets.size();i++){
        Net* net = routinggraph->routingNets[i];
        set<int>* crossingFPGAs = routinggraph->netFPGAs[i];
        int count =0;
        for(auto pin:net->pins){
            count += crossingFPGAs->insert(pin->FPGAId).second;
        }
        if(count==1){
            //tdmNet赋值初始化
            TDMNet* tdmNet = new TDMNet();
            tdmNet->isIntraNet = true == false;
            for(auto pin:net->pins){
                tdmNet->pins.push_back(pin);
            }
            tdmNet->toFPGAId = net->pins[0]->FPGAId;
            tdmNet->fromFPGAId =net->pins[0]->FPGAId;
            TDMNets.push_back(tdmNet);
        }
        else{
            vector<TDMNet *> subNets(count,NULL);
            unordered_map<int,int> FPGAToSubnets;
            int index = 0;
            for(auto FPGAId:*crossingFPGAs){
                //subNet初始化 给所有的Subnet赋序号
                subNets[index]=new TDMNet();
                subNets[index]->parentNetId = net->id;
                FPGAToSubnets[FPGAId] = index;
                index++;
            }
            int driversubNetId = -1;
            int driverFPGAId = -1;
            for(auto pin:net->pins){
                int subNetId = FPGAToSubnets[pin->FPGAId];
                if(pin->type=='o'){
                    driversubNetId = subNetId;
                    driverFPGAId = pin->FPGAId;
                }
                if(pin->type =='o'){
                    subNets[subNetId]->driverId = net->pins.size();
                }
                subNets[subNetId]->pins.push_back(pin);
            }
            Pin* driverPin = subNets[driversubNetId]->pins[subNets[driversubNetId]->driverId];
            for(int j=0;j<crossingFPGAs->size();j++){
                subNets[j]->isIntraNet = (j==driverFPGAId);
                if(j!=driverFPGAId){
                    subNets[j]->pins.push_back(driverPin);
                }
                subNets[j]->fromFPGAId = driverFPGAId;
            }
            for(auto subNet:subNets){
                if(subNet->pins.size()!=1){
                    TDMNets.push_back(subNet);
                }
            }
        }
    }

    vector<vector<Troncon*>> troncons;

    // 横跨FPGA的线网
    vector<XdrVar *> xdrVars;

    int FPGACount = graph->FPGAs.size();
    troncons.resize(FPGACount,vector<Troncon*>(FPGACount,NULL));
    //  初始化troncon矩阵(SLL)   无向的
    for(int i=0;i<FPGACount;i++){
        for(int j=i+1;j<FPGACount;j++){
            //题目有给
            int limit = 20;
            troncons[i][j] = new Troncon(i,j,limit);
        }
    }

    for(auto net:TDMNets){
        if(!net->isIntraNet){
            int fromFPGA = net->fromFPGAId;
            int toFPGA = net->toFPGAId;
            //如果矩阵是对称的
            Troncon* troncon = troncons[fromFPGA][toFPGA];
            troncon->nets.push_back(net);
            //方向,id,对应的TDM线网
            XdrVar * xdrVar = new XdrVar();
            xdrVar->net = net;
            net->xdrvar = xdrVar;
            xdrVar->forward = true;
            xdrVar->id = 1;
            xdrVars.push_back(xdrVar);
        }
    }

    for(int i=0;i<FPGACount;i++){
        for(int j=0;j<FPGACount;j++){
            //如果经过Troncon的线网数量大于0
            if(troncons[i][j]->nets.size()>0){
                //给Troncon赋id
                //map(id,<int,int>)
            }
            else {
                //删除矩阵中的Troncon[i][j]
            }
        }
    }
    //Troncon中不为0的且为TDM边个数
    int tronconCount = 0;
    //TDM比率列举到数组中 这里是n的倍数
    vector<int> xdrChoice = {1,4,8,12,16};
    vector<XdrVar*> optXdrVars;
    //遍历所有的troncon,将其分为需要优化的Tdm边和不需要优化的
    for(int i=0;i<tronconCount;i++){
        Troncon* troncon = new Troncon();
        if(troncon->nets.size()>troncon->limit){
            for(auto net:troncon->nets){
                optXdrVars.push_back(net->xdrvar);
            }
        }
        else {
            for(auto net: troncon->nets){
                net->xdrvar->TDMRatio=1;
            }
        }
    }

    for(auto net:TDMNets){
        for(auto pin:net->pins){
            Edge* edge = new Edge();
            edge->driverDieId = pin->DieId;
            edge->fanoutDieId = pin->toDieId;
            // edge要有TDMNet???先放着
        }
    }
    //检查环路

    // constructTimingGraph


    cout<<"                begin TDM optimization                "<<endl;
    //levelize()
    vector<Node*> nodes = graph->Nodes;
    vector<Edge*> edges = graph->Edges;
    queue<Node*> q;
    // node的driverEdge个数
    vector<int> driverEdgeCount(nodes.size(),0);
    for(auto node:nodes){
        driverEdgeCount[node->id] = node->driverEdgeIds.size();
    }
    //深度遍历
    vector<vector<Node*>> levels;
    vector<Node*>* level = nullptr;
    //随机选择一个点
    //source sink
    q.push(routinggraph->source);
    int curLevelNode=1;
    int preLevelNode = 0;
    while(true){
        if(preLevelNode==0){
            preLevelNode = curLevelNode;
            curLevelNode = 0;
        }
        Node* curNode = q.front();
        q.pop();
        preLevelNode--;
        level->push_back(curNode);
        //sink source
        if(curNode==routinggraph->sink){
            break;
        }
        // fanout driver
        for(auto edgeId:curNode->fanoutEdgeIds){
            Edge* edge = edges[edgeId];
            //fanout driver
            driverEdgeCount[edge->fanoutNodeId]--;
            if(driverEdgeCount[edge->fanoutNodeId] == 0){
                //driver fanout
                q.push(nodes[edge->driverNodeId]);
                curLevelNode++;
            }
        }

    }
    //初始化level和relevel后


    int nIter = 10;
    for(int i =0;i<nIter;i++){
        if(i==0){
            //initMu
            const double IntraWeight = 0.5;
            const double InterWeight = 1;
            vector<vector<Node*>> levels;
            vector<double> EdgeWeights(graph->Edges.size(),0);
            vector<double> NodeWeights(graph->Edges.size(),0);
            for(int i=0;i<levels.size();i++){
                vector<Node*>level = levels[i];
                for(auto node:level){
                    for(auto driverEdgeId:node->driverEdgeIds){
                        Edge * driverEdge = graph->Edges[driverEdgeId];
                        EdgeWeights[driverEdgeId] +=
                                driverEdge->driverDieId==driverEdge->fanoutDieId? IntraWeight:InterWeight;
                        NodeWeights[node->id] += EdgeWeights[driverEdgeId];
                    }
                    for(auto fanoutEdgeId:node->fanoutEdgeIds){
                        EdgeWeights[fanoutEdgeId] =
                                NodeWeights[node->id]/node->fanoutEdgeIds.size();
                    }
                }
            }
            vector<vector<Node*>> revlevels;
            for(int j=0;j<revlevels.size();j++){
                vector<Node*>level = revlevels[j];
                for(auto node:level){
                    double fanoutSum = 0;
                    if(node== routinggraph->sink){
                        fanoutSum =1;
                    }
                    else {
                        for(auto fanoutEdgeId:node->fanoutEdgeIds){
                            fanoutSum += routinggraph->mu[fanoutEdgeId];
                        }
                    }
                    if(NodeWeights[node->id]==0){
                        for(auto driverEdgeId:node->driverEdgeIds){
                            routinggraph->mu[driverEdgeId] =
                                    fanoutSum/node->driverEdgeIds.size();
                        }
                    }
                    else {
                        for(auto driverEdgeId:node->driverEdgeIds){
                            routinggraph->mu[driverEdgeId] =
                                    fanoutSum*EdgeWeights[driverEdgeId]/NodeWeights[node->id];
                        }
                    }
                }
            }

            // init Lambda
            map<Troncon*,vector<int>> TronconToXdrVar;
            vector<XdrVar *> optXdrVars;
            const double tdmConf=5;
            for(auto pair:TronconToXdrVar){
                Troncon *troncon = pair.first;
                double sum = 0;
                double maxSum = 0;
                // 对每一条TDM比率（不知道是相同TDM比率的边还是具体到每一条边）
                for(auto varId:pair.second){
                    double tmpSum = 0;
                    for(auto detailEdge:optXdrVars[varId]->detailEdges){
                        tmpSum = routinggraph->mu[detailEdge->ParentEdgeId]*tdmConf;
                    }
                    sum += sqrt(tmpSum);
                    maxSum = max(maxSum,tmpSum);
                }
                double lamda = routinggraph->lamda[troncon->id];
                lamda = pow(sum/troncon->limit,2);
                lamda = max(lamda, maxSum);
            }
        }
        else {
            double ratio = 0;
            const double changeRate = 0.01;
            const double baseRate = 0.2;
            //update
            //getRatio(iter)
            ratio = baseRate*pow(0.5,changeRate*i);
            //updateMu



            vector<vector<Node *>> revLevels;//同line224
            for(int j=0;j<revLevels.size();j++){
                vector<Node*> level = revLevels[j];
                int vIndex = 0;
//              mutex idMutex;
//              mutex counterMutex;
                const int batchSize = 20;
                while(true){
                    int id = vIndex;
                    vIndex+=batchSize;
                    if(id>level.size()) break;
                    for(int k=id;k<min((int)level.size(),id+batchSize);k++){
                        Node *node =level[k];
                        int driverEdgeSize = node->driverEdgeIds.size();
                        vector<pair<Edge*,double>> gradients;
                        for(int p=0;p<driverEdgeSize;p++){
                            Edge *edge = graph->Edges[node->driverEdgeIds[p]];
                            Node *fanoutNode = graph->Nodes[edge->fanoutNodeId];
                            gradients.emplace_back(edge,
                                                   edge->arrivalTimeAlongEdge-node->arrivalTime);
                        }
                        int numCriMu = 0;
                        // arrivalTime 不知道什么时候赋值的
                        for(auto driverEdgeId:node->driverEdgeIds){
                            Node *fanoutNode2 = graph->Nodes[graph->Edges[driverEdgeId]->fanoutNodeId];
                            Edge *driverEdge = graph->Edges[driverEdgeId];
                            // isCritical
                            numCriMu += fanoutNode2->arrivalTime == node->arrivalTime+driverEdge->delay;
                        }
                        double fanoutSum = 0;
                        if(node == routinggraph->sink){
                            fanoutSum=1;
                        }
                        else {
                            for(auto fanoutEdgeId:node->fanoutEdgeIds){
                                fanoutSum += routinggraph->mu[fanoutEdgeId];
                            }
                        }
                        double driverSum=0;
                        for(auto driverEdgeId:node->driverEdgeIds){
                            driverSum+= routinggraph->mu[driverSum];
                        }
                        if(node==routinggraph->sink){
                            if(numCriMu==driverEdgeSize) continue;
                            //sinkFlow driverSum fanoutSum,gradients

                        }
                        else if(numCriMu==driverEdgeSize){
                            //critFlow node driverSum,fanoutSum
                        }
                        else if(driverSum>fanoutSum){
                            //decreaseFlow driverSum fanoutSum gradients,0,node
                        }
                        else if(driverSum<=fanoutSum){
                            //increaseFlow node driverSum fanoutSum
                        }
                        else {
                        }
                        //removeAccIssue node
                    }
                }
                //propagateFlow
            }
            //updateLambda
            map<Troncon*,vector<int>> TronconToXdrVar;
            for(auto&pair:TronconToXdrVar){
                vector<double> muVec;
                Troncon *troncon = pair.first;
                double sum = 0;
                for(auto var:pair.second){
                    double tmpSum = 0;

//                    for(auto edge){
                    //遍历在Troncon上的Edge
//                        tmpSum+= edge->coef*edge->mu
//                    }
                    muVec.push_back(sqrt(tmpSum));
                    sum+=muVec.back();
                }
                sort(muVec.begin(),muVec.end());

//                if (muVec.back() / muVec.front() > _tdmLagData._maxChoice) {
//
//                    break;
//                }
                double lamda = routinggraph->lamda[troncon->id];
                lamda = pow(sum/troncon->limit,2);
                lamda = max(lamda,muVec.back());

            }
        }
        //solveLRS
        vector<XdrVar*> optXdrVars;
        int cnt1=0,cnt2=0,cnt3=0;
        for(auto var:optXdrVars){
            //var 对应的Troncon
            Troncon *troncon;
            double &lambda = routinggraph->lamda[troncon->id];

            double sum = 0;
            //var 对应的Edge
//            for(auto edge:varEdge)
            {
                Edge* edge;
                sum+=5*routinggraph->mu[edge->id];
            }
            int maxChoice =100;
            if(sum!=0){
                double newVal = sqrt(lambda/sum);
//                if(newVal<1||newVal>maxChoice)
                {
                    int nCrit = 0;
//                    for(auto edge:varEdge)
                    {
                        Edge* edge;
//                        nCrit+=edge->isCritical见line312
                    }
                }
                if(newVal<1) {
                    var->TDMRatio=1;
                    cnt1++;
                }
                else if(newVal>maxChoice){
                    var->TDMRatio = maxChoice;
                    cnt2++;
                }
                else {
                    var->TDMRatio = newVal;
                }
            }
            else {
                var->TDMRatio = maxChoice;
                cnt3++;
            }
        }

        //updateArrivalTime
        //resetArrivalTime
        for(auto node:graph->Nodes){
            node->arrivalTime = -1;
        }
        //source->updateArrivalTime(0)
        routinggraph->source->arrivalTime = max(routinggraph->source->arrivalTime,0.0);
        //forwardPropagateMT();
        for(int l = 0;l<levels.size();l++){
            //加进程锁
            int vIdx = 0;
            const int batchSize=20;
            while(true){
                int idx;

                idx = vIdx;
                vIdx+=batchSize;

                if(idx>=levels.size()) break;
                for(int j=idx;j<min((int)levels[l].size(),idx+batchSize);j++){
                    Node* node = levels[l][j];
                    for(auto driverEdgeId:node->driverEdgeIds){
                        Edge* edge = graph->Edges[driverEdgeId];
//                        node->updateArrivalTime(driver->getArrivalTimeAlongEdge());
                        node->arrivalTime = max(node->arrivalTime,edge->arrivalTimeAlongEdge);
                    }
                    for(auto fanoutEdgeId:node->fanoutEdgeIds){
                        Edge* edge = graph->Edges[fanoutEdgeId];
                        //edge->updateDelay()
                        //如果edge没有net，则delay=0
                        //ConstDelay 下次再找
                        int constDelay = 1;
                        edge->delay = constDelay;
                        // 如果该线网有用TDM技术  _delay += _tdmCoef * _net->getXdrVar()->getVal();
                        Node *driverNode = graph->Nodes[edge->driverNodeId];
                        edge->arrivalTimeAlongEdge = edge->delay+driverNode->arrivalTime;
                    }
                }
            }
        }
        //
        double dualVal = 0;
        bool computeDual = false;
        if(!computeDual) dualVal = 0;
        else {
            double result = 0;
            result+= routinggraph->sink->arrivalTime;
            map<Troncon*,vector<int>> TronconToXdrVar;
            for(auto &pair:TronconToXdrVar){
                Troncon *troncon = pair.first;
                double usage = 0;
                for(auto net:troncon->nets){
                    double val = net->xdrvar->TDMRatio;
                    usage += 1/val;
                }
                result+=routinggraph->lamda[troncon->id]*(usage-troncon->limit);
            }
            for(auto& edge:graph->Edges){
                double mu = routinggraph->mu[edge->id];
                Node* fanoutNode = graph->Nodes[edge->fanoutNodeId];
                result += mu*(edge->arrivalTimeAlongEdge-fanoutNode->arrivalTime);

            }
            dualVal = result;
        }
        double primVal = routinggraph->sink->arrivalTime;
        const int interval = 70;
        int bestIter = -1;
        vector<int> bestVals(100,0);
        double bestCost = 10000000;//DBL_MAX
        if(i-interval>0 && bestVals[i-interval]-bestCost<1 ){
            break;
        }
        if(routinggraph->sink->arrivalTime<bestCost){
            bestCost = i;
            printf("bestSol");
            bestCost = routinggraph->sink->arrivalTime;
        }
        bestVals[i] = bestCost;
    }
    //double dual = computeDual(true);
    double result = 0;
    result+= routinggraph->sink->arrivalTime;
    map<Troncon*,vector<int>> TronconToXdrVar;
    for(auto &pair:TronconToXdrVar){
        Troncon *troncon = pair.first;
        double usage = 0;
        for(auto net:troncon->nets){
            double val = net->xdrvar->TDMRatio;
            usage += 1/val;
        }
        result+=routinggraph->lamda[troncon->id]*(usage-troncon->limit);
    }
    for(auto& edge:graph->Edges){
        double mu = routinggraph->mu[edge->id];
        Node* fanoutNode = graph->Nodes[edge->fanoutNodeId];
        result += mu*(edge->arrivalTimeAlongEdge-fanoutNode->arrivalTime);
    }
    double dual = result;
    // updateArrivalTime
    double primal = routinggraph->sink->arrivalTime;
//    dual = max(dual,computeDual(true))

//updateArrivalTime
    //resetArrivalTime
    for(auto node:graph->Nodes){
        node->arrivalTime = -1;
    }
    //source->updateArrivalTime(0)
    routinggraph->source->arrivalTime = max(routinggraph->source->arrivalTime,0.0);
    //forwardPropagateMT();
    for(int l = 0;l<levels.size();l++){
        //加进程锁
        int vIdx = 0;
        const int batchSize=20;
        while(true){
            int idx;

            idx = vIdx;
            vIdx+=batchSize;

            if(idx>=levels.size()) break;
            for(int j=idx;j<min((int)levels[l].size(),idx+batchSize);j++){
                Node* node = levels[l][j];
                for(auto driverEdgeId:node->driverEdgeIds){
                    Edge* edge = graph->Edges[driverEdgeId];
//                        node->updateArrivalTime(driver->getArrivalTimeAlongEdge());
                    node->arrivalTime = max(node->arrivalTime,edge->arrivalTimeAlongEdge);
                }
                for(auto fanoutEdgeId:node->fanoutEdgeIds){
                    Edge* edge = graph->Edges[fanoutEdgeId];
                    //edge->updateDelay()
                    //如果edge没有net，则delay=0
                    //ConstDelay 下次再找
                    int constDelay = 1;
                    edge->delay = constDelay;
                    // 如果该线网有用TDM技术  _delay += _tdmCoef * _net->getXdrVar()->getVal();
                    Node *driverNode = graph->Nodes[edge->driverNodeId];
                    edge->arrivalTimeAlongEdge = edge->delay+driverNode->arrivalTime;
                }
            }
        }
    }
    // "---------------- finish TDM analytical solving ----------------"
    // "==================== begin TDM refinement(greedy) ===================="
    //    updateTiming();
    //updateArrivalTime()
    //resetArrivalTime();
    //_source->updateArrivalTime(0);
    //forwardPropagateMT();
    //updateRequireTime()
    //resetRequireTime();
    //_sink->updateRequireTime(getSinkAT());
    //backwardPropagateMT();
    //getCriticalPath
    vector<Edge*> criticalPath ;
    int iter = 0;
    double bestCost = 100000;//DBL_MAX
//    while(optimizePath(criticalPath)){
    unordered_set<XdrVar*> vars;
    for(auto edge:criticalPath){
        if(edge->net->isIntraNet){

        }
    }
    if(iter%10 ==0){
        if(bestCost <=routinggraph->sink->arrivalTime){
            break;
        }
        else {
            bestCost = routinggraph->sink->arrivalTime;
        }
    }
    //getCriticalPath
    criticalPath;
    iter++;
//    }
    return 0;
}
