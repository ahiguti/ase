
#ifndef GENTYPE_ASE_ALLOCA_HPP
#define GENTYPE_ASE_ALLOCA_HPP

#ifdef __GNUC__
#include <stdlib.h>
#define ASE_ALLOCA(ty, len, var) ty *var = (ty *)alloca(sizeof(ty) * (len))
#define ASE_FASTVECTOR(ty, len, var) ty var[len];
#define ASE_FASTVECTOR_GETPTR(var) var
#else
#include <malloc.h>
#include <vector>
#define ASE_ALLOCA(ty, len, var) ty *var = (ty *)_alloca(sizeof(ty) * (len))
#define ASE_FASTVECTOR(ty, len, var) std::vector<ty> var(len);
#define ASE_FASTVECTOR_GETPTR(var) var.empty() ? 0 : (&var[0])
#endif

#endif

