
#include <ase/asecpnt.hpp>
#include <ase/aseexcept.hpp>
#include <vector>
#include <iostream>

namespace {

struct asetestsvrclass {

  asetestsvrclass(const ase_string& n, ASE_Component::Library& lib) {
  }
  ase_variant StaticSum(const ase_variant *args, ase_size_type nargs) {
    int r = 0;
    for (ase_size_type i = 0; i < nargs; ++i) {
      r += args[i].get_int();
    }
    return r;
  }

  static const ASE_Component::MethodEntry<asetestsvrclass> ClassMethods[];

};

struct asetestsvr {
  asetestsvr(const ase_string& n, const asetestsvrclass& klass,
    const ase_variant *args, ase_size_type nargs) : intval(0) {
  }
  ase_variant SetInt(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      throw std::runtime_error("ASETestSvr::SetInt: missing arg");
    }
    intval = args[0].get_int();
    return ase_variant();
  }
  ase_variant get_int(const ase_variant *args, ase_size_type nargs) {
    return intval;
  }
  ase_variant AddInt(const ase_variant *args, ase_size_type nargs) {
    for (ase_size_type i = 0; i < nargs; ++i) {
      intval += args[i].get_int();
    }
    return ase_variant();
  }
  ase_variant SetString(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      throw std::runtime_error("ASETestSvr::SetString: missing arg");
    }
    strval = args[0].get_string();
    return ase_variant();
  }
  ase_variant get_string(const ase_variant *args, ase_size_type nargs) {
    return strval.to_variant();
  }
  ase_variant AddStrlen(const ase_variant *args, ase_size_type nargs) {
    for (ase_size_type i = 0; i < nargs; ++i) {
      intval += static_cast<int>(args[i].get_string().size());
    }
    return ase_variant();
  }
  ase_variant SetCallback(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      throw std::runtime_error("ASETestSvr::SetCallback: missing arg");
    }
    objval = args[0];
    return ase_variant();
  }
  ase_variant GetCallback(const ase_variant *args, ase_size_type nargs) {
    return objval;
  }
  ase_variant CallbackInt(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      throw std::runtime_error("ASETestSvr::CallbackInt: missing arg");
    }
    int count = args[0].get_int();
    --nargs;
    std::cout << "CallbackInt: ";
    std::vector<ase_variant> cbargs(nargs);
    for (ase_size_type i = 0; i < nargs; ++i) {
      cbargs[i] = args[i + 1].get_int();
      std::cout << cbargs[i] << " ";
    }
    std::cout << std::endl;
    const ase_variant *cba = nargs ? (&cbargs[0]) : 0;
    int r = 0;
    for (int i = 0; i < count; ++i) {
      r += objval.invoke_self(cba, nargs).get_int();
    }
    std::cout << "CallbackInt: r=" << r << std::endl;
    return r;
  }
  ase_variant CallbackString(const ase_variant *args, ase_size_type nargs) {
    if (nargs < 1) {
      throw std::runtime_error("ASETestSvr::CallbackInt: missing arg");
    }
    int count = args[0].get_int();
    --nargs;
    std::cout << "CallbackString: ";
    std::vector<ase_variant> cbargs(nargs);
    for (ase_size_type i = 0; i < nargs; ++i) {
      cbargs[i] = args[i + 1].get_string().to_variant();
      std::cout << cbargs[i] << " ";
    }
    std::cout << std::endl;
    const ase_variant *cba = nargs ? (&cbargs[0]) : 0;
    ase_variant r;
    for (int i = 0; i < count; ++i) {
      r = objval.invoke_self(cba, nargs);
    }
    std::cout << "CallbackString: r=" << r << std::endl;
    return r;
  }
  ase_variant ThrowTest(const ase_variant *args, ase_size_type nargs) {
    throw std::runtime_error("ThrowTest throws");
    return ase_variant();
  }
  ase_variant ThrowTest2(const ase_variant *args, ase_size_type nargs) {
    throw ase_new_out_of_memory_exception();
    return ase_variant();
  }

  static const ASE_Component::MethodEntry<asetestsvr> InstanceMethods[];

  int intval;
  ase_string strval;
  ase_variant objval;

};

const ASE_Component::MethodEntry<asetestsvrclass>
asetestsvrclass::ClassMethods[] = {
//  { "StaticSum", &asetestsvrclass::StaticSum },
};

const ASE_Component::MethodEntry<asetestsvr>
asetestsvr::InstanceMethods[] = {
  { "SetInt", &asetestsvr::SetInt },
  { "get_int", &asetestsvr::get_int },
  { "AddInt", &asetestsvr::AddInt },
  { "SetString", &asetestsvr::SetString },
  { "get_string", &asetestsvr::get_string },
  { "AddStrlen", &asetestsvr::AddStrlen },
  { "SetCallback", &asetestsvr::SetCallback },
  { "GetCallback", &asetestsvr::GetCallback },
  { "CallbackInt", &asetestsvr::CallbackInt },
  { "CallbackString", &asetestsvr::CallbackString },
  { "ThrowTest", &asetestsvr::ThrowTest },
  { "ThrowTest2", &asetestsvr::ThrowTest2 },
};

}; // anonymous namespace

extern "C" {

ASE_COMPAT_DLLEXPORT void
ASE_NewVariantFactory(ASE_VariantFactoryPtr& ap_r)
{
  static const ASE_Component::LibraryEntry ents[] = {
    {
      "TestNative",
      ASE_Component::NewClass<asetestsvrclass, asetestsvr, false>
    },
  };
  ASE_Component::NewLibraryFactory(ents, ap_r);
}

};

