/*
 *
 * Lower Bounds.
 * Implemented:
 * - clique cover bound
 * - cycle bound (is not fast enough to be usefull)
 * - linear programming
 *
 */

#ifndef PP_AE_VC_LB_H
#define PP_AE_VC_LB_H



#include "graph.h"

/*
 * Compute a clique cover lower bound.
 * Removes most cliques from the given Graph.
 * Uses the delete_node_erase function!
 * Returns the lb value after removing the cliques.
 */
unsigned long clique_cover_lb(Graph*);

/*
 * Compute a cycle cover lower bound.
 * THIS FUNCTION IS SLOW. IT HAS A 30 SECOND TIME LIMIT.
 * DO NOT USE WITHOUT MODIFYING. USE lbp INSTEAD!
 * Removes most cycles from a graph.
 * Uses the delete_node_erase function!
 * Returns the lb value after removing the cycles.
 */
unsigned long cycle_bound(Graph*);

/*
 * Compute lp bound of a graph.
 * returns the size of the lp bound.
 */
unsigned long lpb(Graph*);

/*
 * Compute a very basic lower bound.
 * Does not remove any nodes.
 * Very fast. But not very good.
 * returns very basic lower bound.
 */
unsigned long basic_lb(Graph*);


#endif //PP_AE_VC_LB_H
