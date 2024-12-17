
#include <iostream>
#include <map>
#include <set>


#ifndef PP_AE_VC_GRAPH_H
#define PP_AE_VC_GRAPH_H



typedef std::tuple<bool*, bool*> Edge;
typedef std::tuple<unsigned long, std::set<bool*>> neighbour_map_value;

/*
 * New Graph struct to store a Graph.
 * Implemented so it is possible to work with multiple graphs.
 * Manly implemented for LP bound in exercise 04.
 *
 * */
typedef struct __graph {
    Edge *edges;
    Edge* lastfirstedgeposition;
    std::map<bool*, std::string> strings;
    unsigned long m = 0;
    std::map<std::string, bool*> indicatorps;
    bool *indicators;
    std::map<bool*, unsigned long> nodecounts;


    /*
     * neighbours map stores neighbours and degree of a node.
     * used for heuristics.
     *
     * NEIGHBOUR NODES ARE NOT DELETED FROM SET.
     * IF NODE IS NOT IN GRAPH ITS BOOLEAN VALUE IS FALSE.
     *
     * */
    std::map<bool*, neighbour_map_value> neighbours;

    /*
     * node_degree_map takes a certain degree as a key and
     * contains a set of all nodes with such degree.
     * Used for degree-one rule.
     * Can be used for many degree based rules.
     *
     * It currently only contains a degree one entry.
     * IT DOES NOT KEEP TRACK OF EVERY NODES DEGREE.
     *
     * */
    std::map<unsigned long, std::set<bool*>> node_degree_map;

    /*
     * L & R are sets of nodes in case the graph is bipartite
     */

    std::set<bool*> L;
    std::set<bool*> R;

} Graph;


void print_deg_map(Graph* graph);

/*
 * ATENTION: This function does NOT erase the node from the neighbours set.
 * This is faster than actual erasing.
 * If you want to erase the node from the neighbours set see delete_node_erase();
 *
 */
void delete_node(bool* node, Graph* graph);

/*
 * ATENTION: This function does NOT insert the node into the neighbours set.
 * This is faster than actual insertion.
 * If you want to erase the node from the neighbours set see undelete_node_insert();
 *
 */
void undelete_node(bool* node, Graph* graph);

/*
 * ATENTION: This function actually deletes the node from the neighbours set
 * THIS FUNCTION IS MUCH SLOWER THAN delete_node();
 * DO NOT USE THIS IF NOT NECESSARY
 *
 */
void delete_node_erase(bool* node, Graph* graph);

/*
 * ATENTION: This function actually inserts the node into the neighbours set
 * THIS FUNCTION IS MUCH SLOWER THAN undelete_node();
 * DO NOT USE THIS IF NOT NECESSARY
 *
 */
void undelete_node_insert(bool* node, Graph* graph);

std::set<bool*> get_node_neighbours(bool* node, Graph* graph);

unsigned long get_node_degree(bool* node, Graph* graph);

void print_graph(Graph* graph);



void delete_graph(Graph* graph);

unsigned long get_m(Graph* graph);

// void set_m(unsigned long);

std::map<bool*, unsigned long>* get_nodecounts(Graph* graph);

Edge* get_last_first_edge_position(Graph* graph);

void set_last_first_edge_position(Edge*, Graph* graph);

Edge* get_first_edge(Graph* graph);

void print_edge_array(Graph* graph);

Graph readin();


#endif //PP_AE_VC_GRAPH_H
