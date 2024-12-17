
#include "graph.h"

#include <stack>

/*
 * Degree one lower bound
 * Argument graph: a graph
 * Returns: number of nodes that must be taken into the vertex cover.
 *
 * */
unsigned long deg_one(Graph* graph, std::stack<bool*>* res_stack) {
    //if (graph->node_degree_map.find(1) == graph->node_degree_map.end()) return 0;
    unsigned long lb = 0;

    while (graph->node_degree_map.find(1) != graph->node_degree_map.end()) {
        bool* vertex = *(graph->node_degree_map[1].begin());
        res_stack->push(*(get_node_neighbours(vertex, graph).begin()));
        delete_node_erase(*(get_node_neighbours(vertex, graph).begin()), graph);
        delete_node_erase(vertex, graph);
        lb++;
    }
    return lb;
}


unsigned long deg_zero(Graph* graph) {
    //if (graph->node_degree_map.find(1) == graph->node_degree_map.end()) return 0;
    unsigned long lb = 0;

    while (graph->node_degree_map.find(0) != graph->node_degree_map.end()) {
        bool* vertex = *(graph->node_degree_map[0].begin());
        delete_node_erase(vertex, graph);
        lb++;
    }
    return lb;
}