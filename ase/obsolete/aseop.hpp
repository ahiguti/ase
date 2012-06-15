
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpp.hpp>
#include <ase/aseexcept.hpp>

bool ASE_OpBoolean(const ase_variant& x);
ase_int ASE_OpInt(const ase_variant& x);
ase_long ASE_OpLong(const ase_variant& x);
double ASE_OpDouble(const ase_variant& x);
ase_string ASE_OpString(const ase_variant& x);

bool ASE_OpLt(const ase_variant& x, const ase_variant& y);
bool ASE_OpLe(const ase_variant& x, const ase_variant& y);
bool ASE_OpGt(const ase_variant& x, const ase_variant& y);
bool ASE_OpGe(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpAdd(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpSub(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpMul(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpDiv(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpMod(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpNeg(const ase_variant& x);
ase_variant ASE_OpInc(const ase_variant& x);
ase_variant ASE_OpDec(const ase_variant& x);
ase_variant ASE_OpAnd(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpOr(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpXor(const ase_variant& x, const ase_variant& y);
ase_variant ASE_OpNot(const ase_variant& x);

