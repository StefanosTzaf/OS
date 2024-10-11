#include <stdio.h>
#include "hash.h"
#include "Graph.h"


int main(){
    Graph graph = graphCreate();
    Map map = mapCreate(compareMapNodes, destroyMapNodes, 100);
//    graphAddNode(graph, "1", map);
//     graphAddNode(graph, "2", map);
//    graphAddNode(graph, "3", map);
//    graphAddNode(graph, "4", map);
//    graphAddNode(graph, "5", map);

    addVertex(graph, "1/1/2020", 100, "1", "2", map);
   // addVertex(graph, "2/1/2020", 200, "1", "3", map);
    //εαν δεν υπάρχει ο κόμβος τότε θα δημιουργηθεί
    //addVertex(graph, "3/1/2020", 300, "3", "19", map);
 //   addVertex(graph, "4/1/2020", 400, "4", "1", map);
 //   addVertex(graph, "5/1/2020", 5001, "4", "1", map);

//    displayGraph(graph, map);

    printf("---------------------------------------------------------\n\n");
    //does not exists
   // addVertex(graph, "66/1/2020", 100, "19", "2", map);
  //  removeGraphNode("7", map, graph);
    //remove
  //  removeGraphNode("1", map, graph);
  //  removeGraphNode("3", map, graph);
 //   removeGraphNode("5", map, graph);
 //  removeGraphNode("4", map, graph);


  // displayGraph(graph, map);

    destroyGraph(graph);
    map_destroy(map);
    return 0;
}