
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECPNT_HPP
#define GENTYPE_ASECPNT_HPP

#include <ase/asecpp.hpp>
#include <ase/aseatomic.hpp>
#include <ase/asevtable.hpp>

struct ASE_Component {

 public:

  template <typename T> struct MethodEntry {

    const char *Name;
    ase_variant (T::*Method)(const ase_variant *args, ase_size_type nargs);

  };

  struct Library : public ase_noncopyable {

    virtual ase_unified_engine_ref& GetUnifiedEngineRef() = 0;
    virtual ase_variant GetClass(const ase_string& name) = 0;
    virtual ase_variant GetGlobals() = 0;

   protected:

    Library() { }
    virtual ~Library() { }

  };

  struct NoopImpl {

    ASELIB_DLLEXT ase_variant Noop(const ase_variant *args,
      ase_size_type nargs);

  };

 private:

  struct implpriv;
  struct iibase;
  struct kibase;
  template <typename Tk, typename Ti> struct iimpl;
  template <typename Tk, typename Ti> struct kimpl;

  struct rcbase : public ase_variant_impl_default {

    rcbase() : refcount(1) { }

    ase_atomic_count refcount;

  };

  struct iibase : public rcbase {

    static iibase *get(const ase_variant& self) {
      return static_cast<iibase *>(self.get_rep().p);
    }
    static kibase *getClass(const ase_variant& self) {
      return static_cast<kibase *>(self.get_impl_priv());
    }

    ASELIB_DLLEXT static void add_ref(const ase_variant& self);
    ASELIB_DLLEXT static ase_int get_attributes(const ase_variant& self);
    ASELIB_DLLEXT static ase_string get_string(const ase_variant& self);
    ASELIB_DLLEXT static ase_variant get_property(const ase_variant& self,
      const char *name, ase_size_type namelen);
    ASELIB_DLLEXT static ase_int GetMethodId(const ase_variant& self,
      const char *name, ase_size_type namelen);
    ASELIB_DLLEXT static ase_variant invoke_by_name(const ase_variant& self,
      const char *name, ase_size_type namelen, const ase_variant *args,
      ase_size_type nargs);

  };

  template <typename Tk, typename Ti>
  struct iimpl : public iibase {

    inline iimpl(const Tk& klass, const ase_variant *args,
      ase_size_type nargs);

    static iimpl *get(const ase_variant& self) {
      return static_cast<iimpl *>(
	iibase::get(self));
    }
    static kimpl<Tk, Ti> *getClass(const ase_variant& self) {
      return static_cast<kimpl<Tk, Ti> *>(
	iibase::getClass(self));
    }

    inline static void release(const ase_variant& self);
    inline static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
      const ase_variant *args, ase_size_type nargs);

    Ti& get_value() { return value; }

   private:

    Ti value;

  };

  struct kibase : public rcbase {

    ASELIB_DLLEXT kibase(const char *const *kmethods,
      ase_size_type nkmethods, const char *const *imethods,
      ase_size_type nimethods, const ase_variant_vtable& kvtbl,
      const ase_variant_vtable& ivtbl, bool unlock_engine);
    ASELIB_DLLEXT ~kibase(); /* DCHK */

    static kibase *get(const ase_variant& self) {
      return static_cast<kibase *>(self.get_rep().p);
    }

    ASELIB_DLLEXT static void add_ref(const ase_variant& self);
    ASELIB_DLLEXT static ase_int get_attributes(const ase_variant& self);
    ASELIB_DLLEXT static ase_string get_string(const ase_variant& self);
    ASELIB_DLLEXT static ase_variant get_property(const ase_variant& self,
      const char *name, ase_size_type namelen);
    ASELIB_DLLEXT static ase_int GetMethodId(const ase_variant& self,
      const char *name, ase_size_type namelen);
    ASELIB_DLLEXT static ase_int get_instance_method_id(const ase_variant& self,
      const char *name, ase_size_type namelen);
    ASELIB_DLLEXT static ase_variant invoke_by_name(const ase_variant& self,
      const char *name, ase_size_type namelen, const ase_variant *args,
      ase_size_type nargs);
    ASELIB_DLLEXT static ase_variant GetWeak(const ase_variant& self);

    ASELIB_DLLEXT const ase_variant_vtable *get_klass_vtable() const;
    ASELIB_DLLEXT const ase_variant_vtable *get_instance_vtable() const;

    struct impl_type;
    impl_type *impl;

  };

  template <typename Tk, typename Ti>
  struct kimpl : public kibase {

    static kimpl *get(const ase_variant& self) {
      return static_cast<kimpl *>(
	kibase::get(self));
    }
    inline static Tk *get_class_pointer(const ase_variant& klobj) {
      if (ase_variant::HasSameInvokeById<
	ase_variant_impl< kimpl<Tk, Ti> > >(klobj)) {
	return &(get(klobj)->value);
      }
      ase_variant klobj2 = get_class_from_single_class_library(klobj);
      if (ase_variant::HasSameInvokeById<
	ase_variant_impl< kimpl<Tk, Ti> > >(klobj2)) {
	return &(get(klobj2)->value);
      }
      return 0;
    }
    inline static Ti *get_instance_pointer(const ase_variant& iobj) {
      if (ase_variant::HasSameInvokeById<
	ase_variant_impl< iimpl<Tk, Ti> > >(iobj)) {
	return &(iimpl<Tk, Ti>::get(iobj)->get_value());
      }
      return 0;
    }

    inline static ase_variant create_component(const ase_string& n,
      const char *const *kmethods,
      ase_size_type nkmethods, const char *const *imethods,
      ase_size_type nimethods, Library& lib, bool unlock_engine);

    inline static void release(const ase_variant& self);
    inline static ase_variant invoke_by_id(const ase_variant& self, ase_int id,
      const ase_variant *args, ase_size_type nargs);
    inline static ase_variant invoke_self(const ase_variant& self,
      const ase_variant *args, ase_size_type nargs);

    ase_string name;
    Tk value;

   private:

    inline kimpl(const ase_string& n, const char *const *kmethods,
      ase_size_type nkmethods, const char *const *imethods,
      ase_size_type nimethods, Library& lib, bool unlock_engine);

  };

  template <typename T> struct scoped_arr : public ase_noncopyable {
    scoped_arr(ase_size_type len) : ptr(new T[len > 0 ? len : 1]) { }
    ~scoped_arr() { delete [] ptr; }
    T *const ptr;
  };

  template <typename T> static ase_size_type
  array_length(T& arr) {
    return sizeof(arr) / sizeof(arr[0]);
  }

  ASELIB_DLLEXT static void throw_invalid_method_id();
  ASELIB_DLLEXT static ase_variant get_class_from_single_class_library(
    const ase_variant& val);

 public:

  typedef ase_variant NewClassFunc(const ase_string& n, Library& lib);

  struct LibraryEntry {

    const char *Name;
    NewClassFunc *const NewClass;

  };

  ASELIB_DLLEXT static void NewLibraryFactory(NewClassFunc *const ncfunc,
    ASE_VariantFactoryPtr& ap_r);
    /* single class library */
  ASELIB_DLLEXT static void NewLibraryFactory(const LibraryEntry *ents,
    ase_size_type nents, ASE_VariantFactoryPtr& ap_r);
    /* multiple classes library */
  template <ase_size_type n>
  static void NewLibraryFactory(const LibraryEntry (& ents)[n],
    ASE_VariantFactoryPtr& ap_r) {
    NewLibraryFactory(ents, n, ap_r);
  }

  template <typename Tk, typename Ti, bool UnlockEngine>
  static ase_variant NewClass(const ase_string& n, Library& lib) {
    ase_size_type nkm = array_length(Tk::ClassMethods);
    ase_size_type nim = array_length(Ti::InstanceMethods);
    scoped_arr<const char *> kmn(nkm);
    scoped_arr<const char *> imn(nim);
    for (ase_size_type i = 0; i < nkm; ++i) {
      kmn.ptr[i] = Tk::ClassMethods[i].Name;
    }
    for (ase_size_type i = 0; i < nim; ++i) {
      imn.ptr[i] = Ti::InstanceMethods[i].Name;
    }
    return kimpl<Tk, Ti>::create_component(n,
      &kmn.ptr[0], nkm, &imn.ptr[0], nim, lib, UnlockEngine);
  }

  template <typename Tk, typename Ti>
  static Tk *GetClassPointer(const ase_variant& klobj) {
    return kimpl<Tk, Ti>::get_class_pointer(klobj);
  }
  template <typename Tk, typename Ti>
  static Ti *GetInstancePointer(const ase_variant& iobj) {
    return kimpl<Tk, Ti>::get_instance_pointer(iobj);
  }

};

template <typename Tk, typename Ti>
ASE_Component::iimpl<Tk, Ti>::iimpl(const Tk& klass, const ase_variant *args,
  ase_size_type nargs)
  : value(klass, args, nargs)
{
}

template <typename Tk, typename Ti> void
ASE_Component::iimpl<Tk, Ti>::release(const ase_variant& self)
{
  iimpl *selfp = get(self);
  if ((selfp->refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "ASECompInst");
    delete selfp;
  }
}

template <typename Tk, typename Ti> ase_variant
ASE_Component::iimpl<Tk, Ti>::invoke_by_id(const ase_variant& self,
  ase_int id, const ase_variant *args, ase_size_type nargs)
{
  iimpl *selfp = get(self);
  if (id < 0 ||
    static_cast<ase_size_type>(id) >= array_length(Ti::InstanceMethods)) {
    throw_invalid_method_id();
  }
  return (selfp->value.*(Ti::InstanceMethods[id].Method))(args, nargs);
}

template <typename Tk, typename Ti> ase_variant
ASE_Component::kimpl<Tk, Ti>::create_component(const ase_string& n,
  const char *const *kmethods, ase_size_type nkmethods,
  const char *const *imethods, ase_size_type nimethods,
  Library& lib, bool unlock_engine)
{
  kibase *p =
    new kimpl<Tk, Ti>(n, kmethods, nkmethods, imethods, nimethods,
      lib, unlock_engine);
  return ase_variant::create_object(p->get_klass_vtable(), p,
    "ASECompKlass"); /* nothrow */
}

template <typename Tk, typename Ti> void
ASE_Component::kimpl<Tk, Ti>::release(const ase_variant& self)
{
  kimpl *selfp = get(self);
  if ((selfp->refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "ASECompKlass");
    delete selfp;
  }
}

template <typename Tk, typename Ti> ase_variant
ASE_Component::kimpl<Tk, Ti>::invoke_by_id(
  const ase_variant& self, ase_int id, const ase_variant *args,
  ase_size_type nargs)
{
  kimpl *selfp = get(self);
  if (id < 0 ||
    static_cast<ase_size_type>(id) >= array_length(Tk::ClassMethods)) {
    throw_invalid_method_id();
  }
  return (selfp->value.*(Tk::ClassMethods[id].Method))(args, nargs);
}

template <typename Tk, typename Ti> ase_variant
ASE_Component::kimpl<Tk, Ti>::invoke_self(
  const ase_variant& self, const ase_variant *args, ase_size_type nargs)
{
  kimpl *selfp = get(self);
  iibase *p =
    new iimpl<Tk, Ti>(selfp->value, args, nargs);
  return ase_variant::create_object(selfp->get_instance_vtable(), p,
    "ASECompInst"); /* nothrow */
}

template <typename Tk, typename Ti>
ASE_Component::kimpl<Tk, Ti>::kimpl(const ase_string& n,
  const char *const *kmethods, ase_size_type nkmethods,
  const char *const *imethods, ase_size_type nimethods,
  Library& lib, bool unlock_engine)
    : kibase(
	kmethods, nkmethods, imethods, nimethods,
	ase_variant_impl< kimpl<Tk, Ti> >::vtable,
	ase_variant_impl< iimpl<Tk, Ti> >::vtable,
	unlock_engine),
      name(n), value(n, lib)
{
}

#endif

