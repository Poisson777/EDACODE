#ifndef _NET_TPP_
#define _NET_TPP_

#include <vector>
#include <map>
#include<unordered_map>
#include <algorithm>
#include "Node.tpp"
#include "Edge.tpp"
#include<iostream>

using namespace std;

class Edge;

class Net {
private:
public:
    int netId;
    std::vector<Edge*> edges;
    std::vector<int> edges_crossing;// 存放
    Node *sourceNode;
    std::vector<Node*> sinkNodes;
    std::map<int,double> weight;// sinkDie - weight
    std::map<int,std::vector<int>> path_node;// sinkDie sourceDie
    std::map<int,std::vector<int>> path_edge;// sinkDie sourceDie
    std::map<int,std::vector<int>> crossing;//sinkDie crossing
    std::map<std::pair<int, int>, int> att_edges;
    std:: vector<int> flag;
    std::map<Edge*,int> edge_sinkNode;

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
            for(auto it = path_node[netInfo.first].rbegin(); it != path_node[netInfo.first].rend() ; ++it){
                std::cout<<*it<<" ";
            }
            std::cout<<"\n";
        }
    }
    void addAtt_edges(int a,int b,int index){
        att_edges[{a,b}] = index;
//        std::cout << "添加("<<a<<b<<index<<"\n";
    }
    int getAtt_edge(int a,int b){
        return att_edges[{a,b}];
    }
    void assignflag(int count){
        for (int i = 0; i < count; ++i) {
            flag.push_back(-1);
        }
    }
    void abc(vector<Die*> dies){
        for (int k = 0; k < sinkNodes.size(); ++k) {
            int sinkDieId = sinkNodes[k]->die->DieId;
            if (path_node.find(sinkDieId) != path_node.end()) continue;
            path_node[sinkDieId].push_back(sinkDieId);

            while (path_node[sinkDieId].back() != sourceNode->die->DieId) {
                int next = flag[path_node[sinkDieId].back()];
                if (dies[next]->fpga->FPGAId != dies[path_node[sinkDieId].back()]->fpga->FPGAId) {
                    crossing[sinkDieId].push_back(
                            getAtt_edge(next,path_node[sinkDieId].back())
                    );
//                    cout << "(" << path_node[sinkDieId].back() << "," << next << ")" << endl;
//                    cout << getAtt_edge( next,path_node[sinkDieId].back()) << "sjdks" << endl;
                }
                path_edge[sinkDieId].push_back(
                        getAtt_edge(next,path_node[sinkDieId].back())
                );
                path_node[sinkDieId].push_back(next);
            }
        }

    }
};

#endif
