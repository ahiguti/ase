
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>

#include "asedummy_util.hpp"
#include "asedummy_aseimpl.hpp"
#include "asedummy_asepr.hpp"

#define DBG_TOASE(x)
#define DBG_DUMMYPRX(x)
#define DBG_EXC(x)

namespace asedummy {

ase_string
ase_dummy_util::to_ase_string(ase_dummy_context& ctx,
  const ase_dummy_string& str)
{
  return ase_string();
}

ase_dummy_string
ase_dummy_util::to_dummy_string(ase_dummy_context& ctx, const char *str,
  ase_size_type slen)
{
  return ase_dummy_string();
}

ase_dummy_string
ase_dummy_util::to_dummy_string(ase_dummy_context& ctx, const ase_string& str)
{
  return ase_dummy_string();
}

ase_variant
ase_dummy_util::to_ase_variant(ase_dummy_context& ctx,
  const ase_dummy_value& val)
{
  {
  }
  return create_dummy_proxy(ctx, val);
}

ase_dummy_value
ase_dummy_util::to_dummy_value(ase_dummy_context& ctx, const ase_variant& val)
{
  {
  }
  return create_aseproxy(ctx, val);
}

ase_variant
ase_dummy_util::create_dummy_proxy(ase_dummy_context& ctx,
  const ase_dummy_value& value)
{
  return ase_variant();
}

ase_dummy_value
ase_dummy_util::create_aseproxy(ase_dummy_context& ctx,
  const ase_variant& value)
{
  return ase_dummy_value();
}

void
ase_dummy_util::handle_ase_exception(ase_dummy_context& ctx)
{
  ase_variant ex = ase_variant::create_from_active_exception();
}

}; // namespace asedummy

