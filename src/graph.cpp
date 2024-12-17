
#include "graph.h"
#include "dr.h"


#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <algorithm>




void print_deg_map(Graph* graph) {
    for (const auto& set: graph->node_degree_map) {
        std::cout << set.first << " : ";
        for (const auto& node: set.second) {
            std::cout << graph->strings[node] << " ";
        }
        std::cout << std::endl;
    }
}



void remove_node_degree(Graph* graph, bool* node) {
    graph->node_degree_map[std::get<0>(graph->neighbours[node])].erase(node);
    if (graph->node_degree_map[std::get<0>(graph->neighbours[node])].begin() == graph->node_degree_map[std::get<0>(graph->neighbours[node])].end()) {
        graph->node_degree_map.erase(std::get<0>(graph->neighbours[node]));
    }
}


void update_node_degree(Graph* graph, bool* node) {
    unsigned long val = std::get<0>(graph->neighbours[node]);
    auto ref = graph->node_degree_map.find(val);
    if( ref  != graph->node_degree_map.end()) {
        ref->second.insert(node);
    } else {
        graph->node_degree_map[val] = {node};
    }
}


void delete_node(bool* node, Graph* graph) {
    /*
     * ATENTION: This function does NOT erase the node from the neighbours set.
     * This is faster than actual erasing.
     * If you want to erase the node from the neighbours set see delete_node_erase();
     *
     */
    *node = false;
    remove_node_degree(graph, node);
    for (const auto& neighbour: std::get<1>(graph->neighbours[node])) {
        remove_node_degree(graph, neighbour);
        std::get<0>(graph->neighbours[neighbour]) -= 1;
        update_node_degree(graph, neighbour);
    }
}


void undelete_node(bool* node, Graph* graph) {
    /*
     * ATENTION: This function does NOT insert the node into the neighbours set.
     * This is faster than actual insertion.
     * If you want to erase the node from the neighbours set see undelete_node_insert();
     *
     */
    *node = true;
    if (std::get<0>(graph->neighbours[node]) == 1) {
        graph->node_degree_map[1].insert(node);
    }
    for (const auto& neighbour: std::get<1>(graph->neighbours[node])) {
        remove_node_degree(graph, neighbour);
        std::get<0>(graph->neighbours[neighbour]) += 1;
        update_node_degree(graph, neighbour);
    }
}


void delete_node_erase(bool* node, Graph* graph) {
    /*
     * ATENTION: This function actually deletes the node from the neighbours set
     * THIS FUNCTION IS MUCH SLOWER THAN delete_node();
     * DO NOT USE THIS IF NOT NECESSARY
     *
     */
    *node = false;
    remove_node_degree(graph, node);
    for (const auto& neighbour: std::get<1>(graph->neighbours[node])) {
        remove_node_degree(graph, neighbour);
        std::get<0>(graph->neighbours[neighbour]) -= 1;
        update_node_degree(graph, neighbour);
        std::get<1>(graph->neighbours[neighbour]).erase(node);
    }
    deg_zero(graph);
}


void undelete_node_insert(bool* node, Graph* graph) {
    /*
     * ATENTION: This function actually inserts the node into the neighbours set
     * THIS FUNCTION IS MUCH SLOWER THAN undelete_node();
     * DO NOT USE THIS IF NOT NECESSARY
     *
     */
    *node = true;
    if (std::get<0>(graph->neighbours[node]) == 1) {
        graph->node_degree_map[1].insert(node);
    }
    for (const auto& neighbour: std::get<1>(graph->neighbours[node])) {
        remove_node_degree(graph, neighbour);
        std::get<0>(graph->neighbours[neighbour]) += 1;
        update_node_degree(graph, neighbour);
        std::get<1>(graph->neighbours[neighbour]).insert(node);
    }
}


std::set<bool*> get_node_neighbours(bool* node, Graph* graph) {
    ///  a node might not be active eventhough it is in the returned set.
    ///  if a node is not active/not in the graph, its value is set to false
    return std::get<1>(graph->neighbours[node]);
}


unsigned long get_node_degree(bool* node, Graph* graph) {
    return std::get<0>(graph->neighbours[node]);
}


void print_graph(Graph* graph) {
    for (const auto& node: graph->neighbours) {
        if (!*node.first) continue;
        std::cout << graph->strings[node.first] << " : ";
        neighbour_map_value temp = node.second;
        std::cout << std::get<0>(temp) << " : ";
        for (const auto& neighb: std::get<1>(temp)) {
            std::cout << graph->strings[neighb] << " ";
        }
        std::cout << *node.first;
        std::cout << std::endl;
    }
}



void print_edge_array(Graph* graph) {
    // count edges
    unsigned long num_edges = 0;
    for (int i = 0; i < graph->m; i++) {
        if (*std::get<0>(graph->edges[i]) && *std::get<1>(graph->edges[i])) {
            num_edges += 1;
        }
    }

    unsigned long num_nodes = 0;
    for (auto &neighbour: graph->neighbours) {
        if (*neighbour.first) {
            num_nodes += 1;
        }
    }

    std::cout << num_nodes << " " << num_edges << std::endl;

    // print out edges
    for (int i = 0; i < graph->m; i++) {
        if (*std::get<0>(graph->edges[i]) && *std::get<1>(graph->edges[i])) {
            std::cout << graph->strings[std::get<0>(graph->edges[i])] << " " << graph->strings[std::get<1>(graph->edges[i])] << std::endl;
        }
    }
}



void delete_graph(Graph* graph){
    delete(graph->edges);
    delete(graph->indicators);
}


unsigned long get_m(Graph* graph) {
    return graph->m;
}


void set_m(unsigned long new_m, Graph* graph) {
    graph->m = new_m;
}


std::map<bool*, unsigned long>* get_nodecounts(Graph* graph) {
    return (&graph->nodecounts);
}



Edge* get_last_first_edge_position(Graph* graph) {
    return graph->lastfirstedgeposition;
}


void set_last_first_edge_position(Edge* lfe_pos, Graph* graph) {
    graph->lastfirstedgeposition = lfe_pos;
}


Edge* get_first_edge(Graph* graph) {
    if (!graph->lastfirstedgeposition) graph->lastfirstedgeposition = graph->edges;
    auto first_edge = std::find_if(graph->lastfirstedgeposition, graph->edges + graph->m,
                                   [](const auto &edge) { return *std::get<0>(edge) && *std::get<1>(edge); });

    if (first_edge != graph->edges + graph->m) {
        graph->lastfirstedgeposition = first_edge;
        return first_edge;
    } else {
        return nullptr;
    }
}



Graph readin() {

    // DEV
    // std::ifstream file("/home/notna/Uni/algoeng/PP-AE-VC/samples/vc/in/vc1aa.in");
    Graph graph = Graph();
    //graph.node_degree_map[1] = std::set<bool*>();
    bool b = true;


    // TODO: read in Graph size and allocate array edges.
    std::string line;
    int n;
    while (std::getline(std::cin, line) && !line.empty()) {
        if (line[0] == '#') continue;
        std::istringstream isstream(line);
        std::string tempn;
        std::string tempm;
        isstream >> tempn;
        n = stoi(tempn);
        isstream >> tempm;
        graph.m = stoi(tempm);

        break;
    }

    graph.edges = new Edge[graph.m];
    // maybe queue is better.
    graph.indicators = new bool[2*graph.m];

    // TODO: read in Edges and store them in edges and translation
    int edgeindex = 0;
    int indicatorindex = 0;

    while (std::getline(std::cin, line) && !line.empty()) {

        if (line[0] == '#') continue;
        std::stringstream stream(line);
        std::string a;
        std::string b;
        stream >> a;
        stream >> b;
        bool *atrans = graph.indicatorps[a];
        bool *btrans = graph.indicatorps[b];

        if (!atrans) {
            graph.indicators[indicatorindex] = true;
            graph.indicatorps[a] = &(graph.indicators)[indicatorindex];
            graph.strings[&(graph.indicators)[indicatorindex]] = a;
            atrans = &(graph.indicators)[indicatorindex];
            graph.nodecounts[atrans] = 0;

            graph.neighbours[atrans] = std::make_tuple(0, std::set<bool *>());

            indicatorindex++;
        }
        if (!btrans) {
            graph.indicators[indicatorindex] = true;
            graph.indicatorps[b] = &(graph.indicators)[indicatorindex];
            graph.strings[&(graph.indicators)[indicatorindex]] = b;
            btrans = &(graph.indicators)[indicatorindex];
            graph.nodecounts[btrans] = 0;

            graph.neighbours[btrans] = std::make_tuple(0, std::set<bool *>());

            indicatorindex++;
        }

        graph.nodecounts[atrans]++;
        graph.nodecounts[btrans]++;
        // maybe arrays are faster?
        std::get<0>(graph.edges[edgeindex]) = atrans;
        std::get<1>(graph.edges[edgeindex]) = btrans;

        remove_node_degree(&graph, atrans);
        std::get<0>(graph.neighbours[atrans]) += 1;
        /*if (std::get<0>(graph.neighbours[atrans]) == 1) {
            graph.node_degree_map[1].insert(atrans);
        } else {
            graph.node_degree_map[1].erase(atrans);
        }*/
        update_node_degree(&graph, atrans);
        std::get<1>(graph.neighbours[atrans]).insert(btrans);
        remove_node_degree(&graph, btrans);
        std::get<0>(graph.neighbours[btrans]) += 1;
        /*if (std::get<0>(graph.neighbours[btrans]) == 1) {
            graph.node_degree_map[1].insert(btrans);
        } else {
            graph.node_degree_map[1].erase(btrans);
        }*/
        update_node_degree(&graph, btrans);
        std::get<1>(graph.neighbours[btrans]).insert(atrans);

        // edgecounter++;
        edgeindex++;
    }
    return graph;
}