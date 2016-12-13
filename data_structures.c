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

void init_vertice(Vertice *v, unsigned int id, unsigned int demand) {
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

void quicksort_edge_array(Edge **array, unsigned int size) {
  Edge *pivot, *tmp;
  Edge **left, **right;
  if (size > 1) {
    // First element as pivot
    pivot = *array;
    // Left boundary is array's start
    left = array;
    // Right boundary is array's end
    right = array+size-1;
    // Repeat while there are points to look at
    while (left <= right) {
      // Advance left to element larger than or euqal to pivot
      while ((*left)->cost < pivot->cost) {
        left++;
      }
      // Retreat right to element smaller than or equal to pivot
      while ((*right)->cost > pivot->cost) {
        right--;
      }
      // If left is to the left of right, swap them and advance them
      if (left <= right) {
        tmp = *left;
        *left = *right;
        *right = tmp;
        left++;
        right--;
      }
    }
    // Apply quicksort to boundaries defined by left and right
    quicksort_edge_array(array, right-array+1);
    quicksort_edge_array(left, array+size-left);
  }
}

// ===========================================================================
//                           INT LINKED LIST ELEMENTS                         
// ===========================================================================

void init_element(Element *el, unsigned int val) {
  el->value = val;
  el->next = NULL;
}

void destroy_element(Element *el) {
  if (el) {
    free(el);
    el = NULL;
  }
}

// ===========================================================================
//                               INT LINKED LISTS                             
// ===========================================================================

void init_linkedlist(IntLinkedList *ll) {
  ll->head = ll->tail = NULL;
}

void destroy_linkedlist(IntLinkedList *ll) {
  if (ll) {
    while(ll->head) {
      remove_head(ll);
    }
    free(ll);
    ll = NULL;
  }
}

void add_value(IntLinkedList *ll, unsigned int val) {
  Element *el = malloc(sizeof(Element));
  init_element(el, val);
  if (!ll->head) {
    ll->head = ll->tail = el;
    return;
  }
  if (val < ll->head->value) {
    el->next = ll->head;
    ll->head = el;
    return;
  }
  if (val >= ll->tail->value) {
    ll->tail->next = el;
    ll->tail = el;
    return;
  }
  Element *it = ll->head;
  while (it->next && it->next->value < val) {
    it = it->next;
  }
  el->next = it->next;
  it->next = el;
}

bool remove_value(IntLinkedList *ll, unsigned int val) {
  Element *aux;
  if (!ll->head) return false;
  if (ll->head->value >= val) {
    remove_head(ll);
    return true;
  }
  Element *it = ll->head;
  while (it->next) {
    if (it->next->value >= val) {
      aux = it->next;
      if (it->next == ll->tail) {
        ll->tail = it;
      }
      it->next = it->next->next;
      destroy_element(aux);
      return true;
    }
    it = it->next;
  }
  return false;
}

void remove_head(IntLinkedList *ll) {
  if (ll->head) {
    Element *el = ll->head;
    ll->head = el->next;
    if (!ll->head) {
      ll->tail = NULL;
    }
    destroy_element(el);
  }
}

IntLinkedList *deep_copy(IntLinkedList *ll) {
  IntLinkedList *new_ll = malloc(sizeof(IntLinkedList));
  init_linkedlist(new_ll);
  Element *it = ll->head;
  while (it) {
    add_value(new_ll, it->value);
    it = it->next;
  }
  return new_ll;
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
      q_remove_head(q);
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

void q_remove_head(Queue *q) {
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

void quicksort_edges(Graph *g) {
  unsigned int i;
  for (i = 0; i < g->n; i++) {
    quicksort_edge_array(g->edges[i], g->n_edges[i]);
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
    q_remove_head(q);
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
    q_remove_head(q);
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

  (*mark)[origin->id] = true;
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

  smark[origin->id] = true;
  for (i = 0; i < level; i++) {
    if (v_ignore[i] && v_ignore[i]->id != e->dest->id) {
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

void print_solution(Solution *s) {
  unsigned int i;
  if (s) {
    printf("Solution cost: %f\n", s->cost);
    printf("Solution path: ");
    for (i = 0; i < s->n_edges; i++) {
      if (s->edges[i]) {
        printf("%d ", s->edges[i]->dest->id);
      }
    }
    printf("0\n\n");
  } else  {
    printf("No solution!\n");
  }
}

bool build_solution_from_sequence(Solution *s, Vertice **sequence, Graph *g,
                                  IntLinkedList *c, Vertice *origin) {
  bool ret = true;
  unsigned int i, j, id_src, id_dest, degree, demand, it_s = 0;
  Edge *edge, **out_edges;
  IntLinkedList *vehicles = deep_copy(c);

  demand = 0;
  for (i = 0; i < s->n_edges; i++) {
    id_src = sequence[i]->id;
    id_dest = sequence[i+1]->id;
    if (id_src == id_dest) continue;
    out_edges = edges_out(g, id_src);
    degree = degree_out(g, id_src);
    for (j = 0; j < degree; j++) {
      if (out_edges[j]->dest->id == id_dest) {
        edge = out_edges[j];
        break;
      }
    }
    s->edges[it_s] = edge;
    it_s++;
    s->cost += edge->cost;
    demand += edge->dest->demand;
    if (edge->dest == origin) {
      if (!remove_value(vehicles, demand)) {
        ret = false;
        break;
      }
      demand = 0;
    }
  }

  destroy_linkedlist(vehicles);

  return ret;
}

// ===========================================================================
//                                    TREES                                   
// ===========================================================================

void init_tree(Tree *t, Vertice *v, Edge *e, bool e_v, Tree *p,
               IntLinkedList *ll, Vertice *origin, Graph *g) {
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
  t->available_vehicles = ll;
  t->removed_vehicle = false;
  t->lower_bound = get_lower_bound(t, g, origin);
  t->upper_bound = get_upper_bound(t, g, origin);
}

void init_tree_from_parent(Tree *t, Tree *other, Vertice *v, Edge *e,
                          bool e_v, Vertice *origin, Graph *g) {
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
  t->available_vehicles = other->available_vehicles;
  t->removed_vehicle = false;
  t->lower_bound = get_lower_bound(t, g, origin);
  t->upper_bound = get_upper_bound(t, g, origin);
}

void destroy_tree(Tree *t) {
  if (t->left_child) {
    destroy_tree(t->left_child);
  }
  if (t->right_child) {
    destroy_tree(t->right_child);
  }
  if (t->removed_vehicle) {
    destroy_linkedlist(t->available_vehicles);
  }
  free(t);
  t = NULL;
}

double get_lower_bound(Tree *t, Graph *g, Vertice *origin) {
  unsigned int i, j, degree;
  double *out;
  Edge *edge, **out_edges, **e_ignore;

  out = calloc(g->n, sizeof(double));
  e_ignore = calloc(t->level, sizeof(Edge *));

  build_traversed_edges_in_out(t, origin, e_ignore, out);

  for (i = 0; i < g->n; i++) {
    if (out[i]) continue;
    out_edges = edges_out(g, i);
    degree = degree_out(g, i);
    for (j = 0; j < degree; j++) {
      edge = out_edges[j];
      if (in_edges_array(e_ignore, edge, t->level)) continue;
      out[i] = edge->cost;
      break;
    }
  }

  double result = 0;
  for (i = 0; i < g->n; i++) {
    result += out[i];
  }

  free(out);
  free(e_ignore);

  return result;
}

double get_upper_bound(Tree *t, Graph *g, Vertice *origin) {
  unsigned int i, j, degree;
  double *out;
  Edge *edge, **out_edges, **e_ignore;

  out = calloc(g->n, sizeof(double));
  e_ignore = calloc(t->level, sizeof(Edge *));

  build_traversed_edges_in_out(t, origin, e_ignore, out);

  for (i = 0; i < g->n; i++) {
    if (out[i]) continue;
    out_edges = edges_out(g, i);
    degree = degree_out(g, i);
    for (j = 0; j < degree; j++) {
      edge = out_edges[degree-1 - j];
      if (in_edges_array(e_ignore, edge, t->level)) continue;
      out[i] = edge->cost;
      break;
    }
  }

  double result = 0;
  for (i = 0; i < g->n; i++) {
    result += out[i];
  }

  free(out);
  free(e_ignore);

  return result;
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

void build_traversed_edges_in_out(Tree *t, Vertice *origin, Edge **e_ignore,
                                  double *out) {
  Tree *iterator = t;
  Vertice *src, *dest;
  Edge *e;
  unsigned int it_e = 0;
  while (iterator->level > 0) {
    e = iterator->current_e;
    e_ignore[it_e] = e;
    it_e++;

    if (iterator->edge_value) {
      src = e->origin;
      dest = e->dest;

      if (src == origin || !out[src->id]) {
        out[src->id] += e->cost;
      }
    }

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
