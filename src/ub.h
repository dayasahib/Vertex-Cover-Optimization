#include <stack>
#include "graph.h"

#ifndef PP_AE_VC_UB_H
#define PP_AE_VC_UB_H

unsigned long max_deg_heur(Graph* graph, std::stack<bool*>* res_stack);

unsigned long max_deg_heur_rand(Graph* graph, std::stack<bool*>* res_stack);

#endif //PP_AE_VC_UB_H
