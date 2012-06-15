
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCJS_PJSOBJ_ITER_HPP
#define PMCJS_PJSOBJ_ITER_HPP

#include <jsapi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pmc_jsobjiter_s pmc_jsobjiter;

pmc_jsobjiter *pmc_jsobjiter_new(JSContext *cx, JSObject *obj);
  /* return NULL if alloc failed */
void pmc_jsobjiter_destroy(pmc_jsobjiter *iter);
const char *pmc_jsobjiter_next(pmc_jsobjiter *iter);

#ifdef __cplusplus
}
#endif

#endif

