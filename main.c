#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "data_structures.h"

Solution *backtrack_vrp_solve(Graph *g, IntLinkedList *c, Vertice *origin) {
  Solution *best_solution = NULL, *solution;
  unsigned int degree, level, i;
  Tree *root, *current, *nnode;
  Vertice *vertice, *v, **v_ignore;
  Edge *edge, **e_ignore, **out_edges;
  IntLinkedList *vehicles;
  
  root = malloc(sizeof(Tree));
  init_tree(root, origin, NULL, false, NULL, c);

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
      vehicles = deep_copy(current->available_vehicles);
      if (!remove_value(vehicles, current->path_demand_so_far)) {
        destroy_linkedlist(vehicles);
        next_leaf(&current);
        continue;
      }
      current->available_vehicles = vehicles;
      current->removed_vehicle = true;
      current->path_demand_so_far = 0;
      if (current->n_vertices_traversed == g->n) {
        solution = malloc(sizeof(Solution));
        build_solution(current, solution);

        if (!best_solution || solution->cost < best_solution->cost) {
          destroy_solution(best_solution);
          best_solution = solution;
          printf("Best Solution so far: %f\n", solution->cost);
          printf("Using this many edges: %d\n", solution->n_edges);
        }
        else {
          destroy_solution(solution);
        }

        next_leaf(&current);
        continue;
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

  if (argc != 2) {
    printf("ERROR: Please only specify the filename to read from.");
    return 1;
  }

  unsigned int i, j, c;

  size_t bufsize = 32;
  char *buffer = malloc(bufsize*sizeof(char));
  FILE *file;
  file = fopen(argv[1], "r");
  getline(&buffer, &bufsize, file);

  unsigned int n_v = atoi(buffer);
  unsigned int n_e = (n_v * n_v) - n_v;

  Vertice **vertices = malloc(n_v*sizeof(Vertice *));
  for (i = 0; i < n_v; i++) {
    vertices[i] = malloc(sizeof(Vertice));
    getline(&buffer, &bufsize, file);
    init_vertice(vertices[i], i, atoi(buffer));
  }

  Graph *g = malloc(sizeof(Graph));
  init_graph(g, n_v, vertices);
  for (i = 0; i < n_v; i++) {
    init_graph_edges(g, i, n_v-1);
  }

  Edge **edges = malloc(n_e*sizeof(Edge *));
  c = 0;
  for (i = 0; i < n_v; i++) {
    for (j = 0; j < n_v; j++) {
      if (i == j) continue;
      edges[c] = malloc(sizeof(Edge));
      getline(&buffer, &bufsize, file);
      init_edge(edges[c], vertices[i], vertices[j], atof(buffer));
      if (j > i) {
        g->edges[i][j-1] = edges[c];
      }
      else {
        g->edges[i][j] = edges[c];
      }
      c++;
    }
  }

  IntLinkedList *vehicles = malloc(sizeof(IntLinkedList));
  for (i = 0; i < n_v; i++) {
    getline(&buffer, &bufsize, file);
    add_value(vehicles, atoi(buffer));
  }

  Solution *s = backtrack_vrp_solve(g, vehicles, vertices[0]);

  if (s) {
    printf("N Edges: %d\n", s->n_edges);
    printf("Cost: %f\n", s->cost);
  } else {
    printf("No solution!\n");
  }

  destroy_solution(s);
  destroy_linkedlist(vehicles);

  for (i = 0; i < n_v; i++) {
    destroy_vertice(vertices[i]);
  }

  for (i = 0; i < n_e; i++) {
    destroy_edge(edges[i]);
  }

  free(vertices);
  free(edges);

  destroy_graph(g);

  return 0;
}