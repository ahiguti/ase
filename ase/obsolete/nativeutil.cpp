
/*
 *  This file is part of ASE, the abstract script engines.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <ase/asecpnt.hpp>

ASE_Component::NewClassFunc ASE_NewUtilMetaClass;
ASE_Component::NewClassFunc ASE_NewUtilThreadClass;
ASE_Component::NewClassFunc ASE_NewUtilUUIDClass;
ASE_Component::NewClassFunc ASE_NewUtilUTFClass;
ASE_Component::NewClassFunc ASE_NewUtilTestClass;

#ifndef __GNUC__
#define ASENATIVEUTIL_DLLEXT __declspec(dllexport)
#else
#define ASENATIVEUTIL_DLLEXT
#endif

extern "C" {

ASENATIVEUTIL_DLLEXT void
ASE_NewVariantFactory_Util(ASE_VariantFactoryPtr& ap_r)
{
  static const ASE_Component::LibraryEntry ents[] = {
    { "Meta", ASE_NewUtilMetaClass },
    { "Thread", ASE_NewUtilThreadClass },
    { "UUID", ASE_NewUtilUUIDClass },
    { "UTF", ASE_NewUtilUTFClass },
    { "Test", ASE_NewUtilTestClass },
  };
  ASE_Component::NewLibraryFactory(ents, ap_r);
}

};

