
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_DEBUGCOUNT_HPP
#define GTCPP_DEBUGCOUNT_HPP

#include <gtcpp/atomicity.hpp>

namespace gtcpp {

extern int error_constr_count;
extern int error_destr_count;
extern int file_constr_count;
extern int file_destr_count;
extern int xm_constr_count;
extern int xm_destr_count;

void dump_debugcount();

};

#endif

