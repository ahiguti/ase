
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "aseclr_clrpr.hpp"
#include "aseclr_dynproxy.hpp"
#include "aseclr_util.hpp"

#include <ase/asebindmethod.hpp>
#include <ase/aseutf16.hpp>
#include <ase/asecast.hpp>
#include <ase/aseexcept.hpp>

#pragma warning(push)
#pragma warning(disable : 4091)
#include <msclr/lock.h>
#pragma warning(pop)

#define DBG(x)
#define DBG_RC(x)
#define DBG_DEL(x)
#define DBG_INV(x)
#define DBG_DPR(x)

namespace aseclr {

using namespace System::Runtime::Remoting;

ref class ase_clr_dynamic_proxy : Proxies::RealProxy {

 public:

  ase_clr_dynamic_proxy(ase_clr_script_engine *sc, System::Type ^kl,
    const ase_variant& val, System::Object ^bobj)
    : Proxies::RealProxy(kl), clrklass(kl), script_engine(sc),
      value(new aseproxy_node(val, *sc->get_asepr_endnode())), baseobj(bobj) {
  }
  ~ase_clr_dynamic_proxy() {
    dispose_synchronized();
  }
  !ase_clr_dynamic_proxy() {
    dispose_synchronized();
  }
  ase_variant get_synchronized() {
    msclr::lock lck(this);
    if (value) {
      return value->value.get();
    }
    return ase_variant();
  }
  void dispose_synchronized() {
    ase_variant v;
    {
      msclr::lock lck(this);
      if (value) {
        value->value.swap(v);
        delete value;
      }
      value = nullptr;
    }
    /* v is released here */
  }
  virtual Messaging::IMessage ^Invoke(Messaging::IMessage ^amsg)
    override sealed {
    try {
      Messaging::IMethodCallMessage ^mmsg =
        dynamic_cast<Messaging::IMethodCallMessage ^>(amsg);
      int nargs = mmsg->ArgCount;
      ase_string aname = to_ase_string(mmsg->MethodName);
      DBG_DPR(fprintf(stderr, "ASECLR: dynpr %s\n", aname.data()));
      ase_variant obj = get_synchronized();
      ase_variant prop = obj.get_property(aname.data(), aname.size());
      System::Object ^rval = nullptr;
      if (prop.is_void()) {
        if (baseobj != nullptr) {
          array<System::Object ^> ^cargs =
	    gcnew array<System::Object ^>(nargs);
          for (int i = 0; i < nargs; ++i) {
            cargs[i] = mmsg->GetArg(i);
          }
          System::Reflection::BindingFlags fl =
            System::Reflection::BindingFlags::InvokeMethod |
            System::Reflection::BindingFlags::Public |
            System::Reflection::BindingFlags::Instance;
          rval = baseobj->GetType()->InvokeMember(mmsg->MethodName, fl,
	    nullptr, baseobj, cargs);
        } else if (aname == "equals") {
          if (nargs > 0) {
            ase_variant x = to_ase_value(script_engine, mmsg->GetArg(0));
            rval = (x.is_same_object(x));
          } else {
            rval = false;
          }
        } else if (aname == "get_type") {
          rval = clrklass;
        } else if (aname == "GetHashCode") {
          rval = this->GetHashCode();
        } else if (aname == "Finalize") {
          rval = nullptr;
        } else {
	  ase_string s = "ASECLR: dynamic proxy: method not found: " + aname;
	  ase_throw_invalid_arg(s.data());
        }
      } else {
        std::vector<ase_variant> args(nargs);
        for (int i = 0; i < nargs; ++i) {
          args[i] = to_ase_value(script_engine, mmsg->GetArg(i));
        }
        ase_variant arval = prop.invoke_self(nargs ? (&args[0]) : 0,
          static_cast<ase_size_type>(nargs));
        rval = to_clr_value(script_engine, arval);
      }
      Messaging::ReturnMessage ^rmsg = 
        gcnew Messaging::ReturnMessage(rval, nullptr, 0,
	  mmsg->LogicalCallContext, mmsg);
      return rmsg;
    } catch (const std::exception& e) {
      /* TODO: create wrapped exceptions. see aseclr_delegate slso. */
      std::string wh = e.what();
      System::String ^mess = to_clr_string(wh.data(), wh.size());
      throw gcnew System::Exception(mess);
    } catch (const ase_variant& e) {
      ase_string wh = e.get_string();
      System::String ^mess = to_clr_string(wh.data(), wh.size());
      throw gcnew System::Exception(mess);
    }
  }

 private:
 
  System::Type ^clrklass;
  ase_clr_script_engine *script_engine;
  aseproxy_node *value; 
  System::Object ^baseobj;

};

System::Object ^
create_dynamic_proxy(ase_clr_script_engine *sc, System::Type ^typ,
  const ase_variant& val, System::Object ^baseobj)
{
  Proxies::RealProxy ^pr = gcnew ase_clr_dynamic_proxy(sc, typ, val, baseobj);
  return pr->GetTransparentProxy();
}

}; // namespace aseclr

