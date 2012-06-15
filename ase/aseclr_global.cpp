
#include <ase/ase.hpp>
#include <ase/asecclass.hpp>
#include <ase/aseexcept.hpp>

#include "aseclr_global.hpp"
#include "aseclr_dynproxy.hpp"
#include "aseclr_sctx.hpp"
#include "aseclr_wrapper.hpp"

namespace aseclr {

namespace {

int
delim_index(System::String ^str)
{
  int r = str->IndexOf('\\');
  if (r < 0) {
    r = str->IndexOf('/');
  }
  return r;
}

int
delim_lastindex(System::String ^str)
{
  int r = str->LastIndexOf('\\');
  if (r < 0) {
    r = str->LastIndexOf('/');
  }
  return r;
}

System::Reflection::Assembly ^
load_assembly_internal(const ase_string& name)
{
  System::String ^nm = aseclr::to_clr_string(name);
  System::String ^cb = nullptr;
  if (delim_index(nm) < 0) {
    System::Reflection::Assembly ^core =
      System::Reflection::Assembly::GetAssembly(System::Object::typeid);
    System::String ^loc = core->Location;
    int ridx = delim_lastindex(loc);
    if (ridx >= 0) {
      loc = loc->Substring(0, ridx + 1);
      cb = loc + nm + ".dll";
    }
    System::Reflection::AssemblyName ^an = gcnew
      System::Reflection::AssemblyName();
    an->Name = nm;
    an->CodeBase = cb;
    System::Reflection::Assembly ^a =
      System::Reflection::Assembly::Load(an);
    return a;
  } else {
    System::Reflection::Assembly ^a =
      System::Reflection::Assembly::LoadFile(nm);
    return a;
  }
}

ase_variant
load_assembly(ase_clr_script_engine *sc, const ase_string& name)
{
  return aseclr::to_ase_value(sc, load_assembly_internal(name));
}

ase_variant
load_klass(ase_clr_script_engine *sc, const ase_string& mod,
  const ase_string& kl)
{
  System::Reflection::Assembly ^a = load_assembly_internal(mod);
  ase_variant av = aseclr::to_ase_value(sc, a);
  if (kl.empty()) {
    return av;
  }
  return av.get_property(kl.data(), kl.size());
}

struct clrklass {

  clrklass() : script_engine(0) { }

  void set_engine_ref(ase_clr_script_engine *sc) {
    script_engine = sc;
  }

  ase_variant internalGetExecutingAssembly(const ase_variant *args,
    ase_size_type nargs) {
    return aseclr::get_executing_assembly(script_engine);
  }
  ase_variant internalGetCore(const ase_variant *args, ase_size_type nargs) {
    return aseclr::get_mscorlib_assembly(script_engine);
  }
  ase_variant internalLoadAssembly(const ase_variant *args,
    ase_size_type nargs) {
    if (nargs < 1) {
      ase_throw_missing_arg("ASECLR: Usage: LoadAssembly(name)");
    }
    return load_assembly(script_engine, args[0].get_string());
  }
  ase_variant internalLoad(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      ase_throw_missing_arg("ASECLR: Usage: Load(modulename, [classname])");
    }
    ase_string s;
    if (nargs > 1) {
      s = args[1].get_string();
    }
    return load_klass(script_engine, args[0].get_string(), s);
  }
  ase_variant internalExtend(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 2) {
      ase_throw_missing_arg("ASECLR: Usage: Extend(clrobject, aseobject)");
    }
    System::Object ^obj = aseclr::extract_clrproxy(args[0]);
    if (obj == nullptr) {
      ase_throw_type_mismatch("ASECLR: not a CLR object");
    }
    System::Object ^r = aseclr::create_dynamic_proxy(script_engine,
      obj->GetType(), args[1], obj);
    return aseclr::to_ase_value(script_engine, r);
  }
  ase_variant internalImplement(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 2) {
      ase_throw_missing_arg("ASECLR: Usage: Implement(clrtype, aseobject)");
    }
    System::Object ^obj = aseclr::extract_clrproxy(args[0]);
    System::Type ^typ = cli::safe_cast<System::Type ^>(obj);
    System::Object ^r = aseclr::create_dynamic_proxy(script_engine,
      typ, args[1], nullptr);
    return aseclr::to_ase_value(script_engine, r);
  }
  ase_variant internalCreateDelegate(const ase_variant *args,
    ase_size_type nargs) {
    if (nargs < 2) {
      ase_throw_missing_arg(
        "ASECLR: Usage: CreateDelegate(asetype, aseobject)");
    }
    return aseclr::create_delegate(script_engine, args[0], args[1]);
  }
  ase_variant internalCreateWrapper(const ase_variant *args,
    ase_size_type nargs) {
    if (nargs < 2) {
      ase_throw_missing_arg("ASECLR: Usage: CreateWrapper(type, value)");
    }
    System::Object ^tobj = aseclr::to_clr_value(script_engine, args[0]);
    System::Type ^typ = cli::safe_cast<System::Type ^>(tobj);
    System::Object ^v = aseclr::to_clr_value(script_engine, args[1]);
    System::Object ^wr = gcnew ValueWrapper(typ, v);
    return aseclr::to_ase_value(script_engine, wr);
  }
  ase_variant internalEnumOr(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      ase_throw_missing_arg("ASECLR: Usage: EnumOr(values, ...)");
    }
    System::Object ^v = aseclr::to_clr_value(script_engine, args[0]);
    System::Type ^tv = v->GetType();
    System::Int32 rv = System::Convert::ToInt32(v);
    /* TODO: FlagsAttribute */
    for (ase_size_type i = 1; i < nargs; ++i) {
      System::Object ^ao = aseclr::to_clr_value(script_engine, args[i]);
      if (!tv->IsInstanceOfType(ao)) {
	ase_throw_type_mismatch("ASECLR: EnumOr: type mismatch");
      }
      System::Int32 iv = System::Convert::ToInt32(ao);
      rv |= iv;
    }
    System::Object ^ro = System::Enum::ToObject(tv, rv);
    return aseclr::to_ase_value(script_engine, ro);
  }
  ase_variant internalAddEventHandler(const ase_variant *args,
    ase_size_type nargs) {
    if (nargs < 3) {
      ase_throw_missing_arg(
        "ASECLR: Usage: AddEventHandler(object, eventname, handler)");
    }
    return aseclr::add_event_handler(script_engine, args[0],
      args[1].get_string(), args[2]);
  }
  ase_variant internalRemoveEventHandler(const ase_variant *args,
    ase_size_type nargs) {
    if (nargs < 3) {
      ase_throw_missing_arg(
        "ASECLR: Usage: RemoveEventHandler(object, eventname, handler)");
    }
    aseclr::remove_event_handler(script_engine, args[0], args[1].get_string(),
      args[2]);
    return ase_variant();
  }
  
  ase_variant handle_exceptions(
    ase_variant (clrklass::*f)(const ase_variant *, ase_size_type),
    const ase_variant *args, ase_size_type nargs) {
    try {
      return (this->*f)(args, nargs);
    } catch (const std::exception&) {
      /* we need to handle std::exception and ase_variant explicitly, or
        they are caught by the following 'catch System::Exception' clause */
      throw;
    } catch (const ase_variant&) {
      throw;
    } catch (System::Exception ^ex) {
      aseclr::throw_ase_exception(script_engine, ex);
    }
  }
  
  ase_variant GetExecutingAssembly(const ase_variant *args,
    ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalGetExecutingAssembly,
      args, nargs);
  }
  ase_variant GetCore(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalGetCore, args, nargs);
  }
  ase_variant LoadAssembly(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalLoadAssembly, args, nargs);
  }
  ase_variant Load(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalLoad, args, nargs);
  }
  ase_variant Extend(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalExtend, args, nargs);
  }
  ase_variant Implement(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalImplement, args, nargs);
  }
  ase_variant CreateDelegate(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalCreateDelegate, args, nargs);
  }
  ase_variant CreateWrapper(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalCreateWrapper, args, nargs);
  }
  ase_variant EnumOr(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalEnumOr, args, nargs);
  }
  ase_variant AddEventHandler(const ase_variant *args, ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalAddEventHandler, args, nargs);
  }
  ase_variant RemoveEventHandler(const ase_variant *args,
    ase_size_type nargs) {
    return handle_exceptions(&clrklass::internalRemoveEventHandler,
      args, nargs);
  }
  ase_variant Noop(const ase_variant *args, ase_size_type nargs) {
    return ase_variant();
  }
  
 private:
 
  ase_clr_script_engine *script_engine;

};

}; // anonymous namespace

void
ase_clr_initialize()
{
  ase_cclass<clrklass>::initializer()
    .def("GetExecutingAssembly", &clrklass::GetExecutingAssembly)
    .def("GetCore",              &clrklass::GetCore)
    .def("LoadAssembly",         &clrklass::LoadAssembly)
    .def("Load",                 &clrklass::Load)
    .def("Extend",               &clrklass::Extend)
    .def("Implement",            &clrklass::Implement)
    .def("CreateDelegate",       &clrklass::CreateDelegate)
    .def("CreateWrapper",        &clrklass::CreateWrapper)
    .def("EnumOr",               &clrklass::EnumOr)
    .def("AddEventHandler",      &clrklass::AddEventHandler)
    .def("RemoveEventHandler",   &clrklass::RemoveEventHandler)
    ;
}

ase_variant
ase_clr_create_global()
{
  return ase_cclass<clrklass>::create0();
}

void
ase_clr_set_engine_ref(const ase_variant& globj, ase_clr_script_engine *sc)
{
  clrklass& kl = ase_cclass<clrklass>::unbox(globj);
  kl.set_engine_ref(sc);
}

}; // namespace aseclr

