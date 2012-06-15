
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asebindmethod.hpp>
#include <ase/asecast.hpp>
#include <ase/asealloca.hpp>
#include <ase/asealgo.hpp>

#include "asejni_jobjpr.hpp"
#include "asejni_priv.hpp"
#include "asejni_classinfo.hpp"

#include <algorithm>

#define DBG(x)
#define DBG_SELF(x)
#define DBG_REWIND(x)
#define DBG_ARR(x)
#define DBG_DYNTYPE(x)
#define DBG_PROXY(x)
#define DBG_PROP(x)
#define DBG_MARR(x)
#define DBG_LATE(x)
#define DBG_GETSTR(x)
#define DBG_GREF(x)
#define DBG_SET(x)
#define DBG_LATEGET(x)
#define DBG_MID(x)
#define DBG_FINDMETHOD(x)
#define DBG_EQ(x)
#define DBG_INV(x)

namespace asejni {

ase_jni_variant_impl_jobject::ase_jni_variant_impl_jobject(JNIEnv *env,
  jobject obj, ase_jni_classinfo *ci)
  : refcount(1), object(0), classinfo(ci)
{
  if (obj) {
    object = util::newGlobalRef(env, obj);
    DBG_GREF(fprintf(stderr, "JNI new globalref %p\n", object));
  }
}

ase_jni_script_engine *
ase_jni_variant_impl_jobject::getContext(const ase_variant& v)
{
  ase_jni_script_engine **scp = static_cast<ase_jni_script_engine **>(
    v.get_impl_priv());
  return *scp;
}

void
ase_jni_variant_impl_jobject::add_ref(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  jv->refcount++;
}

void
ase_jni_variant_impl_jobject::release(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  if ((jv->refcount--) == 1) {
    ase_jni_script_engine *sc = getContext(v);
    JNIEnv *env = sc->getEnv();
    env->DeleteGlobalRef(jv->object);
    ase_variant::debug_on_destroy_object(v, "JNIJavaObject");
    delete jv;
  }
}

ase_int
ase_jni_variant_impl_jobject::get_attributes(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetAttributes(v);
  }
  if (ci.is_latebind) {
    return lateGetAttributes(v);
  }
  return ase_attrib_mutable;
}

bool
ase_jni_variant_impl_jobject::get_boolean(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetBoolean(v);
  }
  return false;
}

ase_int
ase_jni_variant_impl_jobject::get_int(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetInt(v);
  }
  return 0;
}

ase_long
ase_jni_variant_impl_jobject::get_long(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetLong(v);
  }
  return 0;
}

double
ase_jni_variant_impl_jobject::get_double(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetDouble(v);
  }
  return 0.0;
}

ase_string
ase_jni_variant_impl_jobject::get_string(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetString(v);
  }
  if (jv->object == 0)  {
    return ase_string("[ASEJNI Class]");
  }
  DBG_GETSTR(fprintf(stderr, "JNI vimpl get_string begin\n"));
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jvalue cargs[1];
  cargs[0].l = jv->object;
  util::localref<jobject> sobj(env, sc, env->CallStaticObjectMethodA(
    sc->getUtilKlass(), sc->getMIDObjToStr(), cargs));
  ase_string r = util::to_ase_string(env, sc,
    static_cast<jstring>(sobj.get()));
  DBG_GETSTR(fprintf(stderr, "JNI vimpl get_string: %s\n", r.data()));
  return r;
}

bool
ase_jni_variant_impl_jobject::is_same_object(const ase_variant& v,
  const ase_variant& value)
{
  ase_jni_variant_impl_jobject *jvx = get(v);
  jobject objx = jvx->get_object_or_clobj();
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  if (!is_jobject_or_clobj(sc, value)) {
    return false;
  }
  ase_jni_variant_impl_jobject *jvy = get(value);
  jobject objy = jvy->get_object_or_clobj();
  jboolean r = env->IsSameObject(objx, objy);
  DBG_EQ(fprintf(stderr, "is_same_object: %d\n", r));
  return (r == JNI_TRUE);
}

struct javaget_rewind_hook {

  javaget_rewind_hook(JNIEnv *e, ase_jni_script_engine *sc,
    ase_jni_classinfo& retci, jvalue& jrval)
    : env(e), scr(sc), retvalci(retci), jretval(jrval) {
    jretval.l = 0;
  }

  ~javaget_rewind_hook() /* DCHK */ {
    if (retvalci.tinfo->is_objtype) {
      if (jretval.l) {
	DBG_REWIND(fprintf(stderr, "GREWIND localref ret %p\n", jretval.l));
	env->DeleteLocalRef(jretval.l);
      }
    }
  }

  ase_variant conv_retval() {
    const bool examine_dyntype = true;
    return retvalci.tinfo->to_ase(env, scr, jretval, &retvalci,
      examine_dyntype);
  }

 private:

  JNIEnv *env;
  ase_jni_script_engine *scr;
  ase_jni_classinfo& retvalci;
  jvalue& jretval;

  /* noncopyable */
  javaget_rewind_hook(const javaget_rewind_hook& x);
  javaget_rewind_hook& operator =(const javaget_rewind_hook& x);

};

ase_variant
ase_jni_variant_impl_jobject::get_property(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!is_static) {
    if (ci.is_reflectproxy) {
      return proxyGetProperty(v, name, namelen);
    }
    if (ci.is_latebind) {
      return lateGetProperty(v, name, namelen);
    }
  }
  const bool is_array = (ci.component_kinfo != 0);
  if (is_array && ase_string::equals(name, namelen, "length")) {
    return get_length(v);
  }
  ase_jni_script_engine *sc = getContext(v);
  ase_string name8(name, namelen);
  DBG_PROP(fprintf(stderr, "PROP get %s\n", name8.data()));
  ase_jni_classinfo::props_type& ps = is_static ? ci.static_props : ci.props;
  ase_jni_classinfo::props_type::iterator iter = ps.find(name8);
  if (iter == ps.end()) {
    DBG_PROP(fprintf(stderr, "PROP notfound %s\n", name8.data()));
    ase_int id = v.get_method_id(name, namelen);
    if (id >= 0) {
      return ase_new_variant_bind_method_id(v, id);
    }
    return ase_variant();
  }
  ase_jni_propinfo& pi = *(iter->second);
  JNIEnv *env = sc->getEnv();
  sc->getClassMap().resolve_propinfo_synchronized(pi, env, sc);
  if (!pi.field.get()) {
    DBG_PROP(fprintf(stderr, "PROP notfound2 %s\n", name8.data()));
    ase_int id = v.get_method_id(name, namelen);
    if (id >= 0) {
      return ase_new_variant_bind_method_id(v, id);
    }
    return ase_variant();
  }
  DBG_PROP(fprintf(stderr, "PROP found %s\n", name8.data()));
  ase_jni_fieldinfo& fi = *(pi.field.get());
  ase_jni_classinfo& cci = *fi.fldtype;

  jvalue jrval;
  javaget_rewind_hook h(env, sc, cci, jrval);
  if (is_static) {
    cci.tinfo->getfldstatic(env, ci.klass.get(), fi.fid, jrval);
  } else {
    cci.tinfo->getfld(env, jv->object, fi.fid, jrval);
  }
  util::checkJavaException(env, sc, "JNI: getfld");
  return h.conv_retval();
}

struct javaset_rewind_hook {

  javaset_rewind_hook(JNIEnv *e, ase_jni_script_engine *sc,
    ase_jni_classinfo& aci, jvalue& ja)
    : env(e), scr(sc), argci(aci), jarg(ja), is_localref(false) {
    jarg.l = 0;
  }

  ~javaset_rewind_hook() /* DCHK */ {
    if (is_localref && (jarg.l != 0)) {
      DBG_REWIND(fprintf(stderr, "SREWIND localref arg %p\n", jarg.l));
      env->DeleteLocalRef(jarg.l);
    }
  }

  void conv_arg(const ase_variant& arg) {
    argci.tinfo->to_java(env, scr, arg, jarg, is_localref);
  }

 private:

  JNIEnv *env;
  ase_jni_script_engine *scr;
  ase_jni_classinfo& argci;
  jvalue& jarg;
  bool is_localref;

  /* noncopyable */
  javaset_rewind_hook(const javaset_rewind_hook& x);
  javaset_rewind_hook& operator =(const javaset_rewind_hook& x);

};

void
ase_jni_variant_impl_jobject::set_property(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxySetProperty(v, name, namelen, value);
  }
  if (ci.is_latebind) {
    return lateSetProperty(v, name, namelen, value);
  }
  ase_jni_script_engine *sc = getContext(v);
  ase_string name8(name, namelen);
  DBG_PROP(fprintf(stderr, "SPROP set %s\n", name8.data()));
  const bool is_static = (jv->object == 0);
  ase_jni_classinfo::props_type& ps = is_static ? ci.static_props : ci.props;
  ase_jni_classinfo::props_type::iterator iter = ps.find(name8);
  if (iter == ps.end()) {
    DBG_PROP(fprintf(stderr, "SPROP notfound %s\n", name8.data()));
    return;
  }
  ase_jni_propinfo& pi = *(iter->second);
  JNIEnv *env = sc->getEnv();
  sc->getClassMap().resolve_propinfo_synchronized(pi, env, sc);
  if (!pi.field.get()) {
    DBG_PROP(fprintf(stderr, "SPROP notfound2 %s\n", name8.data()));
    return;
  }
  DBG_PROP(fprintf(stderr, "SPROP found %s\n", name8.data()));
  ase_jni_fieldinfo& fi = *(pi.field.get());
  ase_jni_classinfo& cci = *fi.fldtype;

  jvalue jarg;
  javaset_rewind_hook h(env, sc, cci, jarg);
  h.conv_arg(value);
  if (is_static) {
    cci.tinfo->setfldstatic(env, ci.klass.get(), fi.fid, jarg);
  } else {
    cci.tinfo->setfld(env, jv->object, fi.fid, jarg);
  }
  util::checkJavaException(env, sc, "JNI: setfld");
}

void
ase_jni_variant_impl_jobject::del_property(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyDelProperty(v, name, namelen);
  }
  if (ci.is_latebind) {
    return lateDelProperty(v, name, namelen);
  }
}

ase_variant
ase_jni_variant_impl_jobject::get_element(const ase_variant& v, ase_int idx)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetElement(v, idx);
  }
  if (ci.is_latebind) {
    return lateGetElement(v, idx);
  }
  ase_jni_script_engine *sc = getContext(v);
  const bool is_static = (jv->object == 0);
  const bool is_array = (ci.component_kinfo != 0);
  if (is_static || !is_array) {
    return ase_variant();
  }
  ase_jni_classinfo& cci = *ci.component_kinfo;
  jarray arr = static_cast<jarray>(jv->object);
  JNIEnv *env = sc->getEnv();

  jvalue jrval;
  javaget_rewind_hook h(env, sc, cci, jrval);
  jsize pos = ase_numeric_cast<jsize>(idx);
  cci.tinfo->get_elem(env, arr, pos, jrval);
  util::checkJavaException(env, sc, "JNI: getelem");
  return h.conv_retval();
}

void
ase_jni_variant_impl_jobject::set_element(const ase_variant& v, ase_int idx,
  const ase_variant& value)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxySetElement(v, idx, value);
  }
  if (ci.is_latebind) {
    return lateSetElement(v, idx, value);
  }
  ase_jni_script_engine *sc = getContext(v);
  const bool is_static = (jv->object == 0);
  const bool is_array = (ci.component_kinfo != 0);
  if (is_static || !is_array) {
    return;
  }
  ase_jni_classinfo& cci = *ci.component_kinfo;
  jarray arr = static_cast<jarray>(jv->object);
  JNIEnv *env = sc->getEnv();

  jvalue jarg;
  javaset_rewind_hook h(env, sc, cci, jarg);
  h.conv_arg(value);
  jsize pos = ase_numeric_cast<jsize>(idx);
  cci.tinfo->set_elem(env, arr, pos, jarg);
  util::checkJavaException(env, sc, "JNI: setelem");
}

ase_int
ase_jni_variant_impl_jobject::get_length(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetLength(v);
  }
  if (ci.is_latebind) {
    return lateGetLength(v);
  }
  const bool is_static = (jv->object == 0);
  const bool is_array = (ci.component_kinfo != 0);
  if (is_static || !is_array) {
    return 0;
  }
  jarray arr = static_cast<jarray>(jv->object);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jsize r = env->GetArrayLength(arr); /* NOTHROW */
  return ase_numeric_cast<ase_int>(r);
}

void
ase_jni_variant_impl_jobject::set_length(const ase_variant& v, ase_int len)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxySetLength(v, len);
  }
  if (ci.is_latebind) {
    return lateSetLength(v, len);
  }
}

ase_variant
ase_jni_variant_impl_jobject::invoke_by_name(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!is_static) {
    if (ci.is_reflectproxy) {
      return proxyInvokeByName(v, name, namelen, args, nargs);
    }
    if (ci.is_latebind) {
      return lateInvokeByName(v, name, namelen, args, nargs);
    }
  }
  ase_int id = v.get_method_id(name, namelen);
  if (id < 0) {
    ase_string s = "JNI: method not found: " + ase_string(name, namelen);
    ase_throw_invalid_arg(s.data());
  }
  return invoke_by_id(v, id, args, nargs);
}

namespace {

ase_vtype
categorize_vtype(ase_vtype t)
{
  if (t == ase_vtype_int || t == ase_vtype_double) {
    return ase_vtype_long;
  }
  return t;
}

ase_jni_funcinfo *
find_func_overloaded(JNIEnv *env, ase_jni_script_engine *sc, 
  const util::ptrvector<ase_jni_funcinfo *>& funcs, const ase_variant *args,
  ase_size_type nargs)
{
  util::ptrvector<ase_jni_funcinfo *>::size_type i;
  for (i = 0; i < funcs.size(); ++i) {
    ase_jni_funcinfo& fi = *funcs[i];
    if (fi.argtypes.size() != nargs) {
      continue;
    }
    ase_jni_funcinfo::argtypes_type::size_type j;
    for (j = 0; j < fi.argtypes.size(); ++j) {
      const ase_variant& arg = args[j];
      ase_jni_classinfo& cinfo = *(fi.argtypes[j]);
      const ase_vtype paramvt = categorize_vtype(cinfo.tinfo->asevtype);
      const ase_vtype argvt = categorize_vtype(arg.get_type());
      if (paramvt != argvt) {
	if ((paramvt == ase_vtype_object || paramvt == ase_vtype_string)
	  && argvt == ase_vtype_void) {
	  /* allow null */
	  continue;
	}
	if (paramvt == ase_vtype_object && argvt == ase_vtype_string) {
	  /* can cast string from param? */
	  if (env->IsAssignableFrom(sc->getStringKlass(),
	    cinfo.klass.get())) {
	    /* allow string */
	    continue;
	  }
	}
	/* does not match */
	break;
      }
      if (paramvt == ase_vtype_object) {
	jclass paramkl = cinfo.klass.get();
	if (!ase_jni_variant_impl_jobject::is_jobject_or_clobj(sc, arg)) {
	  /* not a java object */
	  break;
	}
	jobject aobj =
	  ase_jni_variant_impl_jobject::get(arg)->get_object_or_clobj();
	if (!env->IsInstanceOf(aobj, paramkl)) {
	  /* does not match */
	  break;
	}
      }
    }
    if (j == fi.argtypes.size()) {
      /* match */
      return &fi;
    }
  }
  return 0;
}

struct javacall_rewind_hook {

  javacall_rewind_hook(JNIEnv *e, ase_jni_script_engine *sc,
    ase_jni_funcinfo& fi, jvalue *ja, bool *islc, ase_size_type na,
    ase_jni_classinfo& retci, jvalue& jrval)
    : env(e), scr(sc), finfo(fi), jargs(ja), islocalref(islc), nargs(na),
      retvalci(retci), jretval(jrval) {
    for (ase_size_type i = 0; i < nargs; ++i) {
      jargs[i].l = 0;
      islocalref[i] = false;
    }
    jretval.l = 0;
  }

  ~javacall_rewind_hook() /* DCHK */ {
    for (ase_size_type i = 0; i < nargs; ++i) {
      if (islocalref[i] && (jargs[i].l != 0)) {
	DBG_REWIND(fprintf(stderr, "CREWIND localref arg %p\n", jargs[i].l));
	env->DeleteLocalRef(jargs[i].l);
      }
    }
    if (retvalci.tinfo->is_objtype) {
      if (jretval.l) {
	DBG_REWIND(fprintf(stderr, "CREWIND localref ret %p\n", jretval.l));
	env->DeleteLocalRef(jretval.l);
      }
    }
  }

  void conv_args(const ase_variant *args) {
    for (ase_size_type i = 0; i < nargs; ++i) {
      finfo.argtypes[i]->tinfo->to_java(env, scr, args[i], jargs[i],
	islocalref[i]);
    }
  }

  ase_variant conv_retval(bool examine_dyntype) {
    return retvalci.tinfo->to_ase(env, scr, jretval, &retvalci,
      examine_dyntype);
  }

 private:

  JNIEnv *env;
  ase_jni_script_engine *scr;
  ase_jni_funcinfo& finfo;
  jvalue *const jargs;
  bool *const islocalref;
  const ase_size_type nargs;
  ase_jni_classinfo& retvalci;
  jvalue& jretval;

  /* noncopyable */
  javacall_rewind_hook(const javacall_rewind_hook& x);
  javacall_rewind_hook& operator =(const javacall_rewind_hook& x);

};

}; // anonymous namespace

ase_variant
ase_jni_variant_impl_jobject::invoke_by_id(const ase_variant& v, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  const ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!is_static) {
    if (ci.is_reflectproxy) {
      return proxyInvokeById(v, id, args, nargs);
    }
    if (ci.is_latebind) {
      return lateInvokeById(v, id, args, nargs);
    }
  }
  ase_jni_script_engine *sc = getContext(v);
  ase_jni_propinfo *piptr = 0;
  if (is_static) {
    if (id < 0 || static_cast<ase_size_type>(id) >= ci.static_proparr.size()) {
      ase_throw_invalid_arg("JNI: method-id not found");
    }
    piptr = ci.static_proparr[id];
  } else {
    if (id < 0 || static_cast<ase_size_type>(id) >= ci.proparr.size()) {
      ase_throw_invalid_arg("JNI: method-id not found");
    }
    piptr = ci.proparr[id];
  }
  ase_jni_propinfo& pi = *piptr;
  JNIEnv *env = sc->getEnv();
  sc->getClassMap().resolve_propinfo_synchronized(pi, env, sc);
  ase_jni_funcinfo *fiptr = 0;
  bool is_classmeta = false;
  if (pi.methods.size() < 1) {
    if (pi.is_builtinmethod) {
      switch (pi.class_or_builtin_method_id) {
      case 0:
	/* NewInstance */
	return invoke_self(v, args, nargs);
      case 1:
	/* NewArray */
	return create_array_multi(v, args, nargs);
      case 2:
	/* NewArrayValue */
	return create_array_value(v, args, nargs);
      default:
	break;
      }
    } else if (pi.is_classmethod) {
      ase_jni_classinfo *jlc_ci = sc->get_jlclass_classinfo();
      ase_jni_propinfo *jlc_pi =
	jlc_ci->proparr[pi.class_or_builtin_method_id];
      sc->getClassMap().resolve_propinfo_synchronized(*jlc_pi, env, sc);
      if (jlc_pi->methods.size() > 1) {
	fiptr = find_func_overloaded(env, sc, jlc_pi->methods, args, nargs);
	if (fiptr == 0) {
	  ase_throw_invalid_arg("JNI: class metamethod not found");
	}
      } else if (jlc_pi->methods.size() == 1) {
	fiptr = jlc_pi->methods[0];
	if (fiptr->argtypes.size() != nargs) {
	  ase_throw_invalid_arg("JNI: method not found");
	}
      } else {
	ase_throw_invalid_arg("JNI: class metamethod not found");
      }
      is_classmeta = true;
    } else {
      ase_throw_invalid_arg("JNI: method not found");
    }
  } else if (pi.methods.size() > 1) {
    fiptr = find_func_overloaded(env, sc, pi.methods, args, nargs);
    if (fiptr == 0) {
      ase_throw_invalid_arg("JNI: method not found");
    }
  } else {
    fiptr = pi.methods[0];
    if (fiptr->argtypes.size() != nargs) {
      ase_throw_invalid_arg("JNI: method not found");
    }
  }
  DBG_INV(fprintf(stderr, "invoke_by_id fi=%p\n", fiptr));
  ase_jni_funcinfo& fi = *fiptr;

  ASE_ALLOCA(jvalue, nargs, jargs);
  jvalue jrval;
  ASE_ALLOCA(bool, nargs, islocalref);
  javacall_rewind_hook h(env, sc, fi, jargs, islocalref, nargs, *fi.rettype,
    jrval);
  h.conv_args(args);
  if (is_classmeta) {
    ase_jni_classinfo *jlc_ci = sc->get_jlclass_classinfo();
    jlc_ci->tinfo->invoke(env, ci.klass.get(), fi.mid, jargs, jrval);
  } else if (is_static) {
    ci.tinfo->invokestatic(env, ci.klass.get(), fi.mid, jargs, jrval);
  } else {
    ci.tinfo->invoke(env, jv->object, fi.mid, jargs, jrval);
  }
  DBG(fprintf(stderr, "invoke_by_id done\n"));
  util::checkJavaException(env, sc, "JNI: invoke");
  DBG(fprintf(stderr, "invoke_by_id noexp\n"));
  const bool examine_dyntype = true;
  return h.conv_retval(examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::invoke_self(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  ase_jni_script_engine *sc = getContext(v);
  const bool is_static = (jv->object == 0);
  const bool is_array = (ci.component_kinfo != 0);
  if (!is_static) {
    if (ci.is_reflectproxy) {
      return proxyInvokeSelf(v, args, nargs);
    }
    if (ci.is_latebind) {
      return lateInvokeSelf(v, args, nargs);
    }
    ase_string s = "JNI: not a function: " + ci.name;
    ase_throw_type_mismatch(s.data());
  }
  if (is_array) {
    return create_array(v, args, nargs);
  }
  if (ci.is_interface) {
    return create_proxy(v, args, nargs);
  }
  if (ci.is_latebind) {
    /* prepend extglobals as the first argument */
    ASE_FASTVECTOR(ase_variant, nargs + 1, altargs);
    altargs[0] = sc->get_globals_var();
    for (ase_size_type i = 0; i < nargs; ++i) {
      altargs[i + 1] = args[i];
    }
    return InvokeSelfInternal(sc, jv, ci, ASE_FASTVECTOR_GETPTR(altargs),
      nargs + 1);
  } else {
    return InvokeSelfInternal(sc, jv, ci, args, nargs);
  }
}

ase_variant
ase_jni_variant_impl_jobject::InvokeSelfInternal(ase_jni_script_engine *sc,
  ase_jni_variant_impl_jobject *jv, ase_jni_classinfo& ci,
  const ase_variant *args, ase_size_type nargs)
{
  DBG_SELF(fprintf(stderr, "invoke_self class=%s\n", ci.name.c_str()));
  JNIEnv *env = sc->getEnv();
  sc->getClassMap().resolve_constr_synchronized(ci, env, sc);
  ase_jni_funcinfo *fiptr = 0;
  if (ci.constructors.size() == 1) {
    if (ci.constructors[0]->argtypes.size() != nargs) {
      ase_throw_invalid_arg("JNI: constructor not found");
    }
    fiptr = ci.constructors[0];
  } else {
    fiptr = find_func_overloaded(env, sc, ci.constructors, args, nargs);
  }
  if (!fiptr) {
    ase_throw_invalid_arg("JNI: constructor not found");
  }
  ase_jni_funcinfo& fi = *fiptr; /* fi is a constructor */
  DBG_SELF(fprintf(stderr, "invoke_self fi=%p\n", fiptr));

  ASE_ALLOCA(jvalue, nargs, jargs);
  jvalue jrval;
  ASE_ALLOCA(bool, nargs, islocalref);
  javacall_rewind_hook h(env, sc, fi, jargs, islocalref, nargs, ci, jrval);
  h.conv_args(args);
  jrval.l = env->NewObjectA(ci.klass.get(), fi.mid, jargs);
  DBG_SELF(fprintf(stderr, "invoke_self newobj=%p\n", jrval.l));
  util::checkJavaException(env, sc, "JNI: invoke");
  const bool examine_dyntype = false; /* no need to examine dyntype */
  return h.conv_retval(examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::create_array(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  /* 'v' itself must be an array class */
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  const bool is_array = (ci.component_kinfo != 0);
  if (!is_array || !is_static) {
    return ase_variant();
  }
  ase_jni_script_engine *sc = getContext(v);
  ase_jni_classinfo& elemci = *ci.component_kinfo;
  ase_check_num_args(nargs, 1, "JNI: createarray: wrong number of arguments");
  ase_int al = args[0].get_int();
  if (al < 0) {
    ase_throw_invalid_arg("JNI: arg #1 must be a positive integer");
  }
  DBG_ARR(fprintf(stderr, "CREATEARR len=%d\n", al));
  jsize alen = ase_numeric_cast<jsize>(al);
  JNIEnv *env = sc->getEnv();

  util::localref<jarray> arr(env, sc,
    elemci.tinfo->create_arr(env, elemci.klass.get(), alen));
  const bool examine_dyntype = false;
  return ase_jni_jobject_new(env, sc, &ci, arr.get(), examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::create_array_multi(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!is_static) {
    return ase_variant();
  }
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jsize ndims = ase_numeric_cast<jsize>(nargs);
  DBG_MARR(fprintf(stderr, "ArrayMulti: dim=%d\n", ndims));
  util::localref<jintArray> dims(env, sc, env->NewIntArray(ndims));
  for (jsize i = 0; i < ndims; ++i) {
    jint elemval = args[i].get_int();
    env->SetIntArrayRegion(dims.get(), i, 1, &elemval);
    util::checkJavaException(env, sc, "JNI: createarraymulti");
  }
  jvalue cargs[2];
  cargs[0].l = ci.klass.get();
  cargs[1].l = dims.get();
  util::localref<jobject> aobj(env, sc, env->CallStaticObjectMethodA(
    sc->getArrayKlass(), sc->getMIDANewInstance(), cargs));
  const bool examine_dyntype = true;
  return ase_jni_jobject_new(env, sc, 0, aobj.get(), examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::create_array_value(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!is_static) {
    return ase_variant();
  }
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jsize alen = ase_numeric_cast<jsize>(nargs);
  util::localref<jarray> arr(env, sc,
    ci.tinfo->create_arr(env, ci.klass.get(), alen));
  for (jsize i = 0; i < alen; ++i) {
    /* see set_element also. */
    jvalue jarg;
    javaset_rewind_hook h(env, sc, ci, jarg);
    h.conv_arg(args[i]);
    ci.tinfo->set_elem(env, arr.get(), i, jarg);
    util::checkJavaException(env, sc, "JNI: setelem");
  }
  const bool examine_dyntype = true;
  return ase_jni_jobject_new(env, sc, 0, arr.get(), examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::create_proxy(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (!ci.is_interface || !is_static) {
    return ase_variant();
  }
  ase_check_num_args(nargs, 1, "JNI: createproxy: wrong number of arguments");
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jobject> probj(env, sc, ase_jni_jniproxy_new(env, sc, ci,
    args[0]));
  const bool examine_dyntype = true;
  return ase_jni_jobject_new(env, sc, &ci, probj.get(), examine_dyntype);
}

ase_variant
ase_jni_variant_impl_jobject::get_enumerator(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyGetEnumerator(v);
  }
  if (ci.is_latebind) {
    return lateGetEnumerator(v);
  }
  return ase_variant();
}

ase_string
ase_jni_variant_impl_jobject::enum_next(const ase_variant& v, bool& hasnext_r)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  if (ci.is_reflectproxy) {
    return proxyEnumNext(v, hasnext_r);
  }
  if (ci.is_latebind) {
    return lateEnumNext(v, hasnext_r);
  }
  return ase_string();
}

ase_variant
ase_jni_variant_impl_jobject::getProxyWrapped(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_classinfo& ci = *jv->classinfo;
  const bool is_static = (jv->object == 0);
  if (is_static || !ci.is_reflectproxy) {
    return ase_variant();
  }
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jvalue cargs[1];
  cargs[0].l = jv->object;
  util::localref<jobject> ih(env, sc, env->CallStaticObjectMethodA(
    sc->getRPrxKlass(), sc->getMIDGetInvocationHandler(), cargs));
  jlong ihpriv = env->GetLongField(ih.get(), sc->getFIDNPRXPriv());
  util::checkJavaException(env, sc, "JNI: ase proxy priv");
  ase_jni_jniproxy_priv *priv =
    reinterpret_cast<ase_jni_jniproxy_priv *>(ihpriv);
  if (!priv) {
    return ase_variant();
  }
  return priv->value.get();
}

ase_int
ase_jni_variant_impl_jobject::proxyGetAttributes(const ase_variant& v)
{
  return getProxyWrapped(v).get_attributes();
}

bool
ase_jni_variant_impl_jobject::proxyGetBoolean(const ase_variant& v)
{
  return getProxyWrapped(v).get_boolean();
}

ase_int
ase_jni_variant_impl_jobject::proxyGetInt(const ase_variant& v)
{
  return getProxyWrapped(v).get_int();
}

ase_long
ase_jni_variant_impl_jobject::proxyGetLong(const ase_variant& v)
{
  return getProxyWrapped(v).get_long();
}

double
ase_jni_variant_impl_jobject::proxyGetDouble(const ase_variant& v)
{
  return getProxyWrapped(v).get_double();
}

ase_string
ase_jni_variant_impl_jobject::proxyGetString(const ase_variant& v)
{
  return getProxyWrapped(v).get_string();
}

ase_variant
ase_jni_variant_impl_jobject::proxyGetProperty(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  ase_variant pr = getProxyWrapped(v);
  DBG_PROXY(fprintf(stderr, "proxyGetProperty %s\n", s.data()));
  if (ase_string::equals(name, namelen, "__wrapped__")) {
    return pr;
  }
  return pr.get_property(name, namelen);
}

void
ase_jni_variant_impl_jobject::proxySetProperty(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  return getProxyWrapped(v).set_property(name, namelen, value);
}

void
ase_jni_variant_impl_jobject::proxyDelProperty(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  return getProxyWrapped(v).del_property(name, namelen);
}

ase_variant
ase_jni_variant_impl_jobject::proxyGetElement(const ase_variant& v,
  ase_int idx)
{
  return getProxyWrapped(v).get_element(idx);
}

void
ase_jni_variant_impl_jobject::proxySetElement(const ase_variant& v,
  ase_int idx, const ase_variant& value)
{
  return getProxyWrapped(v).set_element(idx, value);
}

ase_int
ase_jni_variant_impl_jobject::proxyGetLength(const ase_variant& v)
{
  return getProxyWrapped(v).get_length();
}

void
ase_jni_variant_impl_jobject::proxySetLength(const ase_variant& v,
  ase_int len)
{
  return getProxyWrapped(v).set_length(len);
}

ase_variant
ase_jni_variant_impl_jobject::proxyInvokeByName(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  return getProxyWrapped(v).invoke_by_name(name, namelen, args, nargs);
}

ase_variant
ase_jni_variant_impl_jobject::proxyInvokeById(const ase_variant& v,
  ase_int id, const ase_variant *args, ase_size_type nargs)
{
  return getProxyWrapped(v).invoke_by_id(id, args, nargs);
}

ase_variant
ase_jni_variant_impl_jobject::proxyInvokeSelf(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  return getProxyWrapped(v).invoke_self(args, nargs);
}

ase_variant
ase_jni_variant_impl_jobject::proxyGetEnumerator(const ase_variant& v)
{
  return getProxyWrapped(v).get_enumerator();
}

ase_string
ase_jni_variant_impl_jobject::proxyEnumNext(const ase_variant& v,
  bool& hasnext_r)
{
  return getProxyWrapped(v).enum_next(hasnext_r);
}

ase_int
ase_jni_variant_impl_jobject::lateGetAttributes(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jint r = env->CallIntMethod(jv->object, sc->getMIDLBGetAttributes());
  util::checkJavaException(env, sc, "JNI: lategetattributes");
  return ase_numeric_cast<ase_int>(r);
}

bool
ase_jni_variant_impl_jobject::lateGetBoolean(const ase_variant& v)
{
  return false;
}

ase_int
ase_jni_variant_impl_jobject::lateGetInt(const ase_variant& v)
{
  return 0;
}

ase_long
ase_jni_variant_impl_jobject::lateGetLong(const ase_variant& v)
{
  return 0;
}

double
ase_jni_variant_impl_jobject::lateGetDouble(const ase_variant& v)
{
  return 0.0;
}

ase_string
ase_jni_variant_impl_jobject::lateGetString(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jstring> jr(env, sc, static_cast<jstring>(
    env->CallObjectMethod(jv->object, sc->getMIDLBGetString())));
  return util::to_ase_string(env, sc, jr.get());
}

ase_variant
ase_jni_variant_impl_jobject::lateGetProperty(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  DBG_LATEGET(fprintf(stderr, "lateGetProperty name=%s\n",
    ase_string::newUTF16(name, namelen).data()));
  util::localref<jstring> jname(env, sc, util::to_java_string(env, name,
    namelen));
  jvalue cargs[1];
  cargs[0].l = jname.get();
  util::localref<jobject> jr(env, sc, env->CallObjectMethodA(jv->object,
    sc->getMIDLBGetProperty(), cargs));
  ase_variant r = util::to_ase_variant_from_jobject(env, sc, jr.get());
  DBG_LATEGET(fprintf(stderr,
    "lateGetProperty done type=%d val=%p rep=%p str=%s\n",
    r.get_type(), jr.get(), r.get_rep().p, r.get_string().data()));
  return r;
}

void
ase_jni_variant_impl_jobject::lateSetProperty(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jstring> jname(env, sc, util::to_java_string(env, name,
    namelen));
  util::jobjectref jval(env);
  util::to_jobject_from_ase_variant(env, sc, value, jval);
  jvalue cargs[2];
  cargs[0].l = jname.get();
  cargs[1].l = jval.get();
  DBG_SET(fprintf(stderr, "lateSetProperty type=%d val=%p rep=%p\n",
    value.get_type(), jval.get(), value.get_rep().p));
  env->CallVoidMethodA(jv->object, sc->getMIDLBSetProperty(), cargs);
  util::checkJavaException(env, sc, "JNI: latesetproperty");
}

void
ase_jni_variant_impl_jobject::lateDelProperty(const ase_variant& v,
  const char *name, ase_size_type namelen)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jstring> jname(env, sc, util::to_java_string(env, name,
    namelen));
  jvalue cargs[1];
  cargs[0].l = jname.get();
  env->CallVoidMethodA(jv->object, sc->getMIDLBDelProperty(), cargs);
  util::checkJavaException(env, sc, "JNI: latedelproperty");
}

ase_variant
ase_jni_variant_impl_jobject::lateGetElement(const ase_variant& v,
  ase_int idx)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jint jidx = ase_numeric_cast<jint>(idx);
  jvalue cargs[1];
  cargs[0].i = jidx;
  util::localref<jobject> jr(env, sc, env->CallObjectMethodA(jv->object,
    sc->getMIDLBGetElement(), cargs));
  return util::to_ase_variant_from_jobject(env, sc, jr.get());
}

void
ase_jni_variant_impl_jobject::lateSetElement(const ase_variant& v,
  ase_int idx, const ase_variant& value)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jint jidx = ase_numeric_cast<jint>(idx);
  util::jobjectref jval(env);
  util::to_jobject_from_ase_variant(env, sc, value, jval);
  jvalue cargs[2];
  cargs[0].i = jidx;
  cargs[1].l = jval.get();
  DBG_SET(fprintf(stderr, "lateSetElement type=%d val=%p rep=%p\n",
    value.get_type(), jval.get(), value.get_rep().p));
  env->CallVoidMethodA(jv->object, sc->getMIDLBSetElement(), cargs);
  util::checkJavaException(env, sc, "JNI: latesetelement");
}

ase_int
ase_jni_variant_impl_jobject::lateGetLength(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jint r = env->CallIntMethod(jv->object, sc->getMIDLBGetLength());
  util::checkJavaException(env, sc, "JNI: lategetlength");
  return ase_numeric_cast<ase_int>(r);
}

void
ase_jni_variant_impl_jobject::lateSetLength(const ase_variant& v,
  ase_int len)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  jint jlen = ase_numeric_cast<jint>(len);
  jvalue cargs[1];
  cargs[0].i = jlen;
  env->CallVoidMethodA(jv->object, sc->getMIDLBSetLength(), cargs);
  util::checkJavaException(env, sc, "JNI: latesetlength");
}

struct invokeargs {

  invokeargs(ase_jni_script_engine *sc, JNIEnv *env, const ase_variant *args,
    ase_size_type nargs)
    : jnargs(static_cast<jsize>(nargs)),
      jargs(env, sc, env->NewObjectArray(jnargs, sc->getObjectKlass(), 0))
  {
    for (jsize i = 0; i < jnargs; ++i) {
      util::jobjectref elem(env);
      util::to_jobject_from_ase_variant(env, sc, args[i], elem);
      env->SetObjectArrayElement(jargs.get(), i, elem.get());
      util::checkJavaException(env, sc, "JNI: lateinvokeself args");
    }
  }

  jsize jnargs;
  util::localref<jobjectArray> jargs;

};

ase_variant
ase_jni_variant_impl_jobject::lateInvokeByName(const ase_variant& v,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jstring> jname(env, sc, util::to_java_string(env, name,
    namelen));
  invokeargs iargs(sc, env, args, nargs);
  jvalue cargs[2];
  cargs[0].l = jname.get();
  cargs[1].l = iargs.jargs.get();
  DBG_LATE(fprintf(stderr, "lateinvokebyname\n"));
  util::localref<jobject> jr(env, sc, env->CallObjectMethodA(jv->object,
    sc->getMIDLBInvokeByName(), cargs));
  DBG_LATE(fprintf(stderr, "lateinvokebyname: jr=%p\n", jr.get()));
  ase_variant r = util::to_ase_variant_from_jobject(env, sc, jr.get());
  return r;
}

ase_variant
ase_jni_variant_impl_jobject::lateInvokeById(const ase_variant& v,
  ase_int id, const ase_variant *args, ase_size_type nargs)
{
  /* LateBinding.invoke_by_id is not implemented */
  return ase_variant();
}

ase_variant
ase_jni_variant_impl_jobject::lateInvokeSelf(const ase_variant& v,
  const ase_variant *args, ase_size_type nargs)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  invokeargs iargs(sc, env, args, nargs);
  jvalue cargs[1];
  cargs[0].l = iargs.jargs.get();
  DBG_LATE(fprintf(stderr, "lateinvokeself\n"));
  util::localref<jobject> jr(env, sc, env->CallObjectMethodA(jv->object,
    sc->getMIDLBInvokeSelf(), cargs));
  DBG_LATE(fprintf(stderr, "lateinvokeself: jr=%p\n", jr.get()));
  ase_variant r = util::to_ase_variant_from_jobject(env, sc, jr.get());
  return r;
}

ase_variant
ase_jni_variant_impl_jobject::lateGetEnumerator(const ase_variant& v)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jobject> jr(env, sc, env->CallObjectMethod(jv->object,
    sc->getMIDLBGetEnumerator()));
  return util::to_ase_variant_from_jobject(env, sc, jr.get());
}

ase_string
ase_jni_variant_impl_jobject::lateEnumNext(const ase_variant& v,
  bool& hasnext_r)
{
  ase_jni_variant_impl_jobject *jv = get(v);
  ase_jni_script_engine *sc = getContext(v);
  JNIEnv *env = sc->getEnv();
  util::localref<jstring> jr(env, sc, static_cast<jstring>(
    env->CallObjectMethod(jv->object, sc->getMIDLBEnumNext())));
  if (!jr.get()) {
    hasnext_r = false;
    return ase_string();
  }
  hasnext_r = true;
  return util::to_ase_string(env, sc, jr.get());
}

ase_variant
ase_jni_jobject_new(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classinfo *ci /* nullable */, jobject obj /* nullable */,
  bool examine_dyntype)
{
  DBG(fprintf(stderr, "jobj new obj=%p\n", obj));
  ase_jni_classinfo *dyn_ci = ci;
  if (ci == 0 || (examine_dyntype && obj)) {
    /* get dynamic class */
    util::localref<jclass> dynkl(env, sc, static_cast<jclass>(
      env->CallObjectMethod(obj, sc->getMIDGetClass())));
    if (ci == 0 || (!env->IsSameObject(ci->klass.get(), dynkl.get()))) {
      dyn_ci = sc->getClassMap().find_type_synchronized(env, sc, dynkl);
      DBG_DYNTYPE(fprintf(stderr, "DYNTYPE %s to %s\n",
	(ci ? ci->name.data() : ""), dyn_ci->name.data()));
    } else {
      DBG_DYNTYPE(fprintf(stderr, "TYPE %s\n", ci ? ci->name.data() : ""));
    }
  }
  if (obj != 0 && dyn_ci->is_javalangclass) {
    /* obj is an instance of java.lang.Class. convert to asejni class
      object. */
    util::localref<jclass> dynkl(env, sc, static_cast<jclass>(obj));
    dyn_ci = sc->getClassMap().find_type_synchronized(env, sc, dynkl);
    obj = 0;
    DBG_DYNTYPE(fprintf(stderr, "JLCLASS %s\n", dyn_ci->name.data()));
  }
  std::auto_ptr<ase_jni_variant_impl_jobject> aimpl(
    new ase_jni_variant_impl_jobject(env, obj, dyn_ci));
  const ase_variant_vtable *vtbl = obj
    ? (&dyn_ci->vtbl) : (&dyn_ci->static_vtbl);
  ase_variant r = ase_variant::create_object(vtbl, aimpl.get(),
    "JNIJavaObject");
  aimpl.release();
  return r;
}

jobject
ase_jni_jniproxy_new(JNIEnv *env, ase_jni_script_engine *sc,
  ase_jni_classinfo& ci, const ase_variant& value)
{
  ase_jni_jniproxy_priv *jniproxy_endnode = sc->get_jniproxy_endnode();
  std::auto_ptr<ase_jni_jniproxy_priv> priv(new ase_jni_jniproxy_priv(value,
    &ci, *jniproxy_endnode));
  jvalue cargs[2];
  /* if ci is an interface, create a reflect.Proxy. otherwise, create a
    JNativeObject which implements LateBinding interface. */
  cargs[0].l = ci.is_interface ? ci.klass.get() : 0;
  cargs[1].j = reinterpret_cast<jlong>(priv.get());
  util::localref<jobject> probj(env, sc, env->CallStaticObjectMethodA(
    sc->getJNPRXKlass(), sc->getMIDNewProxy(), cargs));
  priv.release();
  return probj.release();
}

ase_variant
ase_jni_jniproxy_unwrap_latebinding(JNIEnv *env, ase_jni_script_engine *sc,
  jobject obj)
{
  jlong lpriv = env->GetLongField(obj, sc->getFIDNPRXPriv());
  util::checkJavaException(env, sc, "JNI: ase jniproxy unwrap latebinding");
  ase_jni_jniproxy_priv *priv =
    reinterpret_cast<ase_jni_jniproxy_priv *>(lpriv);
  if (!priv) {
    return ase_variant();
  }
  return priv->value.get();
}

}; // namespace asejni

