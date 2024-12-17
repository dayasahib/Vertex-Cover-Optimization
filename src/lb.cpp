#include "lb.h"
#include "graph.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <stack>
#include <algorithm>
#include <chrono>
#include <queue>
#include <iterator>
#include <cmath>



unsigned long basic_lb(Graph* graph) {
    auto max_degree_node = std::max_element(
            get_nodecounts(graph)->begin(), get_nodecounts(graph)->end(),
            [](const auto &a, const auto &b) {
                return a.second < b.second;
            }
    );
    return get_m(graph) / max_degree_node->second;
}


// Function to convert a regular graph to a bipartite graph
Graph graphToBipartite(Graph* graph) {
    Graph bipartiteGraph = Graph();
    bipartiteGraph.edges = new Edge[2*graph->m];
    bipartiteGraph.indicators = new bool[4*graph->m];

    // Create left and right partitions
    for (const auto& node : graph->strings) {
        if (*graph->indicatorps[node.second]) {
            bool *leftNode = new bool(true);
            bool *rightNode = new bool(true);

            bipartiteGraph.L.insert(leftNode);
            bipartiteGraph.R.insert(rightNode);

            bipartiteGraph.strings[leftNode] = graph->strings[node.first] + "_L";
            bipartiteGraph.strings[rightNode] = graph->strings[node.first] + "_R";

            bipartiteGraph.indicatorps[graph->strings[node.first] + "_L"] = leftNode;
            bipartiteGraph.indicatorps[graph->strings[node.first] + "_R"] = rightNode;

            bipartiteGraph.neighbours[leftNode] = std::make_tuple(0, std::set<bool*>());
            bipartiteGraph.neighbours[rightNode] = std::make_tuple(0, std::set<bool*>());
        }
    }

    // Create edges in the bipartite graph
    for (unsigned long i = 0; i < graph->m; ++i) {
        if ( std::get<0>(graph->edges[i]) && std::get<1>(graph->edges[i]) ) {
            bool* leftNode = bipartiteGraph.indicatorps[graph->strings[std::get<0>(graph->edges[i])] + "_L"];
            bool* rightNode = bipartiteGraph.indicatorps[graph->strings[std::get<1>(graph->edges[i])] + "_R"];

            bool* leftNode2 = bipartiteGraph.indicatorps[graph->strings[std::get<1>(graph->edges[i])] + "_L"];
            bool* rightNode2 = bipartiteGraph.indicatorps[graph->strings[std::get<0>(graph->edges[i])] + "_R"];

            bipartiteGraph.edges[i*2] = (std::make_tuple(leftNode, rightNode));
            bipartiteGraph.edges[i*2 + 1] = (std::make_tuple(leftNode2, rightNode2));

            std::get<0>(bipartiteGraph.neighbours[leftNode]) += 1;
            std::get<1>(bipartiteGraph.neighbours[leftNode]).insert(rightNode);

            std::get<0>(bipartiteGraph.neighbours[rightNode]) += 1;
            std::get<1>(bipartiteGraph.neighbours[rightNode]).insert(leftNode);

            std::get<0>(bipartiteGraph.neighbours[leftNode2]) += 1;
            std::get<1>(bipartiteGraph.neighbours[leftNode2]).insert(rightNode2);

            std::get<0>(bipartiteGraph.neighbours[rightNode2]) += 1;
            std::get<1>(bipartiteGraph.neighbours[rightNode2]).insert(leftNode2);

            if (std::get<0>(bipartiteGraph.neighbours[leftNode]) == 1) {
                bipartiteGraph.node_degree_map[1].insert(leftNode);
            } else {
                bipartiteGraph.node_degree_map[1].erase(leftNode);
            }
            if (std::get<0>(bipartiteGraph.neighbours[rightNode]) == 1) {
                bipartiteGraph.node_degree_map[1].insert(rightNode);
            } else {
                bipartiteGraph.node_degree_map[1].erase(rightNode);
            }

            if (std::get<0>(bipartiteGraph.neighbours[leftNode2]) == 1) {
                bipartiteGraph.node_degree_map[1].insert(leftNode2);
            } else {
                bipartiteGraph.node_degree_map[1].erase(leftNode2);
            }
            if (std::get<0>(bipartiteGraph.neighbours[rightNode2]) == 1) {
                bipartiteGraph.node_degree_map[1].insert(rightNode2);
            } else {
                bipartiteGraph.node_degree_map[1].erase(rightNode2);
            }
        }
    }

    return bipartiteGraph;
}


void bfs(Graph* graph, std::map<bool*, bool*>& matching, std::map<bool*, int>& dist) {
    std::deque<bool*> queue;

    for (bool* u : graph->L) {
        if (matching[u] == nullptr) {
            dist[u] = 0;
            queue.push_back(u);
        } else {
            dist[u] = std::numeric_limits<int>::max();
        }
    }

    dist[nullptr] = std::numeric_limits<int>::max();

    while (!queue.empty()) {
        bool* u = queue.front();
        queue.pop_front();

        if (dist[u] < dist[nullptr]) {
            for (bool* v : get_node_neighbours(u, graph)) {
                if (dist[matching[v]] == std::numeric_limits<int>::max()) {
                    dist[matching[v]] = dist[u] + 1;
                    queue.push_back(matching[v]);
                }
            }
        }
    }
}

// Function to perform DFS
bool dfs(bool* u, Graph* graph, std::map<bool*, bool*>& matching, std::map<bool*, int>& dist) {
    if (u != nullptr) {
        for (bool* v : get_node_neighbours(u, graph)) {
            if (dist[matching[v]] == dist[u] + 1 && dfs(matching[v], graph, matching, dist)) {
                matching[u] = v;  // Corrected line
                matching[v] = u;
                return true;
            }
        }
        dist[u] = std::numeric_limits<int>::max();
        return false;
    }
    return true;
}

// Hopcroft-Karp algorithm
std::map<bool*, bool*> hopcroft_karp(Graph* graph) {
    std::map<bool*, bool*> matching;
    std::map<bool*, int> dist;

    for (bool* u : graph->L) {
        matching[u] = nullptr;
    }

    while (true) {
        bfs(graph, matching, dist);
        bool augmenting_paths_found = false;

        for (bool* u : graph->L) {
            if (matching[u] == nullptr && dfs(u, graph, matching, dist)) {
                augmenting_paths_found = true;
            }
        }

        if (!augmenting_paths_found) {
            break;
        }
    }
    std::map<bool*, bool*> valid_matching;

    // Copy valid entries to the new map
    for (const auto& entry : matching) {
        if (entry.second != nullptr) {
            valid_matching[entry.first] = entry.second;
        }
    }
    return valid_matching;
}

void print_match(Graph* graph, std::map<bool*, bool*> matching){
    // Print the matching result (for testing)
    std::cout << "Matching Result:" << std::endl;
    for (const auto& entry : matching) {
        std::cout << graph->strings[entry.first] << " --> " << graph->strings[entry.second] << std::endl;
    }
}

unsigned long lpb(Graph* graph) {
    Graph _bp = graphToBipartite(graph);
    Graph* bp = &_bp;
    std::map<bool*, bool*> matching = hopcroft_karp(bp);
//    print_match(bp, matching);
    return matching.size()/4;
}


void intersection(std::set<bool*>* set1, std::set<bool*>* set2) {
    /// results are written into set1!
    auto it = set1->begin();
    while (it != set1->end()) {
        if (!*it) continue;
        if (set2->find(*it) == set2->end()) {
            it = set1->erase(it);
        } else {
            ++it;
        }
    }
}

unsigned long clique_value(bool* node, Graph* graph) {
    // set to 3 so that it does not take parts of circles into clique cover.
    unsigned long min_clique_size = 3;
    unsigned long lb = 0;

    std::set<bool*> clique;
    clique.insert(node);
    clique.insert(std::get<1>(graph->neighbours[node]).begin(), std::get<1>(graph->neighbours[node]).end());
    //print_vertex_set(clique, graph);

    if (std::get<0>(graph->neighbours[node]) < min_clique_size) return 0;
    for (const auto &neighbour: get_node_neighbours(node, graph)) {
        if (clique.find(neighbour) != clique.end()) {
            //print_vertex_set(clique, graph);
            //print_vertex_set(get_node_neighbours(neighbour, graph), graph);
            intersection(&clique, &std::get<1>(graph->neighbours[neighbour]));
            //std::cout << "here" << std::endl;
            clique.insert(neighbour);
            //print_vertex_set(clique, graph);
            if (clique.size() < min_clique_size) break;
        }
    }
    //print_vertex_set(clique, graph);
    if (clique.size() >= min_clique_size) {
        lb = clique.size() - 1;
        for (auto &it: clique) {
            delete_node_erase(it, graph);
        }
    }
    return lb;
}


unsigned long clique_cover_lb(Graph* graph) {
    unsigned long lower_bound = 0;

    for (const auto &node: graph->neighbours) {
        if (!*node.first) continue;
        //std::cout << graph->strings[node.first] << std::endl;
        lower_bound += clique_value(node.first, graph);
        //std::cout << lower_bound << std::endl;
    }

    return lower_bound;
}



void push_neighbours_on_queue(std::queue<bool*>* queue, const std::set<bool*>& neighbours, bool* parent, std::map<bool*, bool*>* parents, std::set<bool*>* looked_at) {
    for (bool* it: neighbours) {
        if (looked_at->find(it) != looked_at->end()) continue;
        if (!*it) continue;
        queue->push(it);
        (*parents)[it] = parent;
    }
}


unsigned long handle_found_circle(bool* root, std::map<bool*, bool*> parents, Graph* graph) {
    unsigned long size = 1; // one for the root
    bool* temp = parents[root];
    while (temp != root) {
        size++;
        delete_node_erase(temp, graph);
        temp = parents[temp];
    }
    delete_node_erase(root, graph);
    return std::ceil(size / 2);
}


unsigned long find_cycle(bool* root, std::set<bool*> looked_at, Graph* graph) {
    if (std::get<0>(graph->neighbours[root]) < 2) return 0;
    if (looked_at.find(root) != looked_at.end()) return 0;

    std::map<bool*, bool*> parents; // stores parent node.
    std::set<bool*> nodes_in_path;
    nodes_in_path.insert(root);

    std::queue<bool*> waiting;
    push_neighbours_on_queue(&waiting, std::get<1>(graph->neighbours[root]), root, &parents, &looked_at);

    while (!waiting.empty() && waiting.size() < 40000) {
        bool* cur_node = waiting.front();
        waiting.pop();


        if (!*cur_node) continue;
        //if (looked_at.find(cur_node) != looked_at.end()) continue;

        if (nodes_in_path.find(cur_node) != nodes_in_path.end()) {
            // found circle
            return handle_found_circle(cur_node, parents, graph);
        }
        if (std::get<0>(graph->neighbours[cur_node]) < 2) {

            continue;
        }
        nodes_in_path.emplace(cur_node);
        looked_at.emplace(cur_node);
        push_neighbours_on_queue(&waiting, std::get<1>(graph->neighbours[cur_node]), cur_node, &parents, &looked_at);
    }
    return 0;
}


unsigned long cycle_bound(Graph* graph) {
    auto start_time = std::chrono::high_resolution_clock::now();
    unsigned long lb = 0;
    std::set<bool*> looked_at;

    auto it = graph->neighbours.begin();
    while (it != graph->neighbours.end()) {

        auto cur_time = std::chrono::high_resolution_clock::now();
        auto e_t = std::chrono::duration_cast<std::chrono::seconds>(cur_time - start_time);
        if (e_t >= std::chrono::seconds(30)) return lb;

        if (!*it->first) {}
        else if (looked_at.find(it->first) != looked_at.end()) {}
        else {
            lb += find_cycle(it->first, looked_at, graph);
            looked_at.insert(it->first);
        }
        ++it;
    }
    return lb;
}

