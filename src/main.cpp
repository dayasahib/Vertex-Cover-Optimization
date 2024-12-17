#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stack>
#include <algorithm>
#include <vector>
#include <chrono>
#include <queue>
#include <iterator>
#include <cmath>

#include "graph.h"
#include "lb.h"
#include "dr.h"
#include "ub.h"




std::stack<bool*>* resstack = new std::stack<bool*>; // stack to store final vertex cover



void printnodestack(std::stack<bool*>* stck, Graph* graph) {
    bool* node;
    while (!stck->empty()) {
        node = stck->top();
        stck->pop();
        std::cout << graph->strings[node] << std::endl;
    }
}


void print_solution_graph(Graph* graph) {
    for (const auto& node: graph->neighbours) {
        if (*node.first) {
            std::cout << graph->strings[node.first] << std::endl;
        }
    }
}


void print_node_set_with_deleted(std::set<bool*> s, Graph* graph) {
    for (bool* node: s) {
        std::cout << graph->strings[node] << std::endl;
    }
}



void print_vertex_set(std::set<bool*> s, Graph* graph) {
    for (const auto &b: s) {
        std::cout << graph->strings[b] << " ";
    }
    std::cout << std::endl;
}




int main() {
    Graph _graph = readin();
    Graph* graph = &_graph;

    unsigned long lb = 0;


    std::stack<bool*> res_stack;
    deg_one(graph, &res_stack);
    max_deg_heur(graph, &res_stack);

    printnodestack(&res_stack, graph);

    /// only for development purposes
    /// REMOVE BEFORE SUBMISSION
    // std::cout << s << std::endl;


    delete_graph(graph);

    return 0;
}