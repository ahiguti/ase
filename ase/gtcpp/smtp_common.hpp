
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GTCPP_SMTP_COMMON_HPP
#define GTCPP_SMTP_COMMON_HPP

#include <gtcpp/string_buffer.hpp>
#include <gtcpp/string_ref.hpp>

namespace gtcpp {

void smtp_escape_body(string_buffer& out, const string_ref& body_unescaped);
void smtp_unescape_body(string_buffer& out, const string_ref& body_escaped);

};

#endif

