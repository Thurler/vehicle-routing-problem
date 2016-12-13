#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "data_structures.h"

Solution *branch_bound_vrp_solve(Graph *g, IntLinkedList *c, Vertice *origin,
                                 int n_iter) {
  Solution *best_solution = NULL, *solution;
  unsigned int degree, level, i, it_counter = 0;
  Tree *root, *current, *nnode;
  Vertice *vertice, *v, **v_ignore;
  Edge *edge, **e_ignore, **out_edges;
  IntLinkedList *vehicles;
  double global_upper_bound;

  root = malloc(sizeof(Tree));
  init_tree(root, origin, NULL, false, NULL, c, origin, g);
  printf("Root Lower Bound: %f\n", root->lower_bound);
  printf("Root Upper Bound: %f\n\n", root->upper_bound);

  out_edges = edges_out(g, origin->id);
  edge = out_edges[0];

  nnode = malloc(sizeof(Tree));
  init_tree_from_parent(nnode, root, edge->dest, edge, true, origin, g);
  add_child_to_parent(root, nnode);
  global_upper_bound = nnode->upper_bound;

  nnode = malloc(sizeof(Tree));
  init_tree_from_parent(nnode, root, edge->origin, edge, false, origin, g);
  add_child_to_parent(root, nnode);
  if (nnode->upper_bound < global_upper_bound) {
    global_upper_bound = nnode->upper_bound;
  }

  current = root->left_child;

  printf("Global Upper Bound: %f\n", global_upper_bound);

  while (current) {
    it_counter++;
    if (n_iter && n_iter > 0 && it_counter > n_iter) {
      destroy_tree(root);
      return best_solution;
    }
    else if (n_iter && n_iter < 0 && best_solution) {
      destroy_tree(root);
      return best_solution;
    }

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
          print_solution(solution);
        }
        else {
          destroy_solution(solution);
        }

        next_leaf(&current);
        continue;
      }
    }

    if (current->lower_bound >= global_upper_bound) {
      next_leaf(&current);
      continue;
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
        init_tree_from_parent(nnode, current, edge->dest, edge, true, origin, g);
        if (nnode->path_demand_so_far > nnode->available_vehicles->tail->value) {
          destroy_tree(nnode);
        }
        else if (nnode->lower_bound > global_upper_bound) {
          destroy_tree(nnode);
        }
        else {
          add_child_to_parent(current, nnode);
          if (nnode->upper_bound < global_upper_bound) {
            global_upper_bound = nnode->upper_bound;
            printf("Global Upper Bound true: %f\n", global_upper_bound);
          }
        }
      }
      if (strongly_connected(g, e_ignore, v_ignore, level, edge, origin)) {
        nnode = malloc(sizeof(Tree));
        init_tree_from_parent(nnode, current, edge->origin, edge, false, origin, g);
        if (nnode->lower_bound >= global_upper_bound) {
          destroy_tree(nnode);
        }
        else {
          add_child_to_parent(current, nnode);
          if (nnode->upper_bound < global_upper_bound) {
            global_upper_bound = nnode->upper_bound;
            printf("Global Upper Bound false: %f\n", global_upper_bound);
          }
        }
      }

      break;
    }
    free(e_ignore);
    free(v_ignore);

    next_leaf(&current);
  }

  return best_solution;
}

Solution *heuristic_vrp_solve(Graph *g, IntLinkedList *c, Vertice *origin,
                              int n_iter) {
  bool change = true;
  unsigned int i, j, n_edges;
  Solution *best_solution, *best_bb_solution, *solution;
  Vertice *aux, **sequence;
  
  best_bb_solution = branch_bound_vrp_solve(g, c, origin, n_iter);
  if (!best_bb_solution) return NULL;
  best_solution = best_bb_solution;
  n_edges = best_bb_solution->n_edges;
  sequence = calloc(n_edges + 1, sizeof(Vertice *));

  printf("\n\nEnd branch and bound, begin local search\n");

  for (i = 0; i < n_edges; i++) {
    sequence[i] = best_bb_solution->edges[n_edges-1 - i]->origin;
  }
  sequence[n_edges] = best_bb_solution->edges[0]->dest;

  while (change) {
    change = false;
    printf("ASDASDASD\n");
    for (i = 1; i < n_edges; i++) {
      for (j = i+1; j < n_edges; j++) {
        aux = sequence[i];
        sequence[i] = sequence[j];
        sequence[j] = aux;
        solution = malloc(sizeof(Solution));
        init_solution(solution, n_edges);
        if (build_solution_from_sequence(solution, sequence, g, c, origin)) {
          if (solution->cost < best_solution->cost) {
            destroy_solution(best_solution);
            best_solution = solution;
            print_solution(solution);
            change = true;
          }
          else goto UNDO;
        }
        else {
          UNDO: aux = sequence[i];
          sequence[i] = sequence[j];
          sequence[j] = aux;
          destroy_solution(solution);
        }
      }
    }
  }

  free(sequence);

  return best_solution;
}

int main(int argc, char const *argv[]) {

  if (argc != 3 && argc != 4) {
    printf("ERROR: Please specify both instance name and algorithm.");
    return 1;
  }

  unsigned int i, j, c;

  size_t bufsize = 32;
  char *buffer = malloc(bufsize*sizeof(char));
  FILE *file;
  file = fopen(argv[1], "r");
  getline(&buffer, &bufsize, file);

  bool algorithm = atoi(argv[2]);
  int n_iter = 0;
  if (algorithm) {
    n_iter = atoi(argv[3]);
  }

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
  quicksort_edges(g);

  IntLinkedList *vehicles = malloc(sizeof(IntLinkedList));
  for (i = 0; i < n_v; i++) {
    getline(&buffer, &bufsize, file);
    add_value(vehicles, atoi(buffer));
  }

  Solution *s;
  if (algorithm) {
    s = heuristic_vrp_solve(g, vehicles, vertices[0], n_iter);
  }
  else {
    s = branch_bound_vrp_solve(g, vehicles, vertices[0], n_iter);
  }

  print_solution(s);

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
