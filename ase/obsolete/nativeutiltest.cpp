
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>

namespace {

struct utiltestclass {

  utiltestclass(const ase_string& n, ASE_Component::Library& lib) {
  }
  ase_variant sum(const ase_variant *args, ase_size_type nargs) {
    int r = 0;
    for (ase_size_type i = 0; i < nargs; ++i) {
      r += args[i].get_int();
    }
    return ase_variant(r);
  }
  ase_variant is_utiltest_class(const ase_variant *args,
    ase_size_type nargs);
  ase_variant is_utiltest_instance(const ase_variant *args,
    ase_size_type nargs);
  ase_variant callback_test(const ase_variant *args,
    ase_size_type nargs);

  static const ASE_Component::MethodEntry<utiltestclass> ClassMethods[];

};

struct utiltest {
  utiltest(const utiltestclass& klass, const ase_variant *args,
    ase_size_type nargs) : value(0) {
  }
  ase_variant SetValue(const ase_variant *args, ase_size_type nargs) {
    if (nargs > 0) {
      value = args[0].get_int();
    }
    return ase_variant();
  }
  ase_variant GetValue(const ase_variant *args, ase_size_type nargs) {
    return value;
  }
  ase_variant Add(const ase_variant *args, ase_size_type nargs) {
    if (nargs > 0) {
      value += args[0].get_int();
    }
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<utiltest> InstanceMethods[];

  int value;

};

ase_variant
utiltestclass::is_utiltest_class(const ase_variant *args,
  ase_size_type nargs)
{
  if (nargs < 1) {
    return false;
  }
  if (ASE_Component::GetClassPointer<utiltestclass, utiltest>(args[0])) {
    return true;
  }
  return false;
}

ase_variant
utiltestclass::is_utiltest_instance(const ase_variant *args,
  ase_size_type nargs)
{
  if (nargs < 1) {
    return false;
  }
  if (ASE_Component::GetInstancePointer<utiltestclass, utiltest>(args[0])) {
    return true;
  }
  return false;
}

ase_variant
utiltestclass::callback_test(const ase_variant *args,
  ase_size_type nargs)
{
  if (nargs < 1) {
    ase_throw_missing_arg("callback_test: missing callback object");
  }
  const ase_variant& cb = args[0];
  return cb.invoke_self(args + 1, nargs - 1);
}

const ASE_Component::MethodEntry<utiltestclass>
utiltestclass::ClassMethods[] = {
  { "Sum", &utiltestclass::sum },
  { "IsUtilTestClass", &utiltestclass::is_utiltest_class },
  { "IsUtilTestInstance", &utiltestclass::is_utiltest_instance },
  { "CallbackTest", &utiltestclass::callback_test },
};

const ASE_Component::MethodEntry<utiltest>
utiltest::InstanceMethods[] = {
  { "SetValue", &utiltest::SetValue },
  { "GetValue", &utiltest::GetValue },
  { "Add", &utiltest::Add },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilTestClass(const ase_string& name, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<utiltestclass, utiltest, false>(name, lib);
}

