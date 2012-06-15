#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "ppport.h"

typedef struct xstest_rep_s {
  int value;
} xstest_rep;

static xstest_rep *
xstest_rep_new()
{
  xstest_rep *rep;
  rep = (xstest_rep *)malloc(sizeof(*rep));
  if (!rep) {
    return NULL;
  }
  rep->value = 0;
  return rep;
}

void
xstest_rep_destroy(xstest_rep *rep)
{
  free(rep);
}

void
xstest_rep_set_value(xstest_rep *rep, int v)
{
  rep->value = v;
}

int
xstest_rep_get_value(xstest_rep *rep)
{
  return rep->value;
}

MODULE = XSTest		PACKAGE = XSTest		

SV *
new(klass)
  char *klass
CODE:
  xstest_rep *rep;
  rep = xstest_rep_new();
  if (!rep) {
    RETVAL = newSV(0);
  } else {
    SV *obj, *objref;
    objref = newSViv(0);
    obj = newSVrv(objref, klass);
    sv_setiv(obj, (IV)rep);
    SvREADONLY_on(obj);
    RETVAL = objref;
  }
OUTPUT:
  RETVAL

void
DESTROY(obj)
  SV *obj;
CODE:
  xstest_rep *rep;
  rep = (xstest_rep *)SvIVx(SvRV(obj));
  xstest_rep_destroy(rep);

int
getValue(obj)
  SV *obj;
CODE:
  xstest_rep *rep;
  rep = (xstest_rep *)SvIVx(SvRV(obj));
  RETVAL = xstest_rep_get_value(rep);
OUTPUT:
  RETVAL

void
setValue(obj, v)
  SV *obj
  int v
CODE:
  xstest_rep *rep;
  rep = (xstest_rep *)SvIVx(SvRV(obj));
  xstest_rep_set_value(rep, v);

