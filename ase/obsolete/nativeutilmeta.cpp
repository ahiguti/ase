
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>

#include <stdexcept>

namespace {

struct metamethods {

  struct metaentry_type {
    ase_variant (metamethods::*memfuncptr)(const ase_variant *args,
      ase_size_type nargs);
    ase_size_type minargs;
    const char *usage;
  };

  metamethods(const ase_string& n, ASE_Component::Library& lib)
    : libref(lib) { }
  metamethods(const metamethods& klass, const ase_variant *args,
    ase_size_type nargs) : libref(klass.libref) { }
  void checkArgs(ase_size_type minargs, ase_size_type nargs, const char *usage) {
    if (nargs < minargs) {
      ase_string s = "Usage: " + ase_string(usage);
      ase_throw_missing_arg(s.data());
    }
  }

  /* variant metamethods */
  ase_variant metaGetBoolean(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_boolean(object)");
    return ase_variant(args[0].get_boolean());
  }
  ase_variant metaGetInt(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_int(object)");
    return ase_variant(args[0].get_int());
  }
  ase_variant metaGetLong(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_long(object)");
    return ase_variant(args[0].get_long());
  }
  ase_variant metaGetDouble(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_double(object)");
    return ase_variant(args[0].get_double());
  }
  ase_variant metaGetString(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_string(object)");
    return args[0].get_string().to_variant();
  }
  ase_variant metaGetProperty(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "get_property(object, name)");
    return args[0].get_property(args[1].get_string());
  }
  ase_variant metaSetProperty(const ase_variant *args, ase_size_type nargs) {
    checkArgs(3, nargs, "set_property(object, name, value)");
    args[0].set_property(args[1].get_string(), args[2]);
    return ase_variant();
  }
  ase_variant metaDelProperty(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "del_property(object, name)");
    args[0].del_property(args[1].get_string());
    return ase_variant();
  }
  ase_variant metaGetElement(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "get_element(object, index)");
    return args[0].get_element(args[1].get_int());
  }
  ase_variant metaSetElement(const ase_variant *args, ase_size_type nargs) {
    checkArgs(3, nargs, "set_element(object, index, value)");
    args[0].set_element(args[1].get_int(), args[2]);
    return ase_variant();
  }
  ase_variant metaGetLength(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_length(object)");
    return ase_variant(args[0].get_length());
  }
  ase_variant metaSetLength(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "set_length(object, len)");
    args[0].set_length(args[1].get_int());
    return ase_variant();
  }
  ase_variant metaInvokeByName(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "invoke_by_name(object, name, args...)");
    return args[0].invoke_by_name(args[1].get_string(), args + 2, nargs - 2);
  }
  ase_variant metaGetMethodId(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "GetMethodId(object, name)");
    ase_string s = args[1].get_string();
    return ase_variant(args[0].GetMethodId(s.data(), s.size()));
  }
  ase_variant metaGetInstanceMethodId(const ase_variant *args,
    ase_size_type nargs) {
    checkArgs(2, nargs, "get_instance_method_id(object, name)");
    ase_string s = args[1].get_string();
    return ase_variant(args[0].get_instance_method_id(s.data(), s.size()));
  }
  ase_variant metaInvokeById(const ase_variant *args, ase_size_type nargs) {
    checkArgs(2, nargs, "invoke_by_id(object, id)");
    return args[0].invoke_by_id(args[1].get_int(), args + 2, nargs - 2);
  }
  ase_variant metaInvokeSelf(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "invoke_self(object, args...)");
    return args[0].invoke_self(args + 1, nargs - 1);
  }
  ase_variant metaGetEnumerator(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "get_enumerator(object)");
    return args[0].get_enumerator();
  }
  ase_variant metaEnumNext(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "enum_next(object)");
    bool has_next = false;
    ase_string s = args[0].enum_next(has_next);
    if (!has_next) {
      return ase_variant();
    }
    return s.to_variant();
  }
  ase_variant metaGetWeak(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "GetWeak(object)");
    return args[0].GetWeak();
  }
  ase_variant metaGetStrong(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "GetStrong(object)");
    return args[0].GetStrong();
  }

  /* script engine metamethods */
  ase_variant metaLoadFile(const ase_variant *args, ase_size_type nargs) {
    checkArgs(1, nargs, "LoadFile(filename)");
    ase_unified_engine_ref& ue = libref.GetUnifiedEngineRef();
    return ue.LoadFile(args[0].get_string());
  }
  ase_variant metaForceGC(const ase_variant *args, ase_size_type nargs) {
    ase_unified_engine_ref& ue = libref.GetUnifiedEngineRef();
    ue.ForceGC();
    return ase_variant();
  }
  ase_variant metaWeakenProxies(const ase_variant *args, ase_size_type nargs) {
    ase_unified_engine_ref& ue = libref.GetUnifiedEngineRef();
    ue.WeakenProxies();
    return ase_variant();
  }
  ase_variant metaAbort(const ase_variant *args, ase_size_type nargs) {
    ::abort();
    return ase_variant();
  }
  ase_variant Noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }

  ASE_Component::Library& libref;

  static const ASE_Component::MethodEntry<metamethods> ClassMethods[];
  static const ASE_Component::MethodEntry<metamethods> InstanceMethods[];

};

const ASE_Component::MethodEntry<metamethods> metamethods::ClassMethods[] = {
  { "get_boolean",            &metamethods::metaGetBoolean },
  { "get_int",                &metamethods::metaGetInt },
  { "get_long",               &metamethods::metaGetLong },
  { "get_double",             &metamethods::metaGetDouble },
  { "get_string",             &metamethods::metaGetString },
  { "get_property",           &metamethods::metaGetProperty },
  { "set_property",           &metamethods::metaSetProperty },
  { "del_property",           &metamethods::metaDelProperty },
  { "get_element",            &metamethods::metaGetElement },
  { "set_element",            &metamethods::metaSetElement },
  { "get_length",             &metamethods::metaGetLength },
  { "set_length",             &metamethods::metaSetLength },
  { "invoke_by_name",          &metamethods::metaInvokeByName },
  { "GetMethodId",           &metamethods::metaGetMethodId },
  { "get_instance_method_id",   &metamethods::metaGetInstanceMethodId },
  { "invoke_by_id",            &metamethods::metaInvokeById },
  { "invoke_self",            &metamethods::metaInvokeSelf },
  { "get_enumerator",         &metamethods::metaGetEnumerator },
  { "enum_next",              &metamethods::metaEnumNext },
  { "GetWeak",               &metamethods::metaGetWeak },
  { "GetStrong",             &metamethods::metaGetStrong },
  { "LoadFile",              &metamethods::metaLoadFile },
  { "ForceGC",               &metamethods::metaForceGC },
  { "WeakenProxies",         &metamethods::metaWeakenProxies },
  { "Abort",                 &metamethods::metaAbort },
};

const ASE_Component::MethodEntry<metamethods>
metamethods::InstanceMethods[] = {
  { "Noop",                 &metamethods::Noop },
};

}; // anonymous namespace

ase_variant
ASE_NewUtilMetaClass(const ase_string& name, ASE_Component::Library& lib)
{
  return ASE_Component::NewClass<
    metamethods, metamethods, true
  >(name, lib);
}

