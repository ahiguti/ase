
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/aseexcept.hpp>
#include "asesyn.hpp"
#include "aseparser.h"

#include <memory>
#include <vector>

#define DBG(x)

namespace {

struct asesyn;

struct asesynimpl : public ase_noncopyable {

  asesynimpl() : sidx(0) { }
  virtual ~asesynimpl() { }
  virtual void validate() const { }
  virtual bool is_moduledef() const { return false; }
  virtual bool is_value() const { return false; }
  virtual bool is_expression() const { return false; }
  virtual bool is_sentence() const { return false; }

  asestag get_tag() const;
  ase_int get_constr_id() const;
  ase_string get_name() const;
  ase_string get_argstr(ase_size_type i) const;
  asesyn& get_asesyn(ase_size_type i) const;
  void validate_numargs() const;
  void validate_string(ase_size_type i) const;
  void validate_number(ase_size_type i) const;
  void validate_moduledef(ase_size_type i) const;
  void validate_value(ase_size_type i) const;
  void validate_expression(ase_size_type i) const;
  template <ase_size_type n>
  void validate_syn(ase_size_type i, asestag tag) const;

  ase_size_type sidx;
  std::vector<ase_variant> args;

};

struct asesyn : public ASE_Component::NoopImpl {

  asesyn(const ase_string& n, ASE_Component::Library& lib);
  asesyn(const asesyn& klass, const ase_variant *args, ase_size_type nargs);
  ase_string tostring() const;
  ase_variant tostring(const ase_variant *args, ase_size_type nargs);

  std::auto_ptr<asesynimpl> impl;

  static const ASE_Component::MethodEntry<asesyn> ClassMethods[];
  static const ASE_Component::MethodEntry<asesyn> InstanceMethods[];

};

template <typename T> asesynimpl *
create_asesyn()
{
  return new T();
}

struct asesyn_tbl {
  const char *name;
  asestag tag;
  asesynimpl *(*create_func)();
  ase_int constr_id;
  ase_int minargs;
  ase_int maxargs;
};

struct asesyn_module : public asesynimpl { };
struct asesyn_klass : public asesynimpl { };
struct asesyn_method : public asesynimpl { };
struct asesyn_args : public asesynimpl { };
struct asesyn_var : public asesynimpl { };
struct asesyn_attr : public asesynimpl { };
struct asesyn_seq : public asesynimpl { };
struct asesyn_while : public asesynimpl { };
struct asesyn_dowhile : public asesynimpl { };
struct asesyn_for : public asesynimpl { };
struct asesyn_if : public asesynimpl { };
struct asesyn_break : public asesynimpl { };
struct asesyn_continue : public asesynimpl { };
struct asesyn_throw : public asesynimpl { };
struct asesyn_trycatch : public asesynimpl { };
struct asesyn_return : public asesynimpl { };
struct asesyn_ref : public asesynimpl { };
struct asesyn_let : public asesynimpl { };
struct asesyn_get : public asesynimpl { };
struct asesyn_set : public asesynimpl { };
struct asesyn_minvoke : public asesynimpl { };
struct asesyn_op : public asesynimpl { };
struct asesyn_closure : public asesynimpl { };
struct asesyn_array : public asesynimpl { };
struct asesyn_map : public asesynimpl { };
struct asesyn_cpp : public asesynimpl { };

const asesyn_tbl asesyntbl[] = {
  /* name tag create_func constr_id minargs maxargs */
  { "Module", asestag_module, create_asesyn<asesyn_module>, 0, 0, -1 },
  { "Class", asestag_klass, create_asesyn<asesyn_klass>, 0, 0, -1 },
  { "Method", asestag_method, create_asesyn<asesyn_method>, 0, 4, 4 },
  { "Args", asestag_args, create_asesyn<asesyn_args>, 0, 0, -1 },
  { "Var", asestag_var, create_asesyn<asesyn_var>, 0, 2, 2 },
  { "Attr", asestag_attr, create_asesyn<asesyn_attr>, 0, 0, -1 },
  { "Seq", asestag_seq, create_asesyn<asesyn_seq>, 0, 0, -1 },
  { "While", asestag_while, create_asesyn<asesyn_while>, 0, 2, 2 },
  { "DoWhile", asestag_dowhile, create_asesyn<asesyn_dowhile>, 0, 2, 2 },
  { "For", asestag_for, create_asesyn<asesyn_for>, 0, 4, 4 },
  { "If", asestag_if, create_asesyn<asesyn_if>, 0, 3, 3 },
  { "Break", asestag_break, create_asesyn<asesyn_break>, 0, 0, 1 },
  { "Continue", asestag_continue, create_asesyn<asesyn_continue>, 0, 0, 1 },
  { "Throw", asestag_throw, create_asesyn<asesyn_throw>, 0, 1, 1 },
  { "TryCatch", asestag_trycatch, create_asesyn<asesyn_trycatch>, 0, 3, 3 },
  { "Return", asestag_return, create_asesyn<asesyn_return>, 0, 1, 1 },
  { "LRef", asestag_lref, create_asesyn<asesyn_ref>, 0, 1, 1 },
  { "GRef", asestag_gref, create_asesyn<asesyn_ref>, 1, 1, 1 },
  { "Let", asestag_let, create_asesyn<asesyn_let>, 0, 2, 2 },
  { "get", asestag_get, create_asesyn<asesyn_get>, 0, 2, 2 },
  { "set", asestag_set, create_asesyn<asesyn_set>, 0, 3, 3 },
  { "minvoke", asestag_minvoke, create_asesyn<asesyn_minvoke>, 0, 2, -1 },
  { "Op", asestag_op, create_asesyn<asesyn_op>, 0, 1, -1 },
  { "Closure", asestag_closure, create_asesyn<asesyn_closure>, 0, 2, 2 },
  { "Array", asestag_array, create_asesyn<asesyn_array>, 0, 0, -1 },
  { "Map", asestag_map, create_asesyn<asesyn_map>, 0, 0, -1 },
  { "Cpp", asestag_cpp, create_asesyn<asesyn_cpp>, 0, 1, 1 },
};

ase_int
asesynimpl::get_constr_id() const
{
  return asesyntbl[sidx].constr_id;
}

ase_string
asesynimpl::get_name() const
{
  return ase_string(asesyntbl[sidx].name);
}

ase_string
asesynimpl::get_argstr(ase_size_type i) const
{
  return get_name() + "(): arg #"
    + ase_variant(static_cast<int>(i) + 1).get_string();
}

asesyn&
asesynimpl::get_asesyn(ase_size_type i) const
{
  asesyn *p = ASE_Component::GetInstancePointer<asesyn, asesyn>(args[i]);
  if (!p) {
    ase_string s = get_argstr(i) + ": type mismatch";
    ase_throw_type_mismatch(s.data());
  }
  return *p;
}

void
asesynimpl::validate_numargs() const
{
  const ase_int minargs = asesyntbl[sidx].minargs;
  const ase_int maxargs = asesyntbl[sidx].maxargs;
  if (minargs >= 0 && args.size() < static_cast<ase_size_type>(minargs)) {
    ase_string s = get_name() + "(): "
      + ase_variant(minargs).get_string()
      + " arg(s) required";
    ase_throw_missing_arg(s.data());
  }
  if (maxargs >= 0 && args.size() > static_cast<ase_size_type>(maxargs)) {
    ase_string s = get_name() + "(): too many args";
    ase_throw_too_many_args(s.data());
  }
  for (ase_size_type i = 0; i < args.size(); ++i) {
    if (ase_variant::is_reference_type(args[i].get_type())) {
      get_asesyn(i);
    }
  }
}

void
asesynimpl::validate_string(ase_size_type i) const
{
  ase_validate_string(args[i], get_name().data());
}

template <ase_size_type n> void
asesynimpl::validate_syn(ase_size_type i, asestag tag) const
{
  asesyn& sy = get_asesyn(i);
  if (tag != sy.impl->get_tag()) {
    ase_string s = sy.impl->get_argstr(i) + ": type mismatch";
    ase_throw_type_mismatch(s.data());
  }
}

asesyn::asesyn(const ase_string& n, ASE_Component::Library& lib)
{
  const ase_size_type len = sizeof(asesyntbl) / sizeof(asesyntbl[0]);
  ase_size_type i;
  for (i = 0; i < len; ++i) {
    if (ase_string(asesyntbl[i].name) == n) {
      break;
    }
  }
  if (i >= len) {
    ase_string s = "asesyn: unknown constructor: " + n;
    ase_throw_type_mismatch(s.data());
  }
  impl.reset(asesyntbl[i].create_func()); /* dummy */
  impl->sidx = i;
}

asesyn::asesyn(const asesyn& klass, const ase_variant *args,
  ase_size_type nargs)
{
  /* Foo(name, Class...) */
  const ase_size_type sidx = klass.impl->sidx;
  DBG(fprintf(stderr, "asesyn %s\n", asesyntbl[sidx].name));
  impl.reset(asesyntbl[sidx].create_func());
  for (ase_size_type i = 0; i < nargs; ++i) {
    impl->args.push_back(args[i]);
  }
  impl->sidx = sidx;
  impl->validate_numargs();
  impl->validate();
}

ase_string
asesyn::tostring() const
{
  ase_string s(asesyntbl[impl->sidx].name);
  s = s + "(";
  for (ase_size_type i = 0; i < impl->args.size(); ++i) {
    if (i != 0) {
      s = s + ", ";
    }
    s = s + impl->args[i].get_string();
  }
  s = s + ")";
  return s;
}

ase_variant
asesyn::tostring(const ase_variant *args, ase_size_type nargs)
{
  return tostring().to_variant();
}

const ASE_Component::MethodEntry<asesyn> asesyn::ClassMethods[] = {
  { "Noop",   &ASE_Component::NoopImpl::Noop },
};
const ASE_Component::MethodEntry<asesyn> asesyn::InstanceMethods[] = {
  { "ToString",   &asesyn::tostring },
};

struct asecompiler : public ASE_Component::NoopImpl {

  asecompiler(const ase_string& n, ASE_Component::Library& l) : lib(l) { }
  asecompiler(const asecompiler& klass, const ase_variant *args,
    ase_size_type nargs) : lib(klass.lib) { }
  ase_variant compile_file(const ase_variant *args, ase_size_type nargs);

  ASE_Component::Library& lib;

  static const ASE_Component::MethodEntry<asecompiler> ClassMethods[];
  static const ASE_Component::MethodEntry<asecompiler> InstanceMethods[];

};

#ifdef __GNUC__

struct auto_aseparser : public ase_noncopyable {
  auto_aseparser(const char *filename) : ptr(0) {
    int notfound = 0;
    ptr = aseparser_create(filename, &notfound);
    if (ptr == 0) {
      if (notfound) {
	ase_string s = ase_string("file not found: ") + ase_string(filename);
	ase_throw_io_error(s.data());
      }
      throw std::bad_alloc();
    }
  }
  ~auto_aseparser() {
    aseparser_free(ptr);
  }
  aseparser *get() { return ptr; } 
 private:
  aseparser *ptr;
};

ase_string
token_to_string(const asetoken& t)
{
  if (t.begin == t.end) {
    return ase_string();
  }
  return ase_string(t.begin, t.end - t.begin);
}

ase_variant
token_to_number(const asetoken& t)
{
  ase_string s = token_to_string(t);
  if (s == "true") {
    return ase_variant(true);
  } else if (s == "false") {
    return ase_variant(false);
  }
  double d = std::strtod(s.data(), 0);
  return ase_variant(d);
}

const asepnode &
get_arg(aseparser *psr, const asepnode& nd, size_t argnum)
{
  if (argnum >= nd.nargs) {
    ase_throw_error("get_arg: missing argument");
  }
  const size_t i = nd.args[argnum];
  return *aseparser_get_node(psr, i);
}

ase_variant build_tree(ASE_Component::Library& lib, aseparser *psr,
  const asepnode& nd);

void
build_node_expand_list_rec(ASE_Component::Library& lib, aseparser *psr,
  const asepnode& nd, std::vector<ase_variant>& vec_r)
{
  if (nd.tag == asestag_i_nil) {
    return;
  }
  if (nd.tag != asestag_i_list) {
    ase_throw_error("expand_list: type mismatch");
  }
  ase_variant car = build_tree(lib, psr, get_arg(psr, nd, 0));
  vec_r.push_back(car);
  build_node_expand_list_rec(lib, psr, get_arg(psr, nd, 1), vec_r);
}

ase_variant
build_node_internal(ASE_Component::Library& lib, const char *cnstr,
  const std::vector<ase_variant>& args)
{
  ase_variant c = lib.GetClass(ase_string(cnstr));
  return c.invoke_self(args.empty() ? 0 : (&args[0]), args.size());
}

ase_variant
build_node(ASE_Component::Library& lib, aseparser *psr, const char *cnstr,
  const asepnode& nd, bool has_name)
{
  std::vector<ase_variant> args;
  if (has_name) {
    ase_string tstr = token_to_string(nd.token);
    args.push_back(tstr.to_variant());
  }
  for (size_t i = 0; i < nd.nargs; ++i) {
    const asepnode& cnd = get_arg(psr, nd, i);
    if (cnd.tag == asestag_i_list || cnd.tag == asestag_i_nil) {
      build_node_expand_list_rec(lib, psr, cnd, args);
    } else {
      args.push_back(build_tree(lib, psr, cnd));
    }
  }
  return build_node_internal(lib, cnstr, args);
}

ase_variant
build_tree(ASE_Component::Library& lib, aseparser *psr, const asepnode& nd)
{
  bool has_name = false;
  switch (nd.tag) {
  /* impossible */
  case asestag_i_list:
  case asestag_i_nil:
    ase_throw_error("build_tree: type mismatch");
  /* literals etc. */
  case asestag_i_string:
    return token_to_string(nd.token).to_variant();
  case asestag_i_number:
    return token_to_number(nd.token);
  /* normals */
  case asestag_klass:
  case asestag_method:
  case asestag_var:
  case asestag_lref:
  case asestag_gref:
  case asestag_let:
  case asestag_minvoke:
  case asestag_op:
  case asestag_cpp:
    has_name = true;
    break;
  default:
    ;
  }
  const char *cnstr = aseparser_tag2string(nd.tag);
  return build_node(lib, psr, cnstr, nd, has_name);
}

ase_variant
build_tree(ASE_Component::Library& lib, aseparser *psr)
{
  return build_tree(lib, psr,
    *aseparser_get_node(psr, aseparser_get_top(psr)));
}

ase_variant
asecompiler::compile_file(const ase_variant *args, ase_size_type nargs)
{
  ase_check_num_args(nargs, 1, "CompileFile: wrong number of arguments");
  ase_string fname = args[0].get_string();
  auto_aseparser psr(fname.data());
  if (aseparser_parse(psr.get()) != 0) {
    if (aseparser_is_oom(psr.get())) {
      throw std::bad_alloc();
    }
    int line = 0, col = 0;
    const char *err = aseparser_get_error(psr.get(), &line, &col);
    if (err == 0) {
      err = "unknown error";
    }
    ase_string estr = fname + "(" + ase_variant(line).get_string() + "."
      + ase_variant(col).get_string() + "): " + ase_string(err);
    ase_throw_exception("ASEParseError", estr);
  }
  return build_tree(lib, psr.get());
}

#else

ase_variant
asecompiler::compile_file(const ase_variant *args, ase_size_type nargs)
{
  return ase_variant();
}

#endif

const ASE_Component::MethodEntry<asecompiler> asecompiler::ClassMethods[] = {
  { "CompileFile",   &asecompiler::compile_file },
};
const ASE_Component::MethodEntry<asecompiler> asecompiler::InstanceMethods[] =
{
  { "Noop",   &ASE_Component::NoopImpl::Noop },
};

}; // anonymous namespace

void
ASE_NewVariantFactory_Syntax(ASE_VariantFactoryPtr& ap_r)
{
  static ASE_Component::NewClassFunc *const f =
    ASE_Component::NewClass<asesyn, asesyn, false>;
  static const ASE_Component::LibraryEntry ents[] = {
    { "Module",   f },
    { "Class",    f },
    { "Method",   f },
    { "Args",     f },
    { "Var",      f },
    { "Attr",     f },
    { "Seq",      f },
    { "While",    f },
    { "DoWhile",  f },
    { "For",      f },
    { "If",       f },
    { "Break",    f },
    { "Continue", f },
    { "Throw",    f },
    { "TryCatch", f },
    { "Return",   f },
    { "LRef",     f },
    { "GRef",     f },
    { "Let",      f },
    { "get",      f },
    { "set",      f },
    { "Op",       f },
    { "minvoke",  f },
    { "Closure",  f },
    { "Array",    f },
    { "Map",      f },
    { "Cpp",      f },
    { "Compiler", ASE_Component::NewClass<asecompiler, asecompiler, false> },
  };
  ASE_Component::NewLibraryFactory(ents, ap_r);
}

/*

  Module(Class...)
  Class(STRING, Member...)
  Member := Method | Var
  Method(STRING, Attr, Args, Sentence)
  Args(STRING...)
  Var(STRING, Attr)
  Attr(STRING...)
  Sentence := Seq | While | DoWhile | For | If | Break | Continue | Throw
           | TryCatch | Return | Expr
  Expr := LRef | GRef | Let | get | set | minvoke | Op | Closure | value
  Seq(Sentence...)
  While(Expr, Sentence)
  DoWhile(Sentence, Expr)
  For(Expr, Expr, Expr, Sentence)
  If(Expr, Sentence, Sentence)
  Break(NUMBER)
  Continue(NUMBER)
  Throw(Expr)
  TryCatch(STRING, Sentence, Sentence)
  Return(Expr)
  LRef(STRING)
  GRef(STRING)
  Let(STRING, Expr)
  get(Expr, Expr)
  set(Expr, Expr, Expr)
  minvoke(STRING, Expr, Expr...)
  Op(STRING, Expr...)
  Closure(Args, Sentence)
  value := STRING | NUMBER | Array | Map
  Array(value...)
  Map(STRING, Expr, ..., ...)
  Cpp(STRING)

*/

