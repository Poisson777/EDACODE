// temporary test .cpp file
#include "layout/head.h"
#include "layout/GetNetworkReturn.h"
#include "src/TDMNet.cpp"
#include "src/Troncon.cpp"
using namespace std;
extern int sqrt(int);

void printnet(Layout::GetNetworkReturn res){
        for(auto net:res.routing_graph.nets){
        std::cout<<"Net"<<net->netId<<"\n";
        std::cout<<net->sourceNode->die->DieId<<"\n";
        for(auto sinkNode:net->sinkNodes){
            std::cout<<sinkNode->die->DieId<<"\n";
        }
        for(auto edge:net->edges){
            std::cout<<"FROM DIE"<<edge->fromDie->DieId<<" TO DIE"<<edge->toDie->DieId<<"\n";
        }
    }
    int die_count = res.routing_graph.dies.size();
    cout<<die_count<<endl;
    vector<int> troncon_array(die_count*die_count,0);
    for(auto net:res.routing_graph.nets){
        for(auto edge:net->edges){
            troncon_array[edge->fromDie->DieId*die_count+edge->toDie->DieId] +=1;
        }
    }
    bool flag = true;
    for(int i=0;i<die_count;i++) {
        for(int j=i;j<die_count;j++){
//            troncon_array[i*die_count+j]=troncon_array[i*die_count+j]+troncon_array[j*die_count+i];
        }
    }
    for(int i=0;i<die_count*die_count;i++){
        int row = i/die_count;
        int column = i%die_count;
        if(i%die_count==0) cout<<endl;
        cout<<troncon_array[i]<<"+"<<troncon_array[column*die_count+row]<<"/"<<res.adj[i]<<" ";
        if(troncon_array[row*die_count+column]>res.adj[i]
        &&(res.dies[row].fpga->FPGAId)==res.dies[column].fpga->FPGAId){
            cout<<"不行不行"<<endl;
            flag = false;
        }
    }
    if(flag) cout<<endl<<"对了"<<endl;
}
void printTDM(Layout::GetNetworkReturn res){
    for(auto net:res.routing_graph.nets){
        cout<<"这里是net"<<net->netId<<endl;
        int index = 0;
        for(auto a:net->edges)
            cout<<index++<<" "<<a->fromDie->DieId<<" "<<a->toDie->DieId<<endl;
        for(auto sinkNode:net->sinkNodes){
            cout<<"sinkNode"<<sinkNode->die->DieId<<endl;
            cout<<"路径节点为"<<endl;
            for(auto node:net->path_node[sinkNode->die->DieId]){
                cout<< node << " ";
            }
            cout<<endl<<"pathEdge"<<endl;
            for(auto edgeId:net->path_edge[sinkNode->die->DieId])
                cout<<edgeId<<" ";
            cout<<endl<<"crossing"<<endl;
            for(auto edgeId:net->crossing[sinkNode->die->DieId])
                cout<<edgeId<<" ";
            cout<<endl;
        }
    }
}
void tdm(Layout::GetNetworkReturn res){
/*
//    for(auto net:res.routing_graph.nets){
//        cout<<"Net"<<net->netId<<endl;
//        cout<<"Source"<<net->sourceNode->die->DieId<<endl;
//        if(net->sinkNodes.size()>=2) cout<<"nihao"<<endl;
//        cout<<"Sink: "<<endl;
//        for(auto sink:net->sinkNodes) cout<<sink->die->DieId<<" ";
//        cout<<endl;
//        for(auto edge:net->edges){
//            cout<<edge->fromDie->DieId<<" To "<<edge->toDie->DieId<<endl;
//        }
//    }
 */
    map<int,Net*> netId_net;

    for (int i = 0; i < res.routing_graph.nets.size(); ++i) {
        netId_net.insert({res.routing_graph.nets[i]->netId, res.routing_graph.nets[i]});
        res.routing_graph.nets[i]->assignflag(res.routing_graph.dies.size());
        for (int j = 0; j < res.routing_graph.nets[i]->edges.size(); j++) {
            res.routing_graph.nets[i]->flag[res.routing_graph.nets[i]->edges[j]->toDie->DieId] = res.routing_graph.nets[i]->edges[j]->fromDie->DieId;
            res.routing_graph.nets[i]->addAtt_edges(res.routing_graph.nets[i]->edges[j]->fromDie->DieId, res.routing_graph.nets[i]->edges[j]->toDie->DieId, j);
        }
        res.routing_graph.nets[i]->abc(res.routing_graph.dies);
    }


//    printTDM(res);


    map<pair<int,int>,vector<Edge*>> troncon_edges;// 存储当前troncon里的所有Edge对象
    map<pair<int,int>,map<int,int>> troncon_edge;// 在该troncon中net对应的在Edge数组中的索引
    map<int,vector<vector<int>>> xdrvar_edges;// 存储netId
    map<int,vector<int>> max_value;
    map<int,vector<int>> cap;

    int die_count = res.routing_graph.dies.size();

    for(auto net:res.routing_graph.nets){
        for(auto edge:net->edges){
            troncon_edge[make_pair(edge->fromDie->DieId,edge->toDie->DieId)][net->netId]=troncon_edges[make_pair(edge->fromDie->DieId,edge->toDie->DieId)].size();
            troncon_edges[make_pair(edge->fromDie->DieId,edge->toDie->DieId)].push_back(edge);
        }
    }


    for(int i=0;i<die_count*die_count;i++){
        int row = i/die_count;
        int column = i%die_count;
        if((res.dies[row].fpga->FPGAId)!=res.dies[column].fpga->FPGAId
            &&res.adj[i]!=0&&troncon_edges[{row,column}].size()+troncon_edges[{column,row}].size()!=0){
            cout<<"nihao"<<endl;
            cout<<row<<" "<<column<<" "<<troncon_edges[{row,column}].size()<<endl;
            cout<<column<<" "<<row<<" "<<troncon_edges[{column,row}].size()<<endl;
            cout<<res.adj[i]<<endl;
            vector<vector<int>> result;
            double a = static_cast<double>(troncon_edges[{row,column}].size())/(troncon_edges[{row,column}].size()+troncon_edges[{column,row}].size());
            int allo_wire_count = round(a*res.adj[i]);
            max_value[row*die_count+column].resize(allo_wire_count);
            cap[row*die_count+column].resize(allo_wire_count);
            // 假设是排好序的数组
            xdrvar_edges[row*die_count+column].resize(allo_wire_count);
            if(allo_wire_count*4>=troncon_edges[{row,column}].size()){
                cout<<"随便分"<<endl;
                int edgeIndex = 0;
                for(auto edge:troncon_edges[{row,column}]){
                    edge->TDMRatio = 4;
                    xdrvar_edges[row*die_count+column][edgeIndex/4].push_back(edge->net
                    ->netId);
//                    max_value[row*die_count+column][edgeIndex/4]
                    cap[row*die_count+column][edgeIndex/4] = 4;
                    edgeIndex++;
                }
            }
            else{
                cout<<"认真分"<<endl;
                int edgeIndex = 0;
                int addIndex = 0;
                vector<int> temp_array;
                for(auto edge:troncon_edges[{row,column}]){
                    if(allo_wire_count*4>edgeIndex) {
                        xdrvar_edges[row * die_count + column][edgeIndex / 4].push_back(edge->net->netId);
                        edgeIndex++;
                    }
                    else {
//                        double weight = edge->net->path_edge.size()-crossing.size()
                        temp_array.push_back(edge->net->netId);
                        addIndex++;
                        if(addIndex%4==0||addIndex==troncon_edges[{row,column}].size()-allo_wire_count*4){
                            for(auto temp:temp_array){
                                xdrvar_edges[row*die_count+column][0].push_back(temp);
                            }
                            temp_array.clear();
                            cap[row*die_count+column][0]+=4;

//                            //sort
                            for(size_t k=0;k<cap[row*die_count+column].size()-1;k++){
                                if(max_value[row*die_count+column][k]+cap[row*die_count+column][k]>
                                    max_value[row*die_count+column][k+1]+cap[row*die_count+column][k+1]){
                                    swap(max_value[row*die_count+column][k],max_value[row*die_count+column][k+1]);
                                    swap(cap[row*die_count+column][k],cap[row*die_count+column][k+1]);
                                }
                                else break;
                            }
                        }
                    }
                }

            }
            for(size_t l=0;l<xdrvar_edges[{row*die_count+column}].size();l++){
                for(auto netId:xdrvar_edges[{row*die_count+column}][l]){
                    int edge_index = troncon_edge[{row,column}][netId];
                    troncon_edges[{row,column}][edge_index]->TDMRatio = cap[row*die_count+column][l];
                    troncon_edges[{row,column}][edge_index]->InterFPGA = true;
                }
            }
        }
        /*
//        if(troncon_array[row*die_count+column]>res.adj[i]
//           &&(res.dies[row].fpga->FPGAId)!=res.dies[column].fpga->FPGAId){
//            double all_size = troncon_array[row*die_count+column];
//            double limit = res.adj[i];
//            for(auto edge:troncon_edges[make_pair(row,column)]){
//                edge->TDMRatio = std::ceil(static_cast<double>(all_size/limit)/4) * 4;
//                edge->InterFPGA = true;
//            }
//        }
//        else if((res.dies[row].fpga->FPGAId)!=res.dies[column].fpga->FPGAId){
//
//            for(auto edge:troncon_edges[make_pair(row,column)]){
//                edge->TDMRatio = 4;
//                edge->InterFPGA = true;
//            }
        }*/

    }

//    for(int row = 0;row<die_count;row++){
//        for(int column = row;column<die_count;column++){
//            if(res.dies[row].fpga->FPGAId!=res.dies[column].fpga->FPGAId&&res.adj[row*die_count+column]!=0){
//                double limit = res.adj[row*die_count+column];
//                cout<<"------------"<<limit<<endl;
//                cout<<"("<<row<<","<<column<<")"<<troncon_edges[make_pair(row,column)].size()<<endl;
//                cout<<"("<<column<<","<<row<<")"<<troncon_edges[make_pair(column,row)].size()<<endl;
//            }
//
//        }
//    }

    double max_delay = 0.0;
    for(auto net:res.routing_graph.nets){
        double weight_max = 0;
//        net->path_node = net_tdmnet_node[net->netId];
        for(auto sink_tdmNet:net->path_node){
            double weight_sum = 0;
            for(int i=0;i<sink_tdmNet.second.size()-1;i++){
                int to = sink_tdmNet.second[i];
                int from = sink_tdmNet.second[i+1];
                auto m = troncon_edge[make_pair(from,to)][net->netId];
                if(!troncon_edges[make_pair(from,to)][m]->InterFPGA) weight_sum +=1;
                else weight_sum += (troncon_edges[make_pair(from,to)][m]->TDMRatio*2.0+1.0)*0.5;
            }
            net->weight.insert(make_pair(sink_tdmNet.first,weight_sum));
            weight_max = weight_sum>weight_max?weight_sum:weight_max;
        }
        net->max_weight = weight_max;
        cout<<"NetId"<<net->netId<<"weight"<<net->max_weight<<endl;
        max_delay = max_delay>weight_max?max_delay:weight_max;
    }


    cout<<"\nmax_delay\n"<<max_delay<<"\n";
    std::vector<std::pair<int, double>> netInfoArray(res.routing_graph.nets.size());
    for (int i = 0; i < res.routing_graph.nets.size(); ++i) {
        netInfoArray[i] = std::make_pair(res.routing_graph.nets[i]->netId, res.routing_graph.nets[i]->max_weight);
    }
    std::sort(netInfoArray.begin(), netInfoArray.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

//     输出排序后的 netId 序号
    std::cout << "根据 maxWeight 从大到小的排序：" << std::endl;
    for (const auto& netInfo : netInfoArray) {
        std::cout << "netId: " << netInfo.first << " maxWeight: " << netInfo.second << std::endl;
        netId_net[netInfo.first]->output();
    }
//    for(int i=0;i<die_count;i++){
//        for(int j=i;j<die_count;j++){
//            cout<<"[Die"<<i<<",Die"<<j<<"]"<<endl;
//            if(res.routing_graph.dies[i]->fpga->FPGAId!=res.routing_graph.dies[j]->fpga->FPGAId)
//                for(auto el:troncon_edges[make_pair(i,j)]){
//                    cout<<el->net->netId<<" "<<endl;
//                }
//        }
//    }
    cout<<endl<<max_delay<<endl;

}
int main() {
    assert(sqrt(16384) == 128);
    std::cout << "sqrt() test passed.\n";

    Trie<int> trie;

    trie.add("abc123", 1);
    std::string s1 = "abc123";
    assert(trie.query(s1) == 1);
    s1.pop_back();
    assert(trie.query(s1) == 0);
    std::cout << "Trie class test passed.\n";

    clock_t start = clock();

    auto res = Layout::get_network("10");
//    printnet(res);
    tdm(res);
    std::cout <<"\n" <<"Layout::get_network() test passed.\n";
    clock_t end = clock();
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    // 输出结果
    std::cout << "CPU time taken by function: " << cpu_time_used << " seconds" << std::endl;
}
