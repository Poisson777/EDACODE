#ifndef _NET_TPP_
#define _NET_TPP_

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include "Edge.tpp"
#include "Node.tpp"


class Edge;

class Net {
private:
public:
    int netId;
    std::vector<Edge*> edges;
    Node *sourceNode;
    std::vector<Node*> sinkNodes;
    std::map<int,double> weight;// sinkDie - weight
    std::map<int,std::vector<int>> path;// sinkDie sourceDie
    double max_weight = 0;
    Net() : edges(), sourceNode(), sinkNodes() {}
    void output(){
        std::vector<std::pair<int, double>> netInfoArray(weight.size());
        int i=0;
        for(auto w:weight) {
            netInfoArray[i] = std::make_pair(w.first,w.second);
            i++;
        }

        // 使用 std::sort 对 pair 数组进行排序，按照 maxWeight 从大到小的顺序
        std::sort(netInfoArray.begin(), netInfoArray.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });
//
//     输出排序后path
        std::cout<<"SourceDie"<<sourceNode->die->DieId<<"\n";
        std::cout << "Net里的路径" << std::endl;
        for (const auto& netInfo : netInfoArray) {
            std::cout << "sinkDie: " << netInfo.first << " maxWeight: " << netInfo.second << std::endl;
            for(auto it = path[netInfo.first].rbegin(); it!=path[netInfo.first].rend() ; ++it){
                std::cout<<*it<<" ";
            }
            std::cout<<"\n";
        }
    }
};

#endif
