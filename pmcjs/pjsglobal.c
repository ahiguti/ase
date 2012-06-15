
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "pmcjs_impl.h"

#define DBG_ROOT(x)
#define DBG_INVOKE(x)
#define DBG_MEM(x)

struct pmc_jsglobal_s {
  PMCPerl *ppl;
  JSObject *globj;
  SV *sv;
};

pmc_jsglobal *
pmc_jsglobal_new(pTHX_ PMCPerl *ppl, JSContext *cx, JSObject *globj)
{
  pmc_jsglobal *pjg = NULL;

  pjg = (pmc_jsglobal *)pmc_malloc(cx, sizeof(*pjg), "pmc_jsglobal_new");
  if (!pjg) {
    return NULL;
  }
  pjg->ppl = ppl;
  pjg->globj = globj;
  if (!JS_AddNamedRoot(cx, &pjg->globj, "pmc_jsglobal_new")) {
    goto out;
  }
  pjg->sv = newSViv((IV)pjg);
  return pjg;

 out:

  if (pjg) {
    pmc_free(pjg, "pmc_jsglobal_new");
  }
  return NULL;

}

void
pmc_jsglobal_destroy(pTHX_ pmc_jsglobal *pjg)
{
  JSContext *cx = NULL;
  cx = pmc_get_current_jsctx(pjg->ppl);
  JS_RemoveRoot(cx, &pjg->globj);
  SvREFCNT_dec(pjg->sv);
  pmc_free(pjg, "pmc_jsglobal_destroy");
}

SV *
pmc_jsglobal_getsv(pmc_jsglobal *pjg)
{
  return pjg->sv;
}

pmc_jsglobal *
pmc_jsglobal_fromsv(pTHX_ SV *sv)
{
  return (pmc_jsglobal *)SvIVx(sv);
}

PMCPerl *
pmc_jsglobal_getpmcperl(pmc_jsglobal *pjg)
{
  return pjg->ppl;
}

JSContext *
pmc_jsglobal_getcontext(pmc_jsglobal *pjg)
{
  return pmc_get_current_jsctx(pjg->ppl);
}

JSObject *
pmc_jsglobal_getobject(pmc_jsglobal *pjg)
{
  return pjg->globj;
}

