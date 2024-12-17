#include <set>

#include "ub.h"
#include "graph.h"

#include <random>
#include <stack>




unsigned long max_deg_heur(Graph* graph, std::stack<bool*>* res_stack) {
    unsigned long size = 0;
    while (!graph->node_degree_map.empty()) {
        // randomization?
        bool* node = *graph->node_degree_map.rbegin()->second.begin();
        res_stack->push(node);
        delete_node_erase(node, graph);
        size++;
    }
    return size;
}


unsigned long max_deg_heur_rand(Graph* graph, std::stack<bool*>* res_stack) {
    unsigned long size = 0;
    while (!graph->node_degree_map.empty()) {
        // randomization?

        std::random_device rd;
        std::mt19937 gen(rd());

        std::set<bool*>* node_set = &graph->node_degree_map.rbegin()->second;

        std::uniform_int_distribution<> dis(0, node_set->size() - 1);
        int rand_i = dis(gen);
        auto iter = node_set->begin();
        std::advance(iter, rand_i);

        bool* node = *iter;

        res_stack->push(node);
        delete_node_erase(node, graph);
        size++;
    }
    return size;
}



void local_search(Graph* graph) {

}