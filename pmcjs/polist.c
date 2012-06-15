
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <stdlib.h>
#include <string.h>
#include "poutil.h"
#include "polist.h"

#define DBG_MEM(x)

struct pmc_listnode_s {
  pmc_listnode *prev;
  pmc_listnode *next;
  void *value;
};

struct pmc_list_s {
  pmc_listnode endpoint;
  const char *debugid;
};

pmc_list *
pmc_list_new(const char *debugid)
{
  pmc_list *lst;
  lst = (pmc_list *)pmc_malloc_nocx(sizeof(*lst), debugid);
  if (!lst) {
    return NULL;
  }
  lst->endpoint.prev = &lst->endpoint;
  lst->endpoint.next = &lst->endpoint;
  lst->debugid = debugid;
  return lst;
}

pmc_listnode *
pmc_list_next(pmc_list *lst, pmc_listnode *node)
{
  return node->next == (&lst->endpoint) ? NULL : node->next;
}

pmc_listnode *
pmc_list_prev(pmc_list *lst, pmc_listnode *node)
{
  return node->prev == (&lst->endpoint) ? NULL : node->prev;
}

pmc_listnode *
pmc_list_first(pmc_list *lst)
{
  return pmc_list_next(lst, &lst->endpoint);
}

pmc_listnode *
pmc_list_last(pmc_list *lst)
{
  return pmc_list_prev(lst, &lst->endpoint);
}

void
pmc_list_destroy(pmc_list *lst, const char *debugid)
{
  pmc_listnode *i;
  for (i = pmc_list_first(lst); i; ) {
    pmc_listnode *nxt;
    nxt = pmc_list_next(lst, i);
    DBG_MEM(fprintf(stderr, "__fplnd %p\n", i));
    pmc_free(i, debugid);
    i = nxt;
  }
  pmc_free(lst, debugid);
}

size_t
pmc_list_length(pmc_list *lst)
{
  pmc_listnode *i;
  size_t c = 0;
  for (i = pmc_list_first(lst); i; i = pmc_list_next(lst, i)) {
    ++c;
  }
  return c;
}

static pmc_listnode *
pmc_list_insert(pmc_list *lst, pmc_listnode *ndprev, pmc_listnode *ndnext,
  void *value)
{
  pmc_listnode *nd;
  nd = (pmc_listnode *)pmc_malloc_nocx(sizeof(*nd), lst->debugid);
  DBG_MEM(fprintf(stderr, "__mplnd %p\n", nd));
  if (!nd) {
    return NULL;
  }
  nd->value = value;
  nd->prev = ndprev;
  nd->next = ndnext;
  ndprev->next = nd;
  ndnext->prev = nd;
  return nd;
}

pmc_listnode *
pmc_list_insert_before(pmc_list *lst, pmc_listnode *ndnext, void *value)
{
  if (!ndnext) {
    ndnext = &lst->endpoint;
  }
  return pmc_list_insert(lst, ndnext->prev, ndnext, value);
}

pmc_listnode *
pmc_list_insert_after(pmc_list *lst, pmc_listnode *ndprev, void *value)
{
  if (!ndprev) {
    ndprev = &lst->endpoint;
  }
  return pmc_list_insert(lst, ndprev, ndprev->next, value);
}

static void
pmc_list_unlink(pmc_listnode *node)
{
  pmc_listnode *nprev, *nnext;
  nprev = node->prev;
  nnext = node->next;
  nprev->next = nnext;
  nnext->prev = nprev;
}

void
pmc_list_erase(pmc_listnode *node)
{
  pmc_list_unlink(node);
  DBG_MEM(fprintf(stderr, "__fplnd %p\n", node));
  pmc_free(node, "pmc_list_erase");
}

void
pmc_list_move_last(pmc_list *lst, pmc_listnode *node)
{
  pmc_list_unlink(node);
  {
    pmc_listnode *ndprev, *ndnext;
    ndnext = &lst->endpoint;
    ndprev = ndnext->prev;
    node->prev = ndprev;
    node->next = ndnext;
    ndprev->next = node;
    ndnext->prev = node;
  }
}

void *
pmc_listnode_get_value(pmc_listnode *node)
{
  return node->value;
}

void
pmc_listnode_set_value(pmc_listnode *node, void *value)
{
  node->value = value;
}

