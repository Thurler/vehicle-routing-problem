#ifndef DATA_H
#define DATA_H

#include <stdbool.h>

typedef struct Vertice {
  unsigned int id;
  double demand;
} Vertice;

void init_vertice(Vertice *v, unsigned int id, double demand);
void destroy_vertice(Vertice *v);
bool in_vertices_array(Vertice **arr, Vertice *v, unsigned int size);

typedef struct Edge {
  Vertice *origin;
  Vertice *dest;
  double cost;
} Edge;

void init_edge(Edge *e, Vertice *origin, Vertice *destination, double cost);
void destroy_edge(Edge *e);
bool in_edges_array(Edge **arr, Edge *e, unsigned int size);

typedef struct QElement {
  Vertice *value;
  struct QElement *next;
} QElement;

void init_qelement(QElement *qel, Vertice *val);
void destroy_qelement(QElement *qel);

typedef struct Queue {
  bool empty;
  QElement *head;
  QElement *tail;
} Queue;

void init_queue(Queue *q);
void destroy_queue(Queue *q);
void add_tail(Queue *q, Vertice *val);
void remove_head(Queue *q);
Vertice *get_head(Queue *q);

typedef struct Solution {
  double cost;
  unsigned int n_edges;
  Edge **edges;
} Solution;

void init_solution(Solution *s, unsigned int n_edges);
void destroy_solution(Solution *s);

typedef struct Graph {
  unsigned int n;
  unsigned int *n_edges;
  Vertice **v;
  Edge ***edges;
} Graph;

void init_graph(Graph *g, unsigned int n_vertices, Vertice **v);
void init_graph_edges(Graph *g, unsigned int vertice, unsigned int n_edges);
void destroy_graph(Graph *g);
unsigned int degree_out(Graph *g, unsigned int vertice);
Edge **edges_out(Graph *g, unsigned int vertice);
void bfs(Graph *g, Edge **e_ignore, unsigned int size, Edge *e,
         Vertice *origin, bool **mark);
void path(Graph *g, Edge **e_ignore, Vertice **v_ignore, unsigned int size,
          Edge *e, Vertice *origin, Vertice *dest, bool **mark);
bool strongly_connected(Graph *g, Edge **e_ignore, Vertice **v_ignore,
                        unsigned int level, Edge *e, Vertice *origin);

typedef struct Tree {
  Vertice *current_v;
  Edge *current_e;
  bool edge_value;
  struct Tree *parent;
  struct Tree *left_child;
  struct Tree *right_child;
  unsigned int level;
  unsigned int edges_count;
  unsigned int n_vertices_traversed;
  double cost_so_far;
  double path_demand_so_far;
  double *available_vehicles;
} Tree;

void init_tree(Tree *t, Vertice *v, Edge *e, bool e_v, Tree *p);
void init_tree_from_parent(Tree *t, Tree *other, Vertice *v, Edge *e,
                          bool e_v, Vertice *origin);
void destroy_tree(Tree *t);
void add_child_to_parent(Tree *parent, Tree *child);
void build_discarded_edges(Tree *t, Edge **discard_e);
void build_traversed_vertices_edges(Tree *t, Vertice *origin,
                                    Vertice **ignore_v, Edge **ignore_e);
void build_solution(Tree *t, Solution *s);
void next_leaf(Tree **current);

#endif
