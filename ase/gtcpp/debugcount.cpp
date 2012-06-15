
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <gtcpp/debugcount.hpp>
#include <iostream>

namespace gtcpp {

void
dump_debugcount()
{
  #ifndef NDEBUG
  std::cerr << "error c " << error_constr_count << std::endl;
  std::cerr << "error d " << error_destr_count << std::endl;
  std::cerr << "file  c " << file_constr_count << std::endl;
  std::cerr << "file  d " << file_destr_count << std::endl;
  std::cerr << "xm    c " << xm_constr_count << std::endl;
  std::cerr << "xm    d " << xm_destr_count << std::endl;
  #endif
}

};

