// temporary test .cpp file
#include "layout/head.h"
#include "layout/GetNetworkReturn.h"
#include "src/TDMNet.cpp"
#include "src/Troncon.cpp"
using namespace std;
extern int sqrt(int);

void printnet(Layout::GetNetworkReturn res){
    //    for(auto net:res.routing_graph.nets){
//        std::cout<<"Net"<<net->netId<<"\n";
//        std::cout<<net->sourceNode->die->DieId<<"\n";
//        for(auto sinkNode:net->sinkNodes){
//            std::cout<<sinkNode->die->DieId<<"\n";
//        }
//        for(auto edge:net->edges){
//            std::cout<<"FROM DIE"<<edge->fromDie->DieId<<" TO DIE"<<edge->toDie->DieId<<"\n";
//        }
//    }
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
    map<int,map<int,vector<int>>> net_tdmnet;
    map<int,Net*> netId_net;
    for(auto net:res.routing_graph.nets){
        vector<int> flag(res.routing_graph.dies.size(),-1);
        map<int,vector<int>> m;
        netId_net.insert({net->netId,net});
        for(auto edge:net->edges) {
            int from_die_id = edge->fromDie->DieId;
            int to_die_id = edge->toDie->DieId;
            flag[to_die_id] = from_die_id;
        }
        for(auto sinkNode:net->sinkNodes){
            if(m.find(sinkNode->die->DieId)!=m.end()) continue;
            m[sinkNode->die->DieId].push_back(sinkNode->die->DieId);
            while(m[sinkNode->die->DieId].back()!=net->sourceNode->die->DieId){
                int next = flag[m[sinkNode->die->DieId].back()];
                m[sinkNode->die->DieId].push_back(next);
            }
        }
        net_tdmnet[net->netId] = m;
    }
    /*
//    for(auto it1:net_tdmnet){
//        cout<<"Net_Id"<<it1.first<<endl;
//        if(it1.second.size()>1) cout<<"n你好"<<endl;
//        for(auto it2:it1.second){
//            cout<<"sinkDie"<<it2.first<<endl;
//            for(auto a:it2.second){
//                cout<<" "<<a;
//            }
//            cout<<endl;
//        }
//    }
*/
    map<pair<int,int>,vector<Edge*>> troncon_edges;
    map<pair<int,int>,vector<vector<int>>> xdrvars;

    map<pair<int,int>,map<int,int>> troncon_edge;
    int die_count = res.routing_graph.dies.size();
    vector<int> troncon_array(die_count*die_count,0);

    for(auto net:res.routing_graph.nets){
        for(auto edge:net->edges){
            troncon_array[edge->fromDie->DieId*die_count+edge->toDie->DieId] +=1;
            troncon_edge[make_pair(edge->fromDie->DieId,edge->toDie->DieId)][net->netId]=troncon_edges[make_pair(edge->fromDie->DieId,edge->toDie->DieId)].size();
            troncon_edges[make_pair(edge->fromDie->DieId,edge->toDie->DieId)].push_back(edge);

        }
    }
    for(int i=0;i<die_count;i++) {
        for(int j=i;j<die_count;j++){
            troncon_array[i*die_count+j]=troncon_array[i*die_count+j]+troncon_array[j*die_count+i];
        }
    }
//    for(int i=0;i<die_count*die_count;i++){
//        int row = i/die_count;
//        int column = i%die_count;
//        if(i%die_count==0) cout<<endl;
//        cout<<troncon_array[i]<<"/"<<troncon_edges[make_pair(row,column)].size()<<" ";
//    }

    for(int i=0;i<die_count*die_count;i++){
        int row = i/die_count;
        int column = i%die_count;
        if(troncon_array[row*die_count+column]>res.adj[i]
           &&(res.dies[row].fpga->FPGAId)!=res.dies[column].fpga->FPGAId){
            double all_size = troncon_array[row*die_count+column];
            double limit = res.adj[i];
            for(auto edge:troncon_edges[make_pair(row,column)]){
                edge->TDMRatio = std::ceil(static_cast<double>(all_size/limit)/4) * 4;
                edge->InterFPGA = true;

            }
        }
        else if((res.dies[row].fpga->FPGAId)!=res.dies[column].fpga->FPGAId){
            //不用优化，最小为4
            for(auto edge:troncon_edges[make_pair(row,column)]){
                edge->TDMRatio = 4;
                edge->InterFPGA = true;
            }
        }
    }

    double max_delay = 0.0;
    for(auto net:res.routing_graph.nets){
        double weight_max = 0;
        net->path = net_tdmnet[net->netId];
        for(auto sink_tdmNet:net_tdmnet[net->netId]){
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
//        cout<<"NetId"<<net->netId<<"weight"<<net->max_weight<<endl;
        max_delay = max_delay>weight_max?max_delay:weight_max;
    }


    cout<<"\nmax_delay\n"<<max_delay<<"\n";
    std::vector<std::pair<int, double>> netInfoArray(res.routing_graph.nets.size());
    for (int i = 0; i < res.routing_graph.nets.size(); ++i) {
        netInfoArray[i] = std::make_pair(res.routing_graph.nets[i]->netId, res.routing_graph.nets[i]->max_weight);
    }

    // 使用 std::sort 对 pair 数组进行排序，按照 maxWeight 从大到小的顺序
    std::sort(netInfoArray.begin(), netInfoArray.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

//     输出排序后的 netId 序号
    std::cout << "根据 maxWeight 从大到小的排序：" << std::endl;
    int print_count = 0;
    for (const auto& netInfo : netInfoArray) {
        std::cout << "netId: " << netInfo.first << " maxWeight: " << netInfo.second << std::endl;
        netId_net[netInfo.first]->output();

    }
    for(int i=0;i<die_count;i++){
        for(int j=i;j<die_count;j++){
            cout<<"[Die"<<i<<",Die"<<j<<"]"<<endl;
            if(res.routing_graph.dies[i]->fpga->FPGAId!=res.routing_graph.dies[j]->fpga->FPGAId)
                for(auto el:troncon_edges[make_pair(i,j)]){
                    cout<<el->net->netId<<" "<<endl;
                }
        }
    }
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

    auto res = Layout::get_network("2");
//    printnet(res);
    tdm(res);
    std::cout <<"\n" <<"Layout::get_network() test passed.\n";
    clock_t end = clock();
    double cpu_time_used = static_cast<double>(end - start) / CLOCKS_PER_SEC;
    // 输出结果
    std::cout << "CPU time taken by function: " << cpu_time_used << " seconds" << std::endl;
}
