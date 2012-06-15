
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECLR_WRAPPER_HPP
#define GENTYPE_ASECLR_WRAPPER_HPP

#include <vcclr.h>
#include <ase/ase.hpp>

namespace aseclr {

ref struct ValueWrapper {

  ValueWrapper(System::Type ^t, System::Object ^v) : typ(t), val(nullptr) {
      val = System::Convert::ChangeType(v, typ);
  }
  property System::Object ^value {
    void set(System::Object ^v) {
      val = System::Convert::ChangeType(v, typ);
    }
    System::Object ^get() {
      return val;
    }
  }

 private:

  System::Type ^typ;
  System::Object ^val;

};

}; // namespace aseclr

#endif

