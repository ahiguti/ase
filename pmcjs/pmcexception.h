
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef PMCEXCEPTiON_HPP
#define PMCEXCEPTiON_HPP

#include "pmcjs_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

extern JSClass pmc_exception_class;
JSObject *pmc_init_pmc_exception_class(JSContext *cx, JSObject *obj);

#ifdef __cplusplus
}
#endif

#endif

