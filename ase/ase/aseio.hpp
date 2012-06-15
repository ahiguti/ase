
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#ifndef GENTYPE_ASEIO_HPP
#define GENTYPE_ASEIO_HPP

#include <ase/ase.hpp>

#include <vector>
#include <string>

ASELIB_DLLEXT ase_string ase_getenv_mtunsafe(const char *key);
ASELIB_DLLEXT ase_variant ase_read_directory(const ase_string& dpath);
ASELIB_DLLEXT ase_variant ase_glob(const ase_string& pattern);
ASELIB_DLLEXT bool ase_file_exists(const ase_string& fname);
ASELIB_DLLEXT bool ase_change_directory(const ase_string& dname);
ASELIB_DLLEXT ase_string ase_getcwd();
ASELIB_DLLEXT void *ase_get_current_thread_id();
ASELIB_DLLEXT bool ase_newer_than(const ase_string& f1, const ase_string& f2);
ASELIB_DLLEXT ase_string ase_load_file(const ase_string& fname);

#ifdef __GNUC__
#define ASE_DIR_SEP_CHAR '/'
#define ASE_DIR_SEP_STR  "/"
#define ASE_PATH_SEP_CHAR ':'
#define ASE_PATH_SEP_STR ":"
#define ASE_DLL_SUFFIX ".so"
#define ASE_STATICLIB_SUFFIX ".a"
#else
#define ASE_DIR_SEP_CHAR '\\'
#define ASE_DIR_SEP_STR  "\\"
#define ASE_PATH_SEP_CHAR ';'
#define ASE_PATH_SEP_STR ";"
#define ASE_DLL_SUFFIX ".dll"
#define ASE_STATICLIB_SUFFIX ".lib"
#endif

#endif

