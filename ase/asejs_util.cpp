
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include "asejs_sctx.hpp"
#include "asejs_asepr.hpp"
#include "asejs_util.hpp"

#include <ase/aseexcept.hpp>
#include <ase/aseutf16.hpp>

#include <jsdbgapi.h>

#ifdef _MSC_VER
#include <malloc.h>
#define ASE_JS_STACK_ALLOCA(x) _alloca(x)
#else
#include <alloca.h>
#define ASE_JS_STACK_ALLOCA(x) alloca(x)
#endif


#define DBG_ROOT(x)
#define DBG_CONV(x)
#define DBG_NEWOBJ(x)
#define DBG_REQ(x)

namespace asejs {

unsigned int ase_js_util::verbose = 0;

ase_js_scoped_root::ase_js_scoped_root()
  : value(JSVAL_VOID), jsctx(0), add_root(false)
{
}

ase_js_scoped_root::ase_js_scoped_root(JSContext *cx, jsval val,
  bool need_addroot)
  : value(val), jsctx(cx), add_root(need_addroot)
{
  if (add_root) {
    DBG_ROOT(fprintf(stderr, "JSADDROOT %p ase_js_scoped_root\n", &value));
    JS_AddNamedRoot(jsctx, &value, "ase_js_scoped_root");
  }
}

ase_js_scoped_root::ase_js_scoped_root(const ase_js_scoped_root& x)
  : value(x.value), jsctx(x.jsctx), add_root(x.add_root)
{
  if (add_root) {
    DBG_ROOT(fprintf(stderr, "JSADDROOT %p ase_js_scoped_root_cp\n", &value));
    JS_AddNamedRoot(jsctx, &value, "ase_js_scoped_root_cp");
  }
}

ase_js_scoped_root&
ase_js_scoped_root::operator =(const ase_js_scoped_root& x)
{
  if (this != &x) {
    if (add_root) {
      DBG_ROOT(fprintf(stderr, "JSREMROOT %p ase_js_scoped_root_eq\n",
	&value));
      JS_RemoveRoot(jsctx, &value);
    }
    value = x.value;
    jsctx = x.jsctx;
    add_root = x.add_root;
    if (add_root) {
      DBG_ROOT(fprintf(stderr, "JSADDROOT %p ase_js_scoped_root_eq\n",
	&value));
      JS_AddNamedRoot(jsctx, &value, "ase_js_scoped_root");
    }
  }
  return *this;
}

ase_js_scoped_root::~ase_js_scoped_root() /* DCHK */
{
  if (add_root) {
    DBG_ROOT(fprintf(stderr, "JSREMROOT %p ase_js_scoped_root_de\n", &value));
    JS_RemoveRoot(jsctx, &value);
  }
}

ase_string
ase_js_util::get_string(ase_js_script_engine *sc,
  JSContext *cx, jsval val)
{
  if (!JSVAL_IS_STRING(val)) {
    return ase_string();
  }
  JSString *str = jsval_to_string(val);
  if (!str) {
    sc->handle_js_error(cx);
    return ase_string();
  }
  ase_size_type len = JS_GetStringLength(str);
  const ase_ucchar *cs = to_ucchar(JS_GetStringChars(str));
  return ase_string(cs, len);
}

bool
ase_js_util::is_ase_variant(ase_js_script_engine *sc, JSContext *cx,
  jsval jv)
{
  if (!JSVAL_IS_OBJECT(jv)) {
    return false;
  }
  JSObject *o = jsval_to_object(jv);
  JSClass *c = JS_GET_CLASS(cx, o);
  if (c == &ase_js_aseproxy_class) {
    return true;
  }
  return false;
}

ase_variant
ase_js_util::to_ase_variant(ase_js_script_engine *sc, JSContext *cx,
  jsval jv, JSObject *thisobj)
{
  if (JSVAL_IS_PRIMITIVE(jv)) {
    /* value type */
    if (JSVAL_IS_INT(jv)) {
      DBG_CONV(fprintf(stderr, "jsval to int\n"));
      return ase_variant::create_int(JSVAL_TO_INT(jv));
    } else if (JSVAL_IS_BOOLEAN(jv)) {
      DBG_CONV(fprintf(stderr, "jsval to boolean\n"));
      return ase_variant::create_boolean(JSVAL_TO_BOOLEAN(jv) != 0);
    } else if (JSVAL_IS_DOUBLE(jv)) {
      DBG_CONV(fprintf(stderr, "jsval to double\n"));
      return ase_variant::create_double(*jsval_to_double(jv));
    } else if (JSVAL_IS_STRING(jv)) {
      DBG_CONV(fprintf(stderr, "jsval to string\n"));
      JSString *str = jsval_to_string(jv);
      jschar *cs = JS_GetStringChars(str);
      size_t len = JS_GetStringLength(str);
      return ase_string(to_ucchar(cs), len).to_variant();
    }
    DBG_CONV(fprintf(stderr, "jsval to void\n"));
    return ase_variant();
  }
  if (is_ase_variant(sc, cx, jv)) {
    /* wrapped ase variant */
    DBG_CONV(fprintf(stderr, "jsval wrapped ase\n"));
    JSObject *o = jsval_to_object(jv);
    void *priv = JS_GetPrivate(cx, o);
    ase_js_aseproxy_privnode *avp =
      static_cast<ase_js_aseproxy_privnode *>(priv);
    return avp->value;
  }
  /* (!JSVAL_IS_PRIMITIVE) implies JSVAL_IS_OBJECT */
  JSObject *jvobj = jsval_to_object(jv);
  JSObject *ftobj = 0;
  if (JS_ObjectIsFunction(cx, jvobj)) {
    /* enclose thisobj with the function */
    ftobj = thisobj;
  }
  /* create a proxy */
  DBG_CONV(fprintf(stderr, "jsval to proxy\n"));
  ase_js_ase_variant_implnode *p = new ase_js_ase_variant_implnode(
    ase_js_ase_variant_impl::init_arg(sc, cx, jvobj, ftobj),
    *sc->get_js_endnode());
  DBG_NEWOBJ(fprintf(stderr,
    "new ase_variant_implnode node=%p next=%p prev=%p nprev=%p\n", p,
      p->get_next(), p->get_prev(), p->get_next()->get_prev()));
  ase_variant rv = ase_variant::create_object(sc->get_variant_vtable(), p,
    "JSObject");
  return rv;
}

ase_variant
ase_js_util::to_ase_variant(ase_js_script_engine *sc, JSContext *cx,
  jsval jv)
{
  return to_ase_variant(sc, cx, jv, 0);
}

ase_js_scoped_root
ase_js_util::to_js_value_internal(ase_js_script_engine *sc,
  JSContext *cx, const ase_variant& v, bool need_root)
{
  jsval val = JSVAL_VOID;
  if (v.get_vtable_address() == sc->get_variant_vtable()) {
    /* wrapped js value */
    JSObject *o = static_cast<ase_js_ase_variant_implnode *>(v.get_rep().p)
      ->value.get_object();
    if (o) {
      val = object_to_jsval(o);
    } else {
      val = JSVAL_NULL;
    }
    need_root = false;
  } else {
    ase_vtype vt = v.get_type();
    if (ase_variant::is_value_type(vt)) {
      /* value type */
      switch (vt) {
      case ase_vtype_void:
	val = JSVAL_VOID;
	break;
      case ase_vtype_bool:
	val = BOOLEAN_TO_JSVAL(v.get_boolean());
	break;
      case ase_vtype_int:
	{
	  ase_int i = v.get_int();
	  if (INT_FITS_IN_JSVAL(i)) {
	    val = INT_TO_JSVAL(i);
	  } else {
	    double d = i;
	    if (!JS_NewDoubleValue(cx, d, &val)) {
	      sc->handle_js_error(cx);
	      val = JSVAL_VOID;
	    }
	  }
	}
	break;
      case ase_vtype_long:
	{
	  ase_long l = v.get_long();
	  ase_int i = static_cast<ase_int>(l);
	  ase_long ix = i;
	  if (l == ix && INT_FITS_IN_JSVAL(i)) {
	    val = INT_TO_JSVAL(i);
	  } else {
	    double d = static_cast<double>(l);
	    if (!JS_NewDoubleValue(cx, d, &val)) {
	      sc->handle_js_error(cx);
	      val = JSVAL_VOID;
	    }
	  }
	}
	break;
      case ase_vtype_double:
	if (!JS_NewDoubleValue(cx, v.get_double(), &val)) {
	  sc->handle_js_error(cx);
	  val = JSVAL_VOID;
	}
	break;
      case ase_vtype_string:
	{
	  ase_string s = v.get_string();
	  ase_utf16_string s16(s);
	  JSString *str = JS_NewUCStringCopyN(cx, to_jschar(s16.data()),
	    s16.size());
	  if (!str) {
	    sc->handle_js_error(cx);
	    val = JSVAL_VOID;
	  }
	  val = string_to_jsval(str);
	}
	break;
      default:
	val = JSVAL_VOID;
	break;
      }
      need_root = false;
    } else {
      /* create a proxy */
      val = new_ase_proxy(sc, cx, v);
    }
  }
  return ase_js_scoped_root(cx, val, need_root);
}

ase_js_scoped_root
ase_js_util::to_js_value_need_root(ase_js_script_engine *sc, JSContext *cx,
  const ase_variant& v)
{
  return to_js_value_internal(sc, cx, v, true);
}

ase_js_scoped_root
ase_js_util::to_js_value_no_root(ase_js_script_engine *sc, JSContext *cx,
  const ase_variant& v)
{
  return to_js_value_internal(sc, cx, v, false);
}

jsval *
ase_js_util::to_js_value_array(ase_js_script_engine *sc, JSContext *cx,
  const ase_variant *arr, ase_size_type len,
  std::vector<ase_js_scoped_root>& jrarr, std::vector<jsval>& jarr)
{
  for (ase_size_type i = 0; i < len; ++i) {
    jrarr[i] = to_js_value_need_root(sc, cx, arr[i]);
    jarr[i] = jrarr[i].get(); 
  }
  return (len != 0) ? (&jarr[0]) : 0;
}

jsval
ase_js_util::new_ase_proxy(ase_js_script_engine *sc, JSContext *cx,
  const ase_variant& av)
{
  JSObject *proto = sc->get_prototype(cx, av);
  JSObject *o = JS_NewObject(cx, &ase_js_aseproxy_class, proto, 0);
  if (!o) {
    sc->handle_js_error(cx);
    return JSVAL_VOID;
  }
  ase_js_aseproxy_privnode *endnode = sc->get_ase_endnode();
  std::auto_ptr<ase_js_aseproxy_privnode> avp(new ase_js_aseproxy_privnode(
    ase_js_aseproxy_priv::init_arg(sc, av), *endnode));
  if (!JS_SetPrivate(cx, o, avp.get())) {
    sc->handle_js_error(cx);
    return JSVAL_VOID;
  }
  avp.release();
  return object_to_jsval(o);
}

void
ase_js_util::handle_exception(ase_js_script_engine *sc, JSContext *cx)
  throw()
{
  ase_variant ex = ase_variant::create_from_active_exception();
  try {
    std::string description = "ASEException\n";
    JSStackFrame *fp = 0;
    while ((JS_FrameIterator(cx, &fp)) != 0) {
      JSScript *scr = 0;
      const char *scrname = 0;
      JSFunction *func = 0;
      const char *funcname = 0;
      uintN lineno = 0;
      jsbytecode *pc = 0;
      scr = JS_GetFrameScript(cx, fp);
      pc = JS_GetFramePC(cx, fp);
      if (scr == 0 || pc == 0) {
	continue;
      }
      scrname = JS_GetScriptFilename(cx, scr);
      lineno = JS_PCToLineNumber(cx, scr, pc);
      func = JS_GetFrameFunction(cx, fp);
      funcname = func ? JS_GetFunctionName(func) : "";
      ase_variant lnvar(static_cast<ase_long>(lineno));
      ase_string lns = lnvar.get_string();
      description += std::string("\tat ") + funcname + "(" + 
	scrname + ":" + std::string(lns.data(), lns.size()) + ")\n";
    }
    ASE_JS_VERBOSE_EXC(fprintf(stderr, "ASE Exception: %s\n",
      description.data()));
    ase_exception_append_trace(ex, ase_string(description).to_variant());
    ase_js_scoped_root rv = ase_js_util::to_js_value_no_root(sc, cx, ex);
    JS_SetPendingException(cx, rv.get());
  } catch (const std::bad_alloc&) {
    JS_ReportOutOfMemory(cx);
  } catch (...) {
    JS_ReportError(cx, "ASE: unexpected exception is thrown");
  }
  return;
}

void
ase_js_util::ase_js_dump_jsval(JSContext *cx, jsval val)
{
  if (!JSVAL_IS_OBJECT(val)) {
    std::cerr << "dumpjs: not an object" << std::endl;
    return;
  }
  JSObject *o = jsval_to_object(val);
  JSObject *p = JS_GetPrototype(cx, o);
  JSClass *kl = JS_GET_CLASS(cx, o);
  fprintf(stderr, "proto = %p\n", p);
  fprintf(stderr, "class = %p %s\n", kl, kl->name);
  jsval tostr = JSVAL_VOID;
  JS_GetProperty(cx, o, "toString", &tostr);
  JSString *s = JS_ValueToString(cx, tostr);
  ase_string strx(to_ucchar(JS_GetStringChars(s)), JS_GetStringLength(s));
  std::cerr << "dump: " << strx << std::endl;
}

}; // namespace asejs

