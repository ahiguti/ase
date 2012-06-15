
#include <objbase.h>
#include <oleauto.h>

#include "asecom_comvtbl.h"

void *
ase_com_com_vtable(IDispatch *p)
{
  return p->lpVtbl;
}

