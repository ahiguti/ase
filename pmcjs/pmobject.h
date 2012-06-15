
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCOBJECT_HPP
#define PMCOBJECT_HPP

#include "pmcjs_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern JSClass pmc_perl_class;

JSObject *pmc_init_perl_class(pTHX_ PMCPerl *ppl, JSContext *cx,
  JSObject *obj);

#ifdef __cplusplus
}
#endif

#endif

