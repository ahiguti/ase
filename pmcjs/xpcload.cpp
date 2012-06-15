
/*
 *  This file is part of pmcjs.
 *  Copyright (C) 2006 A.Higuchi. All rights reserved.
 *  See COPYRIGHT.txt for details.
 */

#include <boost/preprocessor.hpp>
#include <nsCOMPtr.h>
#include <nsXPCOM.h>
#include <nsIInterfaceInfo.h>
#include <nsIInterfaceInfoManager.h>
#include <nsIServiceManager.h>
#include <nsIComponentManager.h>
#include <nsIComponentRegistrar.h>
#include <nsIGenericFactory.h>
#include <nsServiceManagerUtils.h>
#include <nsComponentManagerUtils.h>

#include <cstdio>
#include <cstdlib>
#include <dlfcn.h>

static void
die_unless(int c, const char *mess)
{
  if (!c) {
    fprintf(stderr, "Fatal: %s\n", mess);
    exit(1);
  }
}

static int
xpcload_main(const char *name)
{
  const char *mozhome = BOOST_PP_STRINGIZE(USE_XPCOM);
  setenv("MOZILLA_FIVE_HOME", mozhome, 0);
  nsCOMPtr<nsIServiceManager> servman;
  nsresult rv = NS_InitXPCOM2(getter_AddRefs(servman), nsnull, nsnull);
  die_unless(NS_SUCCEEDED(rv), "XPCOM init");

  nsCOMPtr<nsISupports> obj = do_CreateInstance(name, &rv);
  if (NS_SUCCEEDED(rv)) {
    fprintf(stderr, "success\n");
  } else {
    fprintf(stderr, "failed\n");
    const char *errstr = dlerror();
    fprintf(stderr, "dlerror: %s\n", errstr);
  }

  rv = NS_ShutdownXPCOM(NULL);
  return 0;
}

int main(int argc, char **argv)
{
  if (argc < 2) {
    printf("usage: %s classname\n", argv[0]);
    return 1;
  }
  int r = xpcload_main(argv[1]);
  return r;
}


