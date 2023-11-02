#include "src/global.h"
#include "src/FileManager.cpp"
#include "src/global.h"
#include "src/TDMData.cpp"
int main(){
    FileManager fileManager;
    fileManager.read("2");
    Graph* graph = new Graph();
    RoutingGraph* routingGraph = new RoutingGraph();
    graph->constructGraph(fileManager.die_position,fileManager.fpga_die);
    graph->toString();
    routingGraph->initRoutingGraph(graph,fileManager.source_sink,fileManager.wires);
    routingGraph->net_toString();
    return 0;
}