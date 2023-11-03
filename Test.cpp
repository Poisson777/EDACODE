#include "src/global.h"
#include "src/FileManager.cpp"
#include "src/global.h"
#include "src/TDMData.cpp"
const int INF = numeric_limits<int>::max();
int main(){
    FileManager fileManager;
    fileManager.read("2");
    Graph* graph = new Graph();
    RoutingGraph* routingGraph = new RoutingGraph();
    graph->constructGraph(fileManager.die_position,fileManager.fpga_die);
    routingGraph->initRoutingGraph(graph,fileManager.source_sink,fileManager.wires);
    routingGraph->net_toString();

    vector<Net*> nets = routingGraph->nets;
    vector<vector<Troncon*>> trons = routingGraph->troncon_array;
    vector<vector<int>> dist;
    map<int,Die*> dies = graph->Dies;
    int numVertics = dies.size();
    map<int,Node*> nodes = graph->Nodes;
    vector<vector<int>> array(numVertics,vector<int>(numVertics,0));
    for(auto trons:trons){
        for(auto t:trons){
            int a = t->TwoDie.first->DieId;
            int b = t->TwoDie.second->DieId;
            array[a][b] = array[b][a] = 1;
        }
    }

    for(auto net:nets){
        Die* source_die = net->sourceNode->die;
        vector<Die*> sinkDies;
        for(auto sinknode:net->sinkNodes){
           Die* sink_die = sinknode->die;
           sinkDies.push_back(sink_die);
           vector<int> distance(numVertics,INF);
           vector<bool> visited(numVertics,false);
           vector<int> previous(numVertics,-1);
           distance[source_die->DieId] =0;
           for(int i=0;i<numVertics;i++){
               int minDistance = INF;
               int minIndex = -1;
               for(int j=0;j<numVertics;j++){
                   if(!visited[j]&& distance[j]<minDistance){
                       minDistance = distance[j];
                       minIndex = j ;
                   }
               }
               visited[minIndex] = true;
               for(int j=0;j<numVertics;j++){
                   if(!visited[j]&&array[minIndex][j]!=0
                   &&distance[minIndex]!=INF
                   &&distance[minIndex]+array[minIndex][j]<distance[j]){
                        distance[j] = distance[minIndex]+array[minIndex][j];
                        previous [j] = minIndex;
                   }
               }
           }
           if(distance[sinknode->die->DieId]==INF) cout<<"无法从起点到终点"<<endl;
           else {
               cout<<"最短路径长度为"<<distance[sinknode->die->DieId];
               stack<int> path;
               int current = sinknode->die->DieId;
               while(current!=-1){
                   path.push(current);
                   current = previous[current];
               }
               cout<<"Net"<<net->NetId<<":"<<source_die->DieId<<"->"<<sink_die->DieId<<"最短路径为";
               int tempId1 = -1;
               int tempId2 = -1;
               while(!path.empty()){
                   tempId1 = tempId2;
                   tempId2 = path.top();
                   cout<<path.top();
                   path.pop();
                   if(!path.empty()){
                       cout<<"->";
                   }
                   if(tempId2!=-1&&tempId1!=-1){
                       bool contains = false;
                       for(auto edge:net->edges){
                           if(edge->fromDie->DieId==tempId1&&edge->toDie->DieId==tempId2){
                               contains= true;
                               cout<<"重复Net"<<net->NetId<<"Die"<<tempId1<<"Die"<<tempId2<<endl;
                               break;
                           }
                       }
                       Edge* edge = new Edge();
                       edge->edgeId = routingGraph->edges.size();
                       edge->fromDie = graph->Dies[tempId1];
                       edge->toDie = graph->Dies[tempId2];
                       if(!contains){
                           routingGraph->edges.push_back(edge);
                           net->edges.push_back(edge);
                       }
                       net->paths[sinknode].push_back(edge);
                   }
               }
               cout<<endl;
           }
        }
    }
    for(auto net:nets){
        cout<<"Net"<<net->NetId<<endl;
        cout<<"Die"<<net->sourceNode->die->DieId<<endl;
        for(auto sinkNode:net->sinkNodes){
            cout<<"Die"<<sinkNode->die->DieId<<" "<<endl;
            for(auto edge:net->paths[sinkNode]){
                cout<<"Edge"<<edge->edgeId<<" Die"<<edge->fromDie->DieId<<" Die"<<edge->toDie->DieId<<endl;
            }
        }
    }


    return 0;
}