#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "data_structures.h"

#define N_V 1000
#define N_E ((N_V * N_V) - N_V)

Solution *backtrack_vrp_solve(Graph *g, Vertice *origin) {
  Solution *best_solution = NULL, *solution;
  unsigned int degree, level, i;
  Tree *root, *current, *nnode;
  Vertice *vertice, *v, **v_ignore;
  Edge *edge, **e_ignore, **out_edges;
  
  root = malloc(sizeof(Tree));
  init_tree(root, origin, NULL, false, NULL);

  out_edges = edges_out(g, origin->id);
  edge = out_edges[0];

  nnode = malloc(sizeof(Tree));
  init_tree_from_parent(nnode, root, edge->dest, edge, true, origin);
  add_child_to_parent(root, nnode);

  nnode = malloc(sizeof(Tree));
  init_tree_from_parent(nnode, root, edge->origin, edge, false, origin);
  add_child_to_parent(root, nnode);

  current = root->left_child;

  while (current) {
    vertice = current->current_v;
    level = current->level;

    if (vertice == origin) {
      current->path_demand_so_far = 0;
      if (current->n_vertices_traversed == g->n) {
        solution = malloc(sizeof(Solution));
        build_solution(current, solution);

        if (!best_solution || solution->cost < best_solution->cost) {
          destroy_solution(best_solution);
          best_solution = solution;
        }
        else {
          destroy_solution(solution);
        }

        next_leaf(&current);
      }
    }

    v_ignore = calloc(level, sizeof(Vertice *));
    e_ignore = calloc(level, sizeof(Edge *));
    build_traversed_vertices_edges(current, origin, v_ignore, e_ignore);
    out_edges = edges_out(g, vertice->id);
    degree = degree_out(g, vertice->id);
    for (i = 0; i < degree; i++) {
      edge = out_edges[i];
      if (in_edges_array(e_ignore, edge, level)) continue;
      v = edge->dest;
      if (in_vertices_array(v_ignore, v, level)) continue;

      if (!best_solution ||
          current->cost_so_far + edge->cost < best_solution->cost) {
        nnode = malloc(sizeof(Tree));
        init_tree_from_parent(nnode, current, edge->dest, edge, true, origin);
        add_child_to_parent(current, nnode);
      }
      if (strongly_connected(g, e_ignore, v_ignore, level, edge, origin)) {
        nnode = malloc(sizeof(Tree));
        init_tree_from_parent(nnode, current, edge->origin, edge, false, origin);
        add_child_to_parent(current, nnode);
      }

      break;
    }
    free(e_ignore);
    free(v_ignore);

    next_leaf(&current);
  }

  return best_solution;
}

int main(int argc, char const *argv[]) {
  unsigned int i, j, c;
  Vertice **vertices = malloc(N_V*sizeof(Vertice *));
  for (i = 0; i < N_V; i++) {
    vertices[i] = malloc(sizeof(Vertice));
    init_vertice(vertices[i], i, i);
  }

  Graph *g = malloc(sizeof(Graph));
  init_graph(g, N_V, vertices);
  for (i = 0; i < N_V; i++) {
    init_graph_edges(g, i, N_V-1);
  }

  Edge **edges = malloc(N_E*sizeof(Edge *));
  c = 0;
  for (i = 0; i < N_V; i++) {
    for (j = 0; j < N_V; j++) {
      if (i == j) continue;
      edges[c] = malloc(sizeof(Edge));
      init_edge(edges[c], vertices[i], vertices[j], i+j);
      if (j > i) {
        g->edges[i][j-1] = edges[c];
      }
      else {
        g->edges[i][j] = edges[c];
      }
      c++;
    }
  }

  Solution *s = backtrack_vrp_solve(g, vertices[0]);

  printf("Cost: %f\n", s->cost);

  destroy_solution(s);

  for (i = 0; i < N_V; i++) {
    destroy_vertice(vertices[i]);
  }

  for (i = 0; i < N_E; i++) {
    destroy_edge(edges[i]);
  }

  free(vertices);
  free(edges);

  destroy_graph(g);

  return 0;
}
