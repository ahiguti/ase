
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASECAST_HPP
#define GENTYPE_ASECAST_HPP

#include <stdexcept>

template <typename Tto, typename Tfrom> Tto
ase_numeric_cast(Tfrom v) {
  Tto r = static_cast<Tto>(v);
  Tfrom rx = static_cast<Tfrom>(r);
  if (rx != v) {
    throw std::runtime_error("numeric cast failed"); /* ASE THROW */
  }
  return r;
}

#endif

