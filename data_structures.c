#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "data_structures.h"

// ===========================================================================
//                              STATIC FUNCTIONS                              
// ===========================================================================

static bool array_only_has_true(bool *arr, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) {
    if (!arr[i]) return false;
  }
  return true;
}

// ===========================================================================
//                                  VERTICES                                 
// ===========================================================================

void init_vertice(Vertice *v, unsigned int id, double demand) {
  v->id = id;
  v->demand = demand;
}

void destroy_vertice(Vertice *v) {
  if (v) {
    free(v);
    v = NULL;
  }
}

bool in_vertices_array(Vertice **arr, Vertice *v, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) {
    if (arr[i] == v) {
      return true;
    }
  }
  return false;
}

// ===========================================================================
//                                    EDGES                                   
// ===========================================================================

void init_edge(Edge *e, Vertice *origin, Vertice *destination, double cost) {
  e->origin = origin;
  e->dest = destination;
  e->cost = cost;
}

void destroy_edge(Edge *e) {
  if (e) {
    free(e);
    e = NULL;
  }
}

bool in_edges_array(Edge **arr, Edge *e, unsigned int size) {
  unsigned int i;
  for (i = 0; i < size; i++) {
    if (arr[i] == e) {
      return true;
    }
  }
  return false;
}

// ===========================================================================
//                                QUEUE ELEMENTS                              
// ===========================================================================

void init_qelement(QElement *qel, Vertice *val) {
  qel->value = val;
  qel->next = NULL;
}

void destroy_qelement(QElement *qel) {
  if (qel) {
    free(qel);
    qel = NULL;
  }
}

// ===========================================================================
//                                    QUEUES                                  
// ===========================================================================

void init_queue(Queue *q) {
  q->head = q->tail = NULL;
  q->empty = true;
}

void destroy_queue(Queue *q) {
  if (q) {
    while (q->head) {
      remove_head(q);
    }
    free(q);
    q = NULL;
  }
}

void add_tail(Queue *q, Vertice *val) {
  QElement *qel = malloc(sizeof(QElement));
  init_qelement(qel, val);
  if (q->tail) {
    q->tail->next = qel;
    q->tail = qel;
  }
  else {
    q->head = q->tail = qel;
    q->empty = false;
  }
}

void remove_head(Queue *q) {
  if (q->head) {
    QElement *qel = q->head;
    q->head = qel->next;
    destroy_qelement(qel);
    if (!q->head) {
      q->tail = NULL;
      q->empty = true;
    }
  }
}

Vertice *get_head(Queue *q) {
  return q->head->value;
}

// ===========================================================================
//                                  SOLUTIONS                                 
// ===========================================================================

void init_solution(Solution *s, unsigned int n_edges) {
  s->cost = 0;
  s->n_edges = n_edges;
  s->edges = calloc(n_edges, sizeof(Edge *));
}

void destroy_solution(Solution *s) {
  if (s) {
    if (s->edges) {
      free(s->edges);
    }
    free(s);
    s = NULL;
  }
}

// ===========================================================================
//                                    GRAPHS                                  
// ===========================================================================

void init_graph(Graph *g, unsigned int n_vertices, Vertice **v) {
  g->n = n_vertices;
  g->v = v;
  g->n_edges = calloc(n_vertices, sizeof(unsigned int));
  g->edges = calloc(n_vertices, sizeof(Edge **));
}

void init_graph_edges(Graph *g, unsigned int vertice, unsigned int n_edges) {
  g->n_edges[vertice] = n_edges;
  g->edges[vertice] = calloc(n_edges, sizeof(Edge *));
}

void destroy_graph(Graph *g) {
  if (g) {
    if (g->edges) {
      int i, n = g->n;
      for (i = 0; i < n; i++) {
        if (g->edges[i]) {
          free(g->edges[i]);
        }
      }
      free(g->edges);
    }
    if (g->n_edges) {
      free(g->n_edges);
    }
    free(g);
    g = NULL;
  }
}

unsigned int degree_out(Graph *g, unsigned int vertice) {
  return g->n_edges[vertice];
}

Edge **edges_out(Graph *g, unsigned int vertice) {
  return g->edges[vertice];
}

void bfs(Graph *g, Edge **e_ignore, unsigned int size, Edge *e,
         Vertice *origin, bool **mark) {
  unsigned int degree, i;
  Vertice *current, *next;
  Edge *edge, **edges;

  Queue *q = malloc(sizeof(Queue));
  init_queue(q);
  add_tail(q, origin);

  while (!q->empty) {
    current = get_head(q);
    remove_head(q);
    edges = edges_out(g, current->id);
    degree = degree_out(g, current->id);
    for (i = 0; i < degree; i++) {
      edge = edges[i];
      next = edge->dest;
      if (!(*mark)[next->id]) {
        if (in_edges_array(e_ignore, edge, size) || edge == e) continue;
        (*mark)[next->id] = true;
        add_tail(q, next);
      }
    }
  }

  destroy_queue(q);
}

void path(Graph *g, Edge **e_ignore, Vertice **v_ignore, unsigned int size,
          Edge *e, Vertice *origin, Vertice *dest, bool **mark) {
  unsigned int degree, i;
  Vertice *current, *next, *mock, **paths;
  Edge *edge, **edges;
  Queue *q;

  mock = malloc(sizeof(Vertice));
  paths = calloc(g->n, sizeof(Vertice *));
  for (i = 0; i < size; i++) {
    if (v_ignore[i]) {
      paths[v_ignore[i]->id] = mock;
    }
  }

  q = malloc(sizeof(Queue));
  init_queue(q);
  add_tail(q, origin);

  while (!q->empty) {
    current = get_head(q);
    remove_head(q);
    edges = edges_out(g, current->id);
    degree = degree_out(g, current->id);
    for (i = 0; i < degree; i++) {
      edge = edges[i];
      next = edge->dest;
      if (!paths[next->id]) {
        if (in_edges_array(e_ignore, edge, size) || edge == e) continue;
        paths[next->id] = current;
        if (next == dest) goto OUT;
        add_tail(q, next);
      }
    }
  }

  OUT: destroy_queue(q);

  for (current = dest; current != origin; current = paths[current->id]) {
    (*mark)[current->id] = true;
  }

  free(paths);
  destroy_vertice(mock);
}

bool strongly_connected(Graph *g, Edge **e_ignore, Vertice **v_ignore,
                        unsigned int level, Edge *e, Vertice *origin) {
  unsigned int i;
  bool *mark, *smark, ret = true;

  mark = calloc(g->n, sizeof(bool));
  smark = calloc(g->n, sizeof(bool));

  mark[origin->id] = true;
  smark[origin->id] = true;
  for (i = 0; i < level; i++) {
    if (v_ignore[i]) {
      mark[v_ignore[i]->id] = true;
      smark[v_ignore[i]->id] = true;
    }
  }

  for (i = 0; i < g->n; i++) {
    if (i == origin->id) {
      bfs(g, e_ignore, level, e, origin, &smark);
      if (!array_only_has_true(smark, g->n)) {
        ret = false;
        break;
      }
    }

    else if (!in_vertices_array(v_ignore, g->v[i], level)) {
      path(g, e_ignore, v_ignore, level, e, g->v[i], origin, &mark);
    }
  }

  if (ret) {
    ret = array_only_has_true(mark, g->n);
  }

  free(mark);
  free(smark);

  return ret;
}

// ===========================================================================
//                                    TREES                                   
// ===========================================================================

void init_tree(Tree *t, Vertice *v, Edge *e, bool e_v, Tree *p) {
  t->current_v = v;
  t->current_e = e;
  t->edge_value = e_v;
  t->parent = p;
  t->left_child = t->right_child = NULL;
  t->level = 0;
  t->edges_count = 0;
  t->n_vertices_traversed = 1;
  t->cost_so_far = 0;
  t->path_demand_so_far = 0;
}

void init_tree_from_parent(Tree *t, Tree *other, Vertice *v, Edge *e,
                          bool e_v, Vertice *origin) {
  t->current_v = v;
  t->current_e = e;
  t->edge_value = e_v;
  t->parent = other;
  t->left_child = t->right_child = NULL;
  t->level = other->level + 1;
  t->edges_count = other->edges_count + e_v;
  t->n_vertices_traversed = other->n_vertices_traversed + (e_v*(v != origin));
  t->cost_so_far = other->cost_so_far + (e_v*(e->cost));
  t->path_demand_so_far = other->path_demand_so_far + (e_v*(v->demand));
}

void destroy_tree(Tree *t) {
  if (t->left_child) {
    destroy_tree(t->left_child);
  }
  if (t->right_child) {
    destroy_tree(t->right_child);
  }
  free(t);
  t = NULL;
}

void add_child_to_parent(Tree *parent, Tree *child) {
  if (!parent->left_child) {
    parent->left_child = child;
  }
  else {
    parent->right_child = child;
  }
}

void build_traversed_vertices_edges(Tree *t, Vertice *origin,
                                    Vertice **ignore_v, Edge **ignore_e) {
  Tree *iterator = t;
  Vertice *v;
  Edge *e;
  unsigned int it_v = 0, it_e = 0;
  while (iterator->level > 0) {
    v = iterator->current_v;
    e = iterator->current_e;
    if (v != origin) {
      ignore_v[it_v] = v;
      it_v++;
    }
    ignore_e[it_e] = e;
    it_e++;
    iterator = iterator->parent;
  }
}

void build_solution(Tree *t, Solution *s) {
  Tree *iterator = t;
  init_solution(s, t->edges_count);
  unsigned int it_s = 0;
  while (iterator->level > 0) {
    if (iterator->edge_value) {
      s->cost += iterator->current_e->cost;
      s->edges[it_s] = iterator->current_e;
      it_s++;
    }
    iterator = iterator->parent;
  }
}

void next_leaf(Tree **current) {
  Tree *aux, *copyaux;
  aux = *current;
  while (aux && !aux->left_child && !aux->right_child) {
    if (aux->parent) {
      if (aux->parent->left_child == aux) {
        aux->parent->left_child = NULL;
      }
      else {
        aux->parent->right_child = NULL;
      }
    }
    copyaux = aux;
    aux = aux->parent;
    destroy_tree(copyaux);
  }
  if (aux) {
    if (aux->left_child) {
      *current = aux->left_child;
    }
    else if (aux->right_child) {
      *current = aux->right_child;
    }
  }
  else {
    *current = aux;
  }
}
