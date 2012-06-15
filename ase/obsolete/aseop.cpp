
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseop.hpp>
#include <ase/aseexcept.hpp>
#include <functional>

bool
ASE_OpBoolean(const ase_variant& x)
{
  return x.get_boolean();
}

ase_int ASE_OpInt(const ase_variant& x)
{
  return x.get_int();
}

ase_long
ASE_OpLong(const ase_variant& x)
{
  return x.get_long();
}

double
ASE_OpDouble(const ase_variant& x)
{
  return x.get_double();
}

ase_string
ASE_OpString(const ase_variant& x)
{
  return x.get_string();
}

namespace {

template <template <class> class Op> bool
ase_compare(const ase_variant& x, const ase_variant& y)
{
  ase_vtype xt = x.get_type();
  switch (xt) {
  case ase_vtype_void:
    return Op<bool>()(false, false);
  case ase_vtype_bool:
    return Op<bool>()(x.get_boolean(), y.get_boolean());
  case ase_vtype_int:
    return Op<ase_int>()(x.get_int(), y.get_int());
  case ase_vtype_long:
    return Op<ase_long>()(x.get_long(), y.get_long());
  case ase_vtype_double:
    return Op<double>()(x.get_double(), y.get_double());
  case ase_vtype_string:
    return Op<ase_string>()(x.get_string(), y.get_string());
  default:
    ;
  }
  ase_throw_type_mismatch("not compatible");
  return false;
}

}; // anonymous namespace

bool
ASE_OpLt(const ase_variant& x, const ase_variant& y)
{
  return ase_compare<std::less>(x, y);
}

bool
ASE_OpLe(const ase_variant& x, const ase_variant& y)
{
  return ase_compare<std::less_equal>(x, y);
}

bool
ASE_OpGt(const ase_variant& x, const ase_variant& y)
{
  return ase_compare<std::greater>(x, y);
}

bool
ASE_OpGe(const ase_variant& x, const ase_variant& y)
{
  return ase_compare<std::greater_equal>(x, y);
}

namespace {

template <template <class> class Op> ase_variant
ase_arith2(const ase_variant& x, const ase_variant& y)
{
  ase_vtype xt = x.get_type();
  switch (xt) {
  case ase_vtype_bool:
  case ase_vtype_int:
  case ase_vtype_long:
    return Op<ase_long>()(x.get_long(), y.get_long());
  case ase_vtype_double:
    return Op<double>()(x.get_double(), y.get_double());
  default:
    ;
  }
  ase_throw_type_mismatch("not compatible");
  return ase_variant();
}

template <template <class> class Op> ase_variant
ase_arith2i(const ase_variant& x, const ase_variant& y) {
  ase_vtype xt = x.get_type();
  switch (xt) {
  case ase_vtype_bool:
  case ase_vtype_int:
  case ase_vtype_long:
  case ase_vtype_double:
    return Op<ase_long>()(x.get_long(), y.get_long());
  default:
    ;
  }
  ase_throw_type_mismatch("not compatible");
  return ase_variant();
}

template <template <class> class Op> ase_variant
ase_arith1(const ase_variant& x) {
  ase_vtype xt = x.get_type();
  switch (xt) {
  case ase_vtype_bool:
  case ase_vtype_int:
  case ase_vtype_long:
    return Op<ase_long>()(x.get_long());
  case ase_vtype_double:
    return Op<double>()(x.get_double());
  default:
    ;
  }
  ase_throw_type_mismatch("not compatible");
  return ase_variant();
}

template <template <class> class Op> ase_variant
ase_logic2(const ase_variant& x, const ase_variant& y) {
  ase_vtype xt = x.get_type();
  switch (xt) {
  case ase_vtype_bool:
    return Op<bool>()(x.get_boolean(), y.get_boolean());
  case ase_vtype_int:
    return Op<ase_int>()(x.get_int(), y.get_int());
  case ase_vtype_long:
  case ase_vtype_double:
    return Op<ase_long>()(x.get_long(), y.get_long());
  default:
    ;
  }
  ase_throw_type_mismatch("not compatible");
  return ase_variant();
}

}; // anonymous namespace

ase_variant
ASE_OpAdd(const ase_variant& x, const ase_variant& y)
{
  return ase_arith2<std::plus>(x, y);
}

ase_variant
ASE_OpSub(const ase_variant& x, const ase_variant& y)
{
  return ase_arith2<std::minus>(x, y);
}

ase_variant
ASE_OpMul(const ase_variant& x, const ase_variant& y)
{
  return ase_arith2<std::multiplies>(x, y);
}

ase_variant
ASE_OpDiv(const ase_variant& x, const ase_variant& y)
{
  return ase_arith2<std::divides>(x, y);
}

ase_variant
ASE_OpMod(const ase_variant& x, const ase_variant& y)
{
  return ase_arith2i<std::modulus>(x, y);
}

ase_variant
ASE_OpNeg(const ase_variant& x)
{
  return ase_arith1<std::negate>(x);
}

namespace {

template <typename T> struct ase_increment {
  T operator ()(const T& x) {
    return x + 1;
  }
};

template <typename T> struct ase_decrement {
  T operator ()(const T& x) {
    return x - 1;
  }
};

}; // anonymous namespace

ase_variant
ASE_OpInc(const ase_variant& x)
{
  return ase_arith1<ase_increment>(x);
}

ase_variant
ASE_OpDec(const ase_variant& x)
{
  return ase_arith1<ase_decrement>(x);
}

ase_variant
ASE_OpAnd(const ase_variant& x, const ase_variant& y)
{
  return ase_logic2<std::logical_and>(x, y);
}

ase_variant
ASE_OpOr(const ase_variant& x, const ase_variant& y)
{
  return ase_logic2<std::logical_or>(x, y);
}

namespace {

template <typename T> struct ase_logical_xor {
  T operator ()(const T& x, const T& y) {
    return x ^ y;
  }
};

}; // anonymous namespace

ase_variant
ASE_OpXor(const ase_variant& x, const ase_variant& y)
{
  return ase_logic2<ase_logical_xor>(x, y);
}

ase_variant
ASE_OpNot(const ase_variant& x)
{
  return !x.get_boolean();
}

