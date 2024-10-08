#include <stdio.h>
#include "hash.h"
#include "Graph.h"


int main(){
    Graph graph = graphCreate();
    Map map = mapCreate();
    graphAddNode(graph, "1", map);
    graphAddNode(graph, "2", map);
    graphAddNode(graph, "3", map);
    graphAddNode(graph, "4", map);
    graphAddNode(graph, "5", map);

    addVertex(graph, "1/1/2020", 100, "1", "2", map);
    addVertex(graph, "2/1/2020", 200, "1", "3", map);
    //εαν δεν υπάρχει ο κόμβος τότε θα δημιουργηθεί
    addVertex(graph, "3/1/2020", 300, "3", "19", map);

    displayGraph(graph, map);

    printf("---------------------------------------------------------\n\n");
    //does not exists
    removeGraphNode("7", map, graph);
    //remove
    removeGraphNode("1", map, graph);

    displayGraph(graph, map);

    destroyGraph(graph, map);
    return 0;
}