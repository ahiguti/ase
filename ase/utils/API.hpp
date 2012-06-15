
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

/* C++ */
class ase_variant {

  void add_ref();  /* private */
  void release(); /* private */
  ase_int get_attributes();
  bool get_boolean();
  ase_int get_int();
  ase_long get_long();
  double get_double();
  ase_string get_string();
  ase_variant get_property(const ase_string& name);
  void set_property(const ase_string& name, const ase_variant& value);
  void del_property(const ase_string& name);
  ase_variant get_element(ase_int idx);
  void set_element(ase_int idx, const ase_variant& value);
  ase_int get_length();
  void set_length(ase_int value);
  ase_variant invoke_by_name(const ase_string& name, const ase_variant *args,
    ase_size_type nargs);
  ase_variant invoke_by_id(ase_int id, const ase_variant *args,
    ase_size_type nargs);
  ase_variant invoke_self(const ase_variant *args, ase_size_type nargs);
  ase_variant get_enumerator();
  ase_string enum_next(bool& hasnext_r);
  ase_variant GetWeak();
  ase_variant GetStrong();

  ase_variant get(const char (& name)[]);
  ase_variant get(ase_int idx);
  void set(const char (& name)[]);
  void set(ase_int idx);
  ase_variant minvoke(const char (& name)[], const ase_variant& arg0, ...);
  ase_variant minvoke(ase_int id, const ase_variant& arg0, ...);
  ase_variant invoke(const ase_variant& arg0, ...);

};

/* Java */
package asejni;
public interface LateBinding {

  int get_attributes();
  String get_string();
  Object get_property(String name);
  void set_property(String name, Object value);
  void del_property(String name);
  Object get_element(int idx);
  void set_element(int idx, Object value);
  int get_length();
  void set_length(int len);
  Object invoke_by_name(String name, Object[] args);
  // Object invoke_by_id(int id, Object[] args);
  Object invoke_self(Object[] args);
  Object get_enumerator();
  String enum_next();
  // Object GetWeak();
  // Object GetStrong();

};

