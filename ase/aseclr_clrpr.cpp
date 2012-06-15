
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "aseclr_clrpr.hpp"
#include "aseclr_util.hpp"

#include <ase/asebindmethod.hpp>
#include <ase/aseutf16.hpp>
#include <ase/asecast.hpp>
#include <ase/aseintermed.hpp>
#include <ase/aseexcept.hpp>

#define DBG(x)
#define DBG_RC(x)
#define DBG_DEL(x)
#define DBG_INV(x)
#define DBG_DPR(x)

#define ASE_CLR_VARIANT_DEBUGID "CLRObject"

namespace aseclr {

const ase_variant_vtable *
get_clrproxy_vtable()
{
  return &ase_variant_impl<
      ase_clr_ase_variant_impl,
      ase_vtoption_multithreaded
    >::vtable;
}

bool
is_clrproxy(const ase_variant& val)
{
  return (val.get_vtable_address() == get_clrproxy_vtable());
}

template <typename F> void
catch_clr_exceptions(const ase_variant& self, F& fobj)
{
  ase_clr_ase_variant_implnode *const selfp =
    ase_clr_ase_variant_impl::get(self);
  try {
    fobj(self, selfp);
  } catch (const std::exception&) {
    throw;
  } catch (const ase_variant&) {
    throw;
  } catch (System::Exception ^e) {
    throw_ase_exception(selfp->value.get_sc(), e);
  }
}


ase_clr_ase_variant_impl::ase_clr_ase_variant_impl(const init_arg& arg)
  : refcount(1), script_engine(arg.sc), value(arg.obj)
{
}

ase_clr_ase_variant_impl::~ase_clr_ase_variant_impl()
{
}

void
ase_clr_ase_variant_impl::dispose()
{
  value = nullptr;
}

void
ase_clr_ase_variant_impl::add_ref(const ase_variant& self)
{
  ase_clr_ase_variant_implnode *const selfp = get(self);
  DBG_RC(fprintf(stderr, "ASECLR addref clrpr %p %lu++\n",
    selfp, selfp->value.refcount));
  selfp->value.refcount++;
}

void
ase_clr_ase_variant_impl::release(const ase_variant& self)
{
  ase_clr_ase_variant_implnode *const selfp = get(self);
  DBG_RC(fprintf(stderr, "ASECLR release clrpr %p %lu--\n",
    selfp, selfp->value.refcount));
  if ((selfp->value.refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, ASE_CLR_VARIANT_DEBUGID);
    DBG_DEL(fprintf(stderr, "ASECLR destroy clrpr %p\n", selfp));
    delete selfp;
  }
}

ase_int
ase_clr_ase_variant_impl::get_attributes(const ase_variant& self)
{
  return 0;
}

bool
ase_clr_ase_variant_impl::get_boolean(const ase_variant& self)
{
  return false;
}

ase_int
ase_clr_ase_variant_impl::get_int(const ase_variant& self)
{
  return 0;
}

ase_long
ase_clr_ase_variant_impl::get_long(const ase_variant& self)
{
  return 0;
}

double
ase_clr_ase_variant_impl::get_double(const ase_variant& self)
{
  return 0.0;
}

struct vimpl_getstring {
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    System::String ^str = obj->ToString();
    rval = to_ase_string(str);
  }
  ase_string rval;
};

ase_string
ase_clr_ase_variant_impl::get_string(const ase_variant& self)
{
  vimpl_getstring fobj;
  catch_clr_exceptions(self, fobj);
  return fobj.rval;
}

struct vimpl_issameobject {
  vimpl_issameobject(const ase_variant& ov) : oval(ov), rval(false) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^sobj = selfp->value.get();
    System::Object ^oobj = extract_clrproxy(oval);
    rval = (sobj == oobj);
  }
  const ase_variant& oval;
  bool rval;
};

bool
ase_clr_ase_variant_impl::is_same_object(const ase_variant& self,
  const ase_variant& value)
{
  vimpl_issameobject fobj(value);
  catch_clr_exceptions(self, fobj);
  return fobj.rval;
}

struct vimpl_getproperty {
  vimpl_getproperty(const char *n, ase_size_type nlen)
    : name(n), namelen(nlen) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    System::String ^n = to_clr_string(name, namelen);
    System::Type ^typ = dynamic_cast<System::Type ^>(obj);
    System::Reflection::MemberTypes proponly =
      System::Reflection::MemberTypes::Property |
      System::Reflection::MemberTypes::Field;
    array<System::Reflection::MemberInfo ^> ^mems = nullptr;
    bool found = false;
    System::Object ^robj = nullptr;
    if (typ != nullptr) {
      /* property of System::Type class? */
      System::Reflection::BindingFlags memfl =
        System::Reflection::BindingFlags::GetProperty |
        System::Reflection::BindingFlags::GetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Instance |
        System::Reflection::BindingFlags::Static;
      mems = System::Type::typeid->GetMember(n, proponly, memfl);
      if (mems->Length != 0) {
        /* property of System::Type class */
        robj = System::Type::typeid->InvokeMember(n, memfl, nullptr, typ,
          nullptr);
        found = true;
      } else {
        /* static property */
        System::Reflection::BindingFlags fl =
          System::Reflection::BindingFlags::GetProperty |
          System::Reflection::BindingFlags::GetField |
          System::Reflection::BindingFlags::Public |
          System::Reflection::BindingFlags::Static;
        mems = typ->GetMember(n, proponly, fl);
        if (mems->Length != 0) {
          robj = typ->InvokeMember(n, memfl, nullptr, nullptr, nullptr);
          found = true;
        }
      }
    } else {
      /* instance property */
      System::Reflection::BindingFlags fl =
        System::Reflection::BindingFlags::GetProperty |
        System::Reflection::BindingFlags::GetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Instance |
        System::Reflection::BindingFlags::Static;
      System::Type ^otyp = obj->GetType();
      mems = otyp->GetMember(n, proponly, fl);
      if (mems->Length != 0) {
        robj = otyp->InvokeMember(n, fl, nullptr, obj, nullptr);
        found = true;
      } else {
        System::Reflection::Assembly ^a =
          dynamic_cast<System::Reflection::Assembly ^>(obj);
        if (a != nullptr) {
          /* member of System::Assembly? */
          System::Reflection::BindingFlags memfl =
            System::Reflection::BindingFlags::InvokeMethod |
            System::Reflection::BindingFlags::GetProperty |
            System::Reflection::BindingFlags::GetField |
            System::Reflection::BindingFlags::Public |
            System::Reflection::BindingFlags::Instance |
            System::Reflection::BindingFlags::Static;
          mems = otyp->GetMember(n, fl);
          if (mems->Length == 0) {
            /* get type or namespace */
            System::Type ^t = a->GetType(n, false); /* nothrow */
            if (t != nullptr) {
              robj = t;
              found = true;
            } else {
              /* namespace */
              ase_string s(name, namelen);
              rval = ase_new_intermediate_property(self, s + ".");
              return;
            }
          }
        }
      }
    }
    if (found) {
      rval = to_ase_value(selfp->value.get_sc(), robj);
    } else {
      rval = ase_new_variant_bind_method_name(self, ase_string(name, namelen));
    }
  }
  const char *const name;
  ase_size_type namelen;
  ase_variant rval;
};

ase_variant
ase_clr_ase_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  vimpl_getproperty arg(name, namelen);
  catch_clr_exceptions(self, arg);
  return arg.rval;
}

struct vimpl_setproperty {
  vimpl_setproperty(const char *n, ase_size_type nlen, const ase_variant& v)
    : name(n), namelen(nlen), value(v) { }
  void operator ()(const ase_variant& self, ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    array<System::Object ^> ^cargs =
      to_clr_value_array(selfp->value.get_sc(), &value, 1);
    System::String ^n = to_clr_string(name, namelen);
    System::Type ^typ = dynamic_cast<System::Type ^>(obj);
    if (typ != nullptr) {
      /* static property */
      System::Reflection::BindingFlags fl =
        System::Reflection::BindingFlags::SetProperty |
        System::Reflection::BindingFlags::SetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Static;
      typ->InvokeMember(n, fl, nullptr, nullptr, cargs);
    } else {
      /* instance property */
      System::Reflection::BindingFlags fl =
        System::Reflection::BindingFlags::SetProperty |
        System::Reflection::BindingFlags::SetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Instance |
        System::Reflection::BindingFlags::Static;
      obj->GetType()->InvokeMember(n, fl, nullptr, obj, cargs);
    }
  }
  const char *const name;
  const ase_size_type namelen;
  const ase_variant value;
};

void
ase_clr_ase_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  vimpl_setproperty arg(name, namelen, value);
  catch_clr_exceptions(self, arg);
}

void
ase_clr_ase_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
}

struct vimpl_getelement {
  vimpl_getelement(ase_int idx) : index(idx) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    if (obj == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to get element");
    }
    System::Collections::IList ^lst =
      dynamic_cast<System::Collections::IList ^>(obj);
    if (lst == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to get element");
    }
    System::Object^ robj = lst[index];
    rval = to_ase_value(selfp->value.get_sc(), robj);
  }
  const ase_int index;
  ase_variant rval;
};

ase_variant
ase_clr_ase_variant_impl::get_element(const ase_variant& self, ase_int index)
{
  vimpl_getelement arg(index);
  catch_clr_exceptions(self, arg);
  return arg.rval;
}

struct vimpl_setelement {
  vimpl_setelement(ase_int idx, const ase_variant& v)
    : index(idx), value(v) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    if (obj == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to set element");
    }
    System::Collections::IList ^lst =
      dynamic_cast<System::Collections::IList ^>(obj);
    if (lst == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to set element");
    }
    lst[index] = to_clr_value(selfp->value.get_sc(), value);
  }
  ase_int index;
  const ase_variant value;
};

void
ase_clr_ase_variant_impl::set_element(const ase_variant& self, ase_int index,
  const ase_variant& value)
{
  vimpl_setelement arg(index, value);
  catch_clr_exceptions(self, arg);
}

struct vimpl_getlength {
  vimpl_getlength() : rval(-1) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    if (obj == nullptr) {
      return;
    }
    System::Collections::ICollection ^cs =
      dynamic_cast<System::Collections::ICollection ^>(obj);
    if (cs == nullptr) {
      return;
    }
    rval = cs->Count;
  }
  ase_int rval;
};

ase_int
ase_clr_ase_variant_impl::get_length(const ase_variant& self)
{
  vimpl_getlength arg;
  catch_clr_exceptions(self, arg);
  return arg.rval;
}

struct vimpl_setlength {
  vimpl_setlength(ase_int l) : len(l) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    if (obj == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to set length");
    }
    System::Collections::ArrayList ^lst =
      dynamic_cast<System::Collections::ArrayList ^>(obj);
    if (lst == nullptr) {
      ase_throw_illegal_operation("ASECLR: attempt to set length");
    }
    /* TODO: implement */
  }
  const ase_int len;
};

void
ase_clr_ase_variant_impl::set_length(const ase_variant& self, ase_int len)
{
  vimpl_setlength arg(len);
  catch_clr_exceptions(self, arg);
}

struct vimpl_invokebyname {
  vimpl_invokebyname(const char *n, ase_size_type nlen, const ase_variant *a,
    ase_size_type na)
    : name(n), namelen(nlen), args(a), nargs(na) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    System::Object ^robj = nullptr;
    array<System::Object ^> ^cargs =
      to_clr_value_array(selfp->value.get_sc(), args, nargs);
    System::String ^n = to_clr_string(name, namelen);
    System::Type ^typ = dynamic_cast<System::Type ^>(obj);
    if (typ != nullptr) {
      if (ase_string::equals(name, namelen, "NewInstance")) {
        /* create instance */
        DBG_INV(System::Console::WriteLine("TYP NewInstance"));
        System::Reflection::BindingFlags fl =
          System::Reflection::BindingFlags::CreateInstance |
          System::Reflection::BindingFlags::Public |
          System::Reflection::BindingFlags::Instance;
        robj = typ->InvokeMember(nullptr, fl, nullptr, nullptr, cargs);
      } else if (ase_string::equals(name, namelen, "NewArray")) {
        /* create array */
        DBG_INV(System::Console::WriteLine("TYP NewArray"));
        System::Type ^arrtyp = typ->MakeArrayType(cargs->Length);
        System::Reflection::BindingFlags fl =
          System::Reflection::BindingFlags::CreateInstance |
          System::Reflection::BindingFlags::Public |
          System::Reflection::BindingFlags::Instance;
        for (int i = 0; i < cargs->Length; ++i) {
          cargs[i] = System::Convert::ChangeType(cargs[i],
            System::Int32::typeid);
        }
        robj = arrtyp->InvokeMember(nullptr, fl, nullptr, nullptr, cargs);
      } else {
        /* method of System::Type class? */
        System::Reflection::BindingFlags memfl =
          System::Reflection::BindingFlags::InvokeMethod |
          System::Reflection::BindingFlags::GetProperty |
          System::Reflection::BindingFlags::GetField |
          System::Reflection::BindingFlags::Public |
          System::Reflection::BindingFlags::Instance |
          System::Reflection::BindingFlags::Static;
        array<System::Reflection::MemberInfo ^> ^mems =
          System::Type::typeid->GetMember(n, memfl);
        if (mems->Length != 0) {
          /* method of System::Type class */
          DBG_INV(System::Console::WriteLine("TYP typeclass mtd=" + n));
          robj = System::Type::typeid->InvokeMember(n, memfl, nullptr, typ,
            cargs);
        } else {
          /* static method */
          DBG_INV(System::Console::WriteLine("TYP STATIC mtd=" + n));
          System::Reflection::BindingFlags fl =
            System::Reflection::BindingFlags::InvokeMethod |
            System::Reflection::BindingFlags::GetProperty |
            System::Reflection::BindingFlags::GetField |
            System::Reflection::BindingFlags::Public |
            System::Reflection::BindingFlags::Static;
          robj = typ->InvokeMember(n, fl, nullptr, nullptr, cargs);
        }
      }
    } else {
      /* instance method */
      DBG_INV(System::Console::WriteLine("INSTANCE mtd=" + n));
      System::Reflection::BindingFlags fl =
        System::Reflection::BindingFlags::InvokeMethod |
        System::Reflection::BindingFlags::GetProperty |
        System::Reflection::BindingFlags::GetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Instance |
        System::Reflection::BindingFlags::Static;
      robj = obj->GetType()->InvokeMember(n, fl, nullptr, obj, cargs);
    }
    rval = to_ase_value(selfp->value.get_sc(), robj);
  }
  const char *const name;
  const ase_size_type namelen;
  const ase_variant *const args;
  const ase_size_type nargs;
  ase_variant rval;
};

ase_variant
ase_clr_ase_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  vimpl_invokebyname arg(name, namelen, args, nargs);
  catch_clr_exceptions(self, arg);
  return arg.rval;
}

ase_variant
ase_clr_ase_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

struct vimpl_invokeself {
  vimpl_invokeself(const ase_variant *a, ase_size_type na)
    : args(a), nargs(na) { }
  void operator ()(const ase_variant& self,
    ase_clr_ase_variant_implnode *selfp) {
    System::Object ^obj = selfp->value.get();
    System::Object ^robj = nullptr;
    System::Type ^typ = dynamic_cast<System::Type ^>(obj);
    if (typ != nullptr) {
      array<System::Object ^> ^cargs =
	to_clr_value_array(selfp->value.get_sc(), args, nargs);
      System::Reflection::BindingFlags fl =
	System::Reflection::BindingFlags::CreateInstance |
	System::Reflection::BindingFlags::Public |
	System::Reflection::BindingFlags::Instance;
      robj = typ->InvokeMember(nullptr, fl, nullptr, nullptr, cargs);
    } else {
      /* obj("methodname", args, ...) */
      if (nargs < 1) {
	ase_throw_missing_arg("ASECLR: missing method name");
      }
      System::Object ^sobj =
	to_clr_value(selfp->value.get_sc(), args[0]);
      System::String ^n = cli::safe_cast<System::String ^>(sobj);
      array<System::Object ^> ^cargs =
	to_clr_value_array(selfp->value.get_sc(), args + 1, nargs - 1);
      System::Reflection::BindingFlags fl =
        System::Reflection::BindingFlags::InvokeMethod |
        System::Reflection::BindingFlags::GetProperty |
        System::Reflection::BindingFlags::GetField |
        System::Reflection::BindingFlags::Public |
        System::Reflection::BindingFlags::Instance |
        System::Reflection::BindingFlags::Static;
      robj = obj->GetType()->InvokeMember(n, fl, nullptr, obj, cargs);
    }
    rval = to_ase_value(selfp->value.get_sc(), robj);
  }
  const ase_variant *const args;
  const ase_size_type nargs;
  ase_variant rval;
};

ase_variant
ase_clr_ase_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  vimpl_invokeself arg(args, nargs);
  catch_clr_exceptions(self, arg);
  return arg.rval;
}

ase_variant
ase_clr_ase_variant_impl::get_enumerator(const ase_variant& self)
{
  return ase_variant();
}

ase_string
ase_clr_ase_variant_impl::enum_next(const ase_variant& self, bool& hasnext_r)
{
  hasnext_r = false;
  return ase_string();
}

ase_variant
create_clrproxy(ase_clr_script_engine *sc, System::Object ^obj)
{
  ase_clr_ase_variant_impl::init_arg arg;
  arg.sc = sc;
  arg.obj = obj;
  ase_clr_ase_variant_implnode *p = new ase_clr_ase_variant_implnode(arg,
    *sc->get_clrpr_endnode());
  return ase_variant::create_object(get_clrproxy_vtable(), p,
    ASE_CLR_VARIANT_DEBUGID);
}

System::Object ^
extract_clrproxy(const ase_variant& val)
{
  if (is_clrproxy(val)) {
    ase_clr_ase_variant_implnode *p =
      static_cast<ase_clr_ase_variant_implnode *>(val.get_rep().p);
    return p->value.get();
  }
  return nullptr;
}

}; // namespace aseclr

