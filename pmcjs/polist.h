
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_POLIST_HPP
#define PMCJS_POLIST_HPP

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pmc_list_s pmc_list;
typedef struct pmc_listnode_s pmc_listnode;

pmc_list *pmc_list_new(const char *debugid);
void pmc_list_destroy(pmc_list *lst, const char *debugid);
size_t pmc_list_length(pmc_list *lst); /* NOTE: O(n) */
pmc_listnode *pmc_list_insert_before(pmc_list *lst, pmc_listnode *ndnext,
  void *value);
pmc_listnode *pmc_list_insert_after(pmc_list *lst, pmc_listnode *ndprev,
  void *value);
void pmc_list_erase(pmc_listnode *node);
void pmc_list_move_last(pmc_list *lst, pmc_listnode *nd);
  /* nd can be in another list */
pmc_listnode *pmc_list_first(pmc_list *lst);
pmc_listnode *pmc_list_last(pmc_list *lst);
void *pmc_listnode_get_value(pmc_listnode *node);
void pmc_listnode_set_value(pmc_listnode *node, void *value);
pmc_listnode *pmc_list_next(pmc_list *lst, pmc_listnode *node);
pmc_listnode *pmc_list_prev(pmc_list *lst, pmc_listnode *node);

#ifdef __cplusplus
}
#endif

#endif

