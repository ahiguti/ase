
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEUTF16_HPP
#define GENTYPE_ASEUTF16_HPP

#include <ase/ase.hpp>

class ase_utf16_string : public ase_noncopyable {

 public:

  struct permissive { };

  ASELIB_DLLEXT ase_utf16_string(const char *str, ase_size_type len,
    const permissive&);
  ASELIB_DLLEXT ase_utf16_string(const char *str, ase_size_type len);
  ASELIB_DLLEXT ase_utf16_string(const ase_string& str);
  ASELIB_DLLEXT ase_utf16_string(const std::string& str);
  ASELIB_DLLEXT ~ase_utf16_string(); /* DCHK */

  const ase_ucchar *data() const { return buffer; }
  ase_ucchar *DataWr() { return buffer; }
  ase_size_type size() const { return length; }

 private:

  ase_size_type length;
  ase_ucchar *buffer;

  void init(const char *str, ase_size_type len, bool permissive);

};

#endif

