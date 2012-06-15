
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseutf16.hpp>
#include <ase/aseflags.hpp>
#include <ase/asecast.hpp>

#include "asejs_sctx.hpp"
#include "asejs_aseimpl.hpp"
#include "asejs_util.hpp"

#define DBG_ROOT(x)
#define DBG_VI_INVOKE(x)
#define DBG_CONSTR(x)
#define DBG_DESTROY(x)
#define DBG_LOCK(x)
#define DBG_GL(x)
#define DBG_EVAL(x)

namespace asejs {

struct ase_js_ase_variant_enum_impl : public ase_variant_impl_default {

 public:
 
  typedef std::vector<ase_string> keys_type;

  static ase_js_ase_variant_enum_impl *get(const ase_variant& self) {
    return static_cast<ase_js_ase_variant_enum_impl *>(self.get_rep().p);
  }

  ase_js_ase_variant_enum_impl(keys_type& k /* swap */);

  static void add_ref(const ase_variant& self);
  static void release(const ase_variant& self);
  static ase_string enum_next(const ase_variant& self, bool& hasnext_r);

 private:

  mutable ase_atomic_count refcount;
  keys_type keys;
  keys_type::size_type current_pos;

};

ase_js_ase_variant_impl::ase_js_ase_variant_impl(
  const ase_js_ase_variant_impl::init_arg& ini)
  : jobject(ini.object), function_thisobject(ini.functhis),
    refcount(1)
{
  JSContext *cx = ini.jscontext;
  DBG_CONSTR(fprintf(stderr, "js: CONSTR %p\n", this));
  if (jobject) {
    DBG_ROOT(fprintf(stderr,
      "JSADDROOT %p ase_js_ase_variant_impl this=%p obj=%p\n",
      &jobject, this, jobject));
    if (!JS_AddNamedRoot(cx, &jobject, "ase_js_ase_variant_impl")) {
      ini.script_engine->handle_js_error(cx);
      return;
    }
    ASE_DEBUG(ase_js_variant_impl_count_addroot++);
    if (function_thisobject) {
      DBG_ROOT(fprintf(stderr,
	"JSADDROOT %p ase_js_ase_variant_impl_f this=%p obj=%p\n",
	&function_thisobject, this, function_thisobject));
      if (!JS_AddNamedRoot(cx, &function_thisobject,
	"ase_js_ase_variant_impl_f")) {
	ini.script_engine->handle_js_error(cx);
	return;
      }
      ASE_DEBUG(ase_js_variant_impl_count_addroot++);
    }
  }
  ASE_DEBUG(ase_js_variant_impl_count_constr++);
}

ase_js_ase_variant_impl::~ase_js_ase_variant_impl() /* DCHK */
{
  ASE_DEBUG(ase_js_variant_impl_count_destr++);
  DBG_DESTROY(fprintf(stderr, "js: DESTROY %p\n", this));
}

void
ase_js_ase_variant_impl::add_ref(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  selfp->value.refcount++;
}

void
ase_js_ase_variant_impl::release(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  if ((selfp->value.refcount--) == 1) {
    selfp->value.dispose(sc);
    ase_variant::debug_on_destroy_object(self, "JSObject");
    delete selfp;
  }
}

class asejs_callfromase_lock {

 public:

  asejs_callfromase_lock(ase_js_script_engine *sc, const char *mess)
    : mutex(sc->get_uemutex()), dbgmess(mess) {
    DBG_LOCK(fprintf(stderr, "js_callfromase lock %p %s %lu\n",
      &mutex, dbgmess, pthread_self()));
    #ifdef ASE_NONPREEMPTIVE
    mutex.lock();
    #endif
  }
  ~asejs_callfromase_lock() /* DCHK */ {
    DBG_LOCK(fprintf(stderr, "js_callfromase unlock %p %s %lu\n",
      &mutex, dbgmess, pthread_self()));
    #ifdef ASE_NONPREEMPTIVE
    mutex.unlock();
    #endif
  }

 private:

  ase_recursive_mutex& mutex;
  const char *dbgmess;

 private:

  /* noncopyable */
  asejs_callfromase_lock(const asejs_callfromase_lock& x);
  asejs_callfromase_lock& operator =(const asejs_callfromase_lock& x);

};

void
ase_js_ase_variant_impl::dispose(ase_js_script_engine *sc)
{
  /* shared_mutex is already locked */
  if (jobject) {
    /* NOTE: can dereference 'sc' only if jobject != 0. */
    ase_js_scoped_context sctx(sc);
    JSContext *const cx = sctx.get();
    if (function_thisobject) {
      ASE_DEBUG(ase_js_variant_impl_count_remroot++);
      DBG_ROOT(fprintf(stderr,
	"JSREMROOT %p ase_js_ase_variant_impl_di_f obj=%p\n",
	&function_thisobject, function_thisobject));
      JS_RemoveRoot(cx, const_cast<JSObject **>(&function_thisobject));
      function_thisobject = 0;
    }
    ASE_DEBUG(ase_js_variant_impl_count_remroot++);
    DBG_ROOT(fprintf(stderr,
      "JSREMROOT %p ase_js_ase_variant_impl_di obj=%p\n", &jobject, jobject));
    JS_RemoveRoot(cx, const_cast<JSObject **>(&jobject));
    jobject = 0;
  }
}

ase_int
ase_js_ase_variant_impl::get_attributes(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_attributes");
  ase_int r = ase_attrib_mutable;
  if (!selfp->value.jobject) {
    return r;
  }
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (JS_IsArrayObject(cx, selfp->value.jobject)) {
    r |= ase_attrib_array;
  }
  if (JS_ObjectIsFunction(cx, selfp->value.jobject)) {
    r |= ase_attrib_function;
  }
  /* TODO: how to determine for perl objects? */
  return r;
}

ase_string
ase_js_ase_variant_impl::get_string(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_string");
  if (!selfp->value.jobject) {
    return ase_string();
  }
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  JSString *str = JS_ValueToString(cx, object_to_jsval(selfp->value.jobject));
  if (!str) {
    if (JS_IsExceptionPending(cx)) {
      /* get_string should not throw as far as possible */
      try {
	sc->handle_js_error(cx);
      } catch (const ase_variant& av) {
	return av.get_string();
      } /* NOTE: std::exception is not caught */
    }
    return ase_string("[JS]");
  }
  jschar *cs = JS_GetStringChars(str);
  size_t cl = JS_GetStringLength(str);
  return ase_string(ase_js_util::to_ucchar(cs), cl);
}

ase_variant
ase_js_ase_variant_impl::get_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_property");
  if (!selfp->value.jobject) {
    return ase_variant();
  }
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  jsval val = JSVAL_VOID;
  ase_utf16_string s(name, namelen);
  if (!JS_GetUCProperty(cx, selfp->value.jobject,
    ase_js_util::to_jschar(s.data()), s.size(), &val)) {
    sc->handle_js_error(cx);
    return ase_variant();
  }
  return ase_js_util::to_ase_variant(sc, cx, val, selfp->value.jobject);
}

void
ase_js_ase_variant_impl::set_property(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant& value)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "set_property");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return;
  }
  ase_js_scoped_root jvr = ase_js_util::to_js_value_no_root(sc, cx, value);
  jsval jv = jvr.get();
  ase_utf16_string s(name, namelen);
  if (!JS_SetUCProperty(cx, selfp->value.jobject,
    ase_js_util::to_jschar(s.data()), s.size(), &jv)) {
    sc->handle_js_error(cx);
    return;
  }
}

void
ase_js_ase_variant_impl::del_property(const ase_variant& self,
  const char *name, ase_size_type namelen)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "del_property");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return;
  }
  jsval v = JSVAL_VOID;
  ase_utf16_string s(name, namelen);
  if (!JS_DeleteUCProperty2(cx, selfp->value.jobject,
    ase_js_util::to_jschar(s.data()), s.size(), &v)) {
    sc->handle_js_error(cx);
    return;
  }
}

ase_variant
ase_js_ase_variant_impl::get_element(const ase_variant& self,
  ase_int idx)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_element");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return ase_variant();
  }
  jsval val = JSVAL_VOID;
  if (!JS_GetElement(cx, selfp->value.jobject, idx, &val)) {
    sc->handle_js_error(cx);
    return ase_variant();
  }
  return ase_js_util::to_ase_variant(sc, cx, val, selfp->value.jobject);
}

void
ase_js_ase_variant_impl::set_element(const ase_variant& self,
  ase_int idx, const ase_variant& value)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "set_element");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return;
  }
  ase_js_scoped_root jvr = ase_js_util::to_js_value_no_root(sc, cx, value);
  jsval jv = jvr.get();
  if (!JS_SetElement(cx, selfp->value.jobject, idx, &jv)) {
    sc->handle_js_error(cx);
  }
}

ase_int
ase_js_ase_variant_impl::get_length(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_length");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return 0;
  }
  jsuint length = 0;
  if (!JS_GetArrayLength(cx, selfp->value.jobject, &length)) {
    sc->handle_js_error(cx);
    return 0;
  }
  return length;
}

void
ase_js_ase_variant_impl::set_length(const ase_variant& self, ase_int len)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "set_length");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return;
  }
  jsuint length = len;
  if (!JS_SetArrayLength(cx, selfp->value.jobject, length)) {
    sc->handle_js_error(cx);
    return;
  }
}

ase_variant
ase_js_ase_variant_impl::invoke_by_name(const ase_variant& self,
  const char *name, ase_size_type namelen, const ase_variant *args,
  ase_size_type nargs)
{
  /* TODO: faster */
  ase_variant pv = get_property(self, name, namelen);
  return pv.invoke_self(args, nargs);
}

ase_variant
ase_js_ase_variant_impl::invoke_by_id(const ase_variant& self, ase_int id,
  const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

ase_variant
ase_js_ase_variant_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "invoke_self");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return ase_variant();
  }
  std::vector<ase_js_scoped_root> jrargs(nargs);
  std::vector<jsval> jargs(nargs);
  jsval *jargsp = ase_js_util::to_js_value_array(sc, cx, args, nargs, jrargs,
    jargs);
  jsval val_r = JSVAL_VOID;
  DBG_VI_INVOKE(fprintf(stderr, "js: isfunction=%d\n",
    JS_ObjectIsFunction(cx, selfp->value.jobject)));
  JSObject *fthis = selfp->value.jobject;
  if (selfp->value.function_thisobject) {
    fthis = selfp->value.function_thisobject;
    DBG_VI_INVOKE(fprintf(stderr, "js: ftobj=%p\n", fthis));
  }
  if (!JS_CallFunctionValue(cx, fthis,
    object_to_jsval(selfp->value.jobject), static_cast<uintN>(nargs), jargsp,
    &val_r)) {
    DBG_VI_INVOKE(fprintf(stderr, "js: callfunction failed\n"));
    sc->handle_js_error(cx);
    return ase_variant();
  }
  return ase_js_util::to_ase_variant(sc, cx, val_r);
}

struct ase_js_scoped_idarr {

  ase_js_scoped_idarr(JSContext *cx, JSIdArray *v) : context(cx), idarr(v) { }
  ~ase_js_scoped_idarr() /* DCHK */ {
    JS_DestroyIdArray(context, idarr);
  }
  JSIdArray *get() const { return idarr; }

 private:

  JSContext *context;
  JSIdArray *idarr;

 private:

  /* noncopyable */
  ase_js_scoped_idarr(const ase_js_scoped_idarr& x);
  ase_js_scoped_idarr& operator =(const ase_js_scoped_idarr& x);

};

ase_variant
ase_js_ase_variant_impl::get_enumerator(const ase_variant& self)
{
  ase_js_ase_variant_implnode *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "get_enumerator");
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  if (!selfp->value.jobject) {
    return ase_variant();
  }
  std::vector<ase_string> keys;
  {
    ase_js_scoped_idarr idarr(cx, JS_Enumerate(cx, selfp->value.jobject));
    const bool need_rooted = true;
    ase_js_scoped_root idv(cx, JSVAL_VOID, need_rooted);
    int len = idarr.get()->length;
    for (int i = 0; i < len; ++i) {
      jsid id = idarr.get()->vector[i];
      if (!JS_IdToValue(cx, id, idv.getaddr())) {
	sc->handle_js_error(cx);
	return ase_variant();
      }
      JSString *idstr = JS_ValueToString(cx, idv.get());
      if (!idstr) {
	sc->handle_js_error(cx);
	return ase_variant();
      }
      const jschar *u16str = JS_GetStringChars(idstr);
      size_t u16len = JS_GetStringLength(idstr);
      ase_string u8str(ase_js_util::to_ucchar(u16str), u16len);
      keys.push_back(u8str);
    }
  }
  ase_js_ase_variant_enum_impl *p = new ase_js_ase_variant_enum_impl(keys);
  return ase_variant::create_object(
    &ase_variant_impl<ase_js_ase_variant_enum_impl>::vtable,
    p, "JSEnum");
}

ase_js_ase_variant_enum_impl::ase_js_ase_variant_enum_impl(keys_type& k)
  : refcount(1), current_pos(0)
{
  keys.swap(k);
}

void
ase_js_ase_variant_enum_impl::add_ref(const ase_variant& self)
{
  ase_js_ase_variant_enum_impl *const selfp = get(self);
  selfp->refcount++;
}

void
ase_js_ase_variant_enum_impl::release(const ase_variant& self)
{
  ase_js_ase_variant_enum_impl *const selfp = get(self);
  if ((selfp->refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "JSEnum");
    delete selfp;
  }
}

ase_string
ase_js_ase_variant_enum_impl::enum_next(const ase_variant& self,
  bool& hasnext_r)
{
  ase_js_ase_variant_enum_impl *const selfp = get(self);
  if (selfp->current_pos >= selfp->keys.size()) {
    hasnext_r = false;
    return ase_string();
  }
  hasnext_r = true;
  ase_string r = selfp->keys[selfp->current_pos];
  (selfp->current_pos)++;
  return r;
}

ase_js_ase_variant_glfunc_impl::ase_js_ase_variant_glfunc_impl(
  ase_js_script_engine *sc, JSContext *cx, funcptr_type ptr)
  : script_engine(sc), funcptr(ptr), refcount(1),
    disposed(false)
{
  DBG_GL(fprintf(stderr, "glfunc new %p sc=%p\n", this, script_engine));
}

ase_js_ase_variant_glfunc_impl::~ase_js_ase_variant_glfunc_impl() /* DCHK */
{
  DBG_GL(fprintf(stderr, "glfunc destroy %p sc=%p\n", this, script_engine));
}

void
ase_js_ase_variant_glfunc_impl::add_ref(const ase_variant& self)
{
  ase_js_ase_variant_glfunc_impl *const selfp = get(self);
  selfp->refcount++;
}

void
ase_js_ase_variant_glfunc_impl::release(const ase_variant& self)
{
  ase_js_ase_variant_glfunc_impl *const selfp = get(self);
  if ((selfp->refcount--) == 1) {
    ase_variant::debug_on_destroy_object(self, "JSGLFunc");
    delete selfp;
  }
}

void
ase_js_ase_variant_glfunc_impl::dispose()
{
  ase_js_script_engine *const sc = script_engine;
  asejs_callfromase_lock g(sc, "gl:dispose");
  disposed = true;
}

ase_variant
ase_js_ase_variant_glfunc_impl::invoke_self(const ase_variant& self,
  const ase_variant *args, ase_size_type nargs)
{
  ase_js_ase_variant_glfunc_impl *const selfp = get(self);
  ase_js_script_engine *const sc = getctx(self);
  asejs_callfromase_lock g(sc, "gl:invoke_self");
  if (selfp->disposed) {
    return ase_variant();
  }
  ase_js_scoped_context sctx(sc);
  JSContext *const cx = sctx.get();
  return selfp->funcptr(sc, cx, args, nargs);
}

ase_variant
ase_js_ase_variant_glfunc_new(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs)
{
  if (nargs < 1) {
    JSObject *o = JS_NewObject(cx, 0, 0, 0);
    if (!o) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
    return ase_js_util::to_ase_variant(sc, cx, object_to_jsval(o));
  }
  std::vector<ase_js_scoped_root> jrargs(nargs);
  std::vector<jsval> jargs(nargs);
  jsval *jargsp = ase_js_util::to_js_value_array(sc, cx, args, nargs, jrargs,
    jargs);
  jsval funcval = jargsp[0];
  JSFunction *func = 0;

  if (JSVAL_IS_OBJECT(funcval)) {
    func = JS_ValueToFunction(cx, funcval);
    /* funcval is still valid because it's rooted by jrargs[0] */
    if (!func) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
  }
  if (!func) {
    JSString *str = JS_ValueToString(cx, funcval);
    func = 0; /* becomes invalid */
    if (!str) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
    jschar *cs = JS_GetStringChars(str);
    size_t cl = JS_GetStringLength(str);
    if (!JS_GetUCProperty(cx, sc->get_js_global(), cs, cl, &funcval)) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
    /* no need to root funcval because it's a property of sc->jglobal */
    if (!JSVAL_IS_OBJECT(funcval)) {
      /* not a function */
      return ase_variant();
    }
    /* check if func again */
    func = JS_ValueToFunction(cx, funcval);
    if (!func) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
  }
  if (!func || !JSVAL_IS_OBJECT(funcval)) {
    /* failsafe */
    return ase_variant();
  }
  JSClass *klass = func->clasp;
  if (klass) {
    /* native class */
    JSObject *robj = JS_ConstructObjectWithArguments(cx, klass, 0, 0,
      static_cast<uintN>(nargs - 1), jargsp + 1);
    if (!robj) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
    return ase_js_util::to_ase_variant(sc, cx, object_to_jsval(robj));
  }
  /* use defined function */
  JSObject *robj = JS_NewObject(cx, 0, 0, 0);
  func = 0; /* becomes invalid */
  if (!robj) {
    sc->handle_js_error(cx);
    return ase_variant();
  }
  const bool need_rooted = true;
  ase_js_scoped_root rrobj(cx, object_to_jsval(robj), need_rooted);
  JSObject *arr = JS_NewArrayObject(cx, static_cast<jsint>(nargs - 1), 0);
  if (!arr) {
    sc->handle_js_error(cx);
    return ase_variant();
  }
  ase_js_scoped_root rarr(cx, object_to_jsval(arr), need_rooted);
  ase_size_type i = 0;
  for (i = 1; i < nargs; ++i) {
    if (!JS_SetElement(cx, arr, static_cast<jsint>(i - 1), jargsp + i)) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
  }
  jsval apargv[2];
  apargv[0] = object_to_jsval(robj);
  apargv[1] = object_to_jsval(arr);
  JSObject *fo = jsval_to_object(funcval);
  jsval val_r = JSVAL_VOID;
  if (!JS_CallFunctionName(cx, fo, "apply", 2, apargv, &val_r)) {
    sc->handle_js_error(cx);
    return ase_variant();
  }
  return ase_js_util::to_ase_variant(sc, cx, val_r);
}

ase_variant
ase_js_ase_variant_glfunc_eval(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs)
{
  if (nargs < 1) {
    return ase_variant();
  }
  ase_string str = args[0].get_string();
  std::string fname_bytes;
  uintN linenum = 1;
  bool is_perl = false;
  if (nargs >= 2) {
    ase_string astr = args[1].get_string();
    fname_bytes = std::string(astr.data(), astr.size());
    std::string::size_type dot = fname_bytes.rfind('.');
    if (dot != fname_bytes.npos) {
      std::string suffix = fname_bytes.substr(dot);
      if (suffix == ".pl") {
	is_perl = true;
      }
    }
  }
  if (nargs >= 3) {
    linenum = args[2].get_int();
  }
  jsval val_r = JSVAL_VOID;
  if (is_perl) {
    std::string su8(str.data(), str.size());
    PMC_APIFunctionTable_2 *pmctable = 0;
    PMCPerl *pmcperl = 0;
    sc->get_pmc(&pmctable, &pmcperl);
    if (pmctable != 0 && pmcperl != 0 &&
      !(pmctable->PMC_Eval(pmcperl, cx, su8.c_str()))) {
      DBG_EVAL(jsval ex = JSVAL_VOID);
      DBG_EVAL(JS_GetPendingException(cx, &ex));
      DBG_EVAL(ase_js_util::ase_js_dump_jsval(cx, ex));
      sc->handle_js_error(cx);
      return ase_variant();
    }
  } else {
    ase_utf16_string str16(str);
    const ase_ucchar *str16begin = str16.data();
    uintN str16len = ase_numeric_cast<uintN>(str16.size());
    ase_size_type skip = 0;
    if (skip < str16len && str16begin[0] == '#') {
      while (skip < str16len && str16begin[skip] != '\n') {
	++skip;
      }
      ++skip;
      ++linenum;
    }
    JSScript *scr = JS_CompileUCScript(cx, sc->get_js_global(),
      ase_js_util::to_jschar(str16begin + skip), str16len - skip,
      fname_bytes.c_str(), linenum);
    if (scr == 0) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
    if (!JS_ExecuteScript(cx, sc->get_js_global(), scr, &val_r)) {
      sc->handle_js_error(cx);
      return ase_variant();
    }
  }
  return ase_js_util::to_ase_variant(sc, cx, val_r);
}

ase_variant
ase_js_ase_variant_glfunc_print(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs)
{
  for (ase_size_type i = 0; i < nargs; ++i) {
    const ase_variant& a = args[i];
    ase_vtype ty = a.get_type();
    if (i != 0) {
      std::cout << " ";
    }
    switch (ty) {
    case ase_vtype_void:
      break;
    case ase_vtype_bool:
      std::cout << (a.get_boolean() ? "true" : "false");
      break;
    case ase_vtype_int:
      std::cout << a.get_int();
      break;
    case ase_vtype_long:
      std::cout << a.get_long();
      break;
    case ase_vtype_double:
      std::cout << a.get_double();
      break;
    case ase_vtype_string:
      std::cout << a.get_string();
      break;
    default:
      std::cout << "[ASEVariant Object]";
      break;
    }
  }
  std::cout << std::endl;
  return ase_variant();
}

ase_variant
ase_js_ase_variant_glfunc_gc(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant *args, ase_size_type nargs)
{
  JS_GC(cx);
  return ase_variant();
}

}; // namespace asejs

