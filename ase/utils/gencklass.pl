#!/usr/bin/env perl

use warnings;
use strict;

gen_tmpl_initializer(8);

sub gen_tmpl_initializer {
  my $nargs = $_[0];
  print "/* THIS FILE IS GENERATED. DON'T EDIT. */\n";
  print "#ifdef ASE_CCLASS_GEN_INC\n";
  print "template <typename T> struct ase_cclass<T>::initializer : ";
  print "private ase_noncopyable {\n";
  gen_genericfunc();
  for (my $i = 0; $i < $nargs; ++$i) {
    gen_tmpl_memfunc($i, 0, 1, 0); # klass, returning void
    gen_tmpl_memfunc($i, 1, 1, 0); # klass, returning non-void
    gen_tmpl_memfunc($i, 0, 0, 0); # instance method, returning void
    gen_tmpl_memfunc($i, 1, 0, 0); # instance method, returning non-void
    gen_tmpl_memfunc($i, 0, 0, 1); # const instance method, returning void
    gen_tmpl_memfunc($i, 1, 0, 1); # const instance method, returning non-void
  }
  print "  ";
  print "initializer(ase_int opt = ase_vtoption_none) : option(opt) { }\n";
  print "  ";
  print "~initializer() { initialize_internal(*this); /* throws */ }\n";
  print "  ";
  print "const ase_int option;\n";
  print "  ";
  print "ase_cclass_base::cklass_method_entries_type me;\n";
  print "};\n";
  print "#endif\n";
}

sub gen_genericfunc {
  # ase_variant (*)(const ase_variant *, ase_size_type);
  print "  static ase_variant callkfg(const ase_variant& selfv, ";
  print "ase_cclass_base::cklass_klsfun_type kf, ";
  print "const ase_variant *a, ase_size_type na) {\n";
  print "    ase_variant (*const tkf)(const ase_variant *, ";
  print "const ase_size_type) = reinterpret_cast<ase_variant (*)(";
  print "const ase_variant *, const ase_size_type)>(kf);\n";
  print "    return (*tkf)(a, na);\n";
  print "  }\n";
  print "  initializer& def(const char *n, ase_variant (*kfp)(";
  print "const ase_variant *, ase_size_type)) {\n";
  print "    ase_cclass_base::add(me, n, callkfg, reinterpret_cast<";
  print "ase_cclass_base::cklass_klsfun_type>(kfp));\n";
  print "    return *this;\n";
  print "  }\n";
  # ase_variant (*)(const ase_variant&, const ase_variant *, ase_size_type);
  print "  static ase_variant callkfgs(const ase_variant& selfv, ";
  print "ase_cclass_base::cklass_klsfun_type kf, ";
  print "const ase_variant *a, ase_size_type na) {\n";
  print "    ase_variant (*const tkf)(const ase_variant&, ";
  print "const ase_variant *, const ase_size_type) ";
  print "= reinterpret_cast<ase_variant (*)(";
  print "const ase_variant&, const ase_variant *, ";
  print "const ase_size_type)>(kf);\n";
  print "    return (*tkf)(selfv, a, na);\n";
  print "  }\n";
  print "  initializer& def(const char *n, ase_variant (*kfp)(";
  print "const ase_variant&, const ase_variant *, ase_size_type)) {\n";
  print "    ase_cclass_base::add(me, n, callkfgs, reinterpret_cast<";
  print "ase_cclass_base::cklass_klsfun_type>(kfp));\n";
  print "    return *this;\n";
  print "  }\n";
  # ase_variant (T::*)(const ase_variant *, ase_size_type);
  print "  static ase_variant callmfg(const ase_variant& selfv, void *self, ";
  print "ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ";
  print "ase_size_type na) {\n";
  print "    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(";
  print "self);\n";
  print "    ase_variant (T::*const tmf)(const ase_variant *, ase_size_type) ";
  print "= reinterpret_cast<ase_variant (T::*)(const ase_variant *, ";
  print "ase_size_type)>(mf);\n";
  print "    return (tself->value.*tmf)(a, na);\n";
  print "  }\n";
  print "  initializer& def(const char *n, ase_variant (T::*mfp)(";
  print "const ase_variant *, ase_size_type)) {\n";
  print "    ase_cclass_base::add(me, n, callmfg, reinterpret_cast<";
  print "ase_cclass_base::cklass_memfun_type>(mfp));\n";
  print "    return *this;\n";
  print "  }\n";
  # ase_variant (T::*)(const ase_variant&, const ase_variant *, ase_size_type);
  print "  static ase_variant callmfgs(const ase_variant& selfv, void *self, ";
  print "ase_cclass_base::cklass_memfun_type mf, const ase_variant *a, ";
  print "ase_size_type na) {\n";
  print "    ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(";
  print "self);\n";
  print "    ase_variant (T::*const tmf)(const ase_variant&, ";
  print "const ase_variant *, ase_size_type) ";
  print "= reinterpret_cast<ase_variant (T::*)(const ase_variant&, ";
  print "const ase_variant *, ase_size_type)>(mf);\n";
  print "    return (tself->value.*tmf)(selfv, a, na);\n";
  print "  }\n";
  print "  initializer& def(const char *n, ase_variant (T::*mfp)(";
  print "const ase_variant&, const ase_variant *, ase_size_type)) {\n";
  print "    ase_cclass_base::add(me, n, callmfgs, reinterpret_cast<";
  print "ase_cclass_base::cklass_memfun_type>(mfp));\n";
  print "    return *this;\n";
  print "  }\n";
}

sub gen_tmpl_memfunc {
  my ($nargs, $has_ret, $is_km, $is_const) = @_;
  print "  ";
  my $targdecl = gen_targs($nargs, $has_ret, 1);
  print "$targdecl" . "static ase_variant ";
  my $cfname;
  if ($is_km) {
    $cfname = $has_ret ? "callkfr" : "callkfv";
    print "$cfname(const ase_variant&, ";
    print "ase_cclass_base::cklass_klsfun_type kf, ";
    print "const ase_variant *a, ase_size_type na) {\n";
  } else {
    $cfname = $is_const
      ? ($has_ret ? "callmfrc" : "callmfvc")
      : ($has_ret ? "callmfr" : "callmfv");
    print "$cfname(const ase_variant&, void *self, ";
    print "ase_cclass_base::cklass_memfun_type mf, ";
    print "const ase_variant *a, ase_size_type na) {\n";
  }
  if (!$is_km) {
    gen_indent();
    print "ase_cclass<T> *const tself = static_cast<ase_cclass<T> *>(self);\n";
  }
  gen_indent();
  my $targs = gen_args($nargs, "A");
  my $rt = $has_ret ? "R" : "void";
  if ($is_km) {
    print "$rt (*const tkf)($targs) = ";
    print "reinterpret_cast<$rt (*)($targs)>(kf);\n";
  } elsif ($is_const) {
    print "$rt (T::*const tmf)($targs) const = ";
    print "reinterpret_cast<$rt (T::*)($targs) const>(mf);\n";
  } else {
    print "$rt (T::*const tmf)($targs) = ";
    print "reinterpret_cast<$rt (T::*)($targs)>(mf);\n";
  }
  gen_indent();
  print "if (na != $nargs) { ase_cclass_base::wrong_num_args(na, $nargs); }\n";
  for (my $i = 0; $i < $nargs; ++$i) {
    gen_indent();
    my $argt = "typename ase_marshal::arg_decl<A$i>::type";
    print "$argt ta$i = ase_marshal::from_variant<A$i>::type::func(a[$i]);\n";
  }
  gen_indent();
  if ($has_ret) {
    print "R tr = ";
  }
  my $cargs = gen_args($nargs, "ta");
  if ($is_km) {
    print "(*tkf)($cargs);\n";
  } else {
    print "(tself->value.*tmf)($cargs);\n";
  }
  gen_indent();
  if ($has_ret) {
    print "return ase_marshal::to_variant<R>::type::func(tr);\n";
  } else {
    print "return ase_variant();\n";
  }
  print "  ";
  print "}\n";
  print "  ";
  print "$targdecl" . "initializer& ";
  if ($is_km) {
    print "def(const char *n, $rt (*kfp)($targs)) {\n";
  } elsif ($is_const) {
    print "def(const char *n, $rt (T::*mfp)($targs) const) {\n";
  } else {
    print "def(const char *n, $rt (T::*mfp)($targs)) {\n";
  }
  gen_indent();
  my $tai = gen_targs($nargs, $has_ret, 0);
  print "ase_cclass_base::add(me, n, $cfname$tai, ";
  if ($is_km) {
    print "reinterpret_cast<ase_cclass_base::cklass_klsfun_type>(kfp));\n";
  } else {
    print "reinterpret_cast<ase_cclass_base::cklass_memfun_type>(mfp));\n";
  }
  gen_indent();
  print "return *this;\n";
  print "  ";
  print "}\n";
}

sub gen_indent {
  print "    ";
}

sub gen_args {
  my ($nargs, $name) = @_;
  my $r = "";
  for (my $i = 0; $i < $nargs; ++$i) {
    $r .= "$name$i";
    if ($i + 1 < $nargs) {
      $r .= ", ";
    }
  }
  return $r;
}

sub gen_targs {
  my ($nargs, $has_ret, $decl) = @_;
  my $r = "";
  if ($nargs > 0 || $has_ret) {
    $r .= $decl ? "template <" : "<";
    if ($has_ret) {
      $r .= $decl ? "typename R" : "R";
      if ($nargs > 0) {
	$r .= ", ";
      }
    }
    for (my $i = 0; $i < $nargs; ++$i) {
      $r .= $decl ? "typename A" : "A";
      $r .= "$i";
      if ($i + 1 < $nargs) {
	$r .= ", ";
      }
    }
    $r .= $decl ? "> " : ">";
  }
  return $r;
}

