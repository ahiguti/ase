
#include <iostream>
#include <string>
#include <boost/noncopyable.hpp>

#define MOZILLA_STRICT_API

#include "nsIGenericFactory.h"
#include "nsCOMPtr.h"
#include "nsXPCOM.h"
#include "nsIServiceManager.h"
#include "nsIClassInfo.h"
#include "nsMemory.h"
#include "nsWeakReference.h"
#include "nsCRT.h"
#include "itxpc.h"
#include "itxpccallback.h"

#define DBG(x)


#define TXPC_CID \
{ 0x6d097b3d, 0x12d5, 0x41fe, \
{ 0xaf, 0xea, 0xb8, 0xc2, 0xb8, 0x45, 0xfc, 0x3e}}

#define TXPC_ContractID "txpc"

class TXPC: public ITXPC, public nsIClassInfo, public boost::noncopyable {

 public:

  TXPC();
  virtual ~TXPC();

  NS_DECL_ITXPC
  NS_DECL_NSICLASSINFO
  NS_DECL_ISUPPORTS

 private:
 
  PRUint32 value;
  bool verbose;
  #if 0
  nsCOMPtr<ITXPCCallback> saved_cb_strong; /* can cause cyclic reference */
  #endif
  nsWeakPtr saved_cb_weak;

};

class CPPCallback:
  public ITXPCCallback, public nsSupportsWeakReference,
  public boost::noncopyable {

 public:

  CPPCallback(const std::string& mess, bool verb);
  virtual ~CPPCallback();

  NS_DECL_ITXPCCALLBACK
  NS_DECL_ISUPPORTS

 private:
 
  std::string message;
  const bool verbose;

};

TXPC::TXPC()
  : value(0), verbose(false)
{
  DBG(verbose = true);
  #if 0
  std::cerr << "sizeof nsWeakReference: " << sizeof(nsWeakReference)
    << std::endl;
  #endif
  if (verbose) {
    std::cerr << "TXPC constructor" << std::endl;
  }
}

TXPC::~TXPC()
{
  if (verbose) {
    std::cerr << "TXPC destructor" << std::endl;
  }
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::SetVerbose(PRBool v)
{
  verbose = v;
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::SetValue(PRUint32 v)
{
  value = v;
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::GetValue(PRUint32 *rval)
{
  (*rval) = value;
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::CreateCallback(PRBool v, ITXPCCallback **cb)
{
  CPPCallback *p = new CPPCallback("c++ function", v);
  p->AddRef();
  (*cb) = p;
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::CreateSaveCallback(PRBool v)
{
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::DoCallback(ITXPCCallback *cb, PRUint32 num)
{
  for (PRUint32 i = 0; i < num; ++i) {
    cb->Callback();
  }
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::SaveCallback(ITXPCCallback *cb, PRBool weak)
{
  if (weak) {
    nsWeakPtr wptr = getter_AddRefs(NS_GetWeakReference(cb));
    if (!wptr) {
      fprintf(stderr, "failed to get wptr\n");
    }
    saved_cb_weak = wptr;
#if 0
  } else {
    saved_cb_strong = cb;
#endif
  }
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::DoSavedCallback(PRUint32 num)
{
  if (saved_cb_weak) {
    nsresult rv;
    nsCOMPtr<ITXPCCallback> cb = do_QueryReferent(saved_cb_weak);
    if (!cb) {
      fprintf(stderr, "saved weak reference becomes null\n");
      return NS_OK;
    }
    for (PRUint32 i = 0; i < num; ++i) {
      rv = cb->Callback();
      if (NS_FAILED(rv)) {
	fprintf(stderr, "callback failed\n");
	break;
      }
    }
#if 0
  } else if (saved_cb_strong) {
    for (PRUint32 i = 0; i < num; ++i) {
      saved_cb_strong->Callback();
    }
#endif
  } else {
    fprintf(stderr, "no callback is set\n");
  }
  return NS_OK;
}

NS_IMETHODIMP /* interface ITXPC */
TXPC::ClearSavedCallback()
{
#if 0
  saved_cb_strong = NULL;
#endif
  saved_cb_weak = NULL;
  return NS_OK;
}

static const nsIID itxpc_iid = ITXPC_IID;

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetInterfaces(PRUint32 *count, nsIID * **array)
{
  if (verbose) {
    std::cerr << "TXPC GetInterfaces" << std::endl;
  }
  (*count) = 1;
  (*array) = NS_STATIC_CAST(nsIID **, nsMemory::Alloc(1 * sizeof(nsIID *)));
  NS_ENSURE_TRUE(*array, NS_ERROR_OUT_OF_MEMORY);
  nsIID *iid = NS_STATIC_CAST(nsIID *, nsMemory::Clone(
    &itxpc_iid, sizeof(nsIID)));
  if (!iid) {
    nsMemory::Free(*array);
    (*count) = 0;
    (*array) = nsnull;
    return NS_ERROR_OUT_OF_MEMORY;
  }
  (*array)[0] = iid;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetHelperForLanguage(PRUint32 language, nsISupports * *retval)
{
  if (verbose) {
    std::cerr << "TXPC GetHelperForLanguage" << std::endl;
  }
  (*retval) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetContractID(char * *aContractID)
{
  if (verbose) {
    std::cerr << "TXPC GetContractID" << std::endl;
  }
  (*aContractID) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassDescription(char * *aClassDescription)
{
  if (verbose) {
    std::cerr << "TXPC GetClassDescription" << std::endl;
  }
  (*aClassDescription) = nsCRT::strdup("TXPC");
  return (*aClassDescription) ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassID(nsCID * *aClassID)
{
  if (verbose) {
    std::cerr << "TXPC GetClassID" << std::endl;
  }
  (*aClassID) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetImplementationLanguage(PRUint32 *aImplementationLanguage)
{
  if (verbose) {
    std::cerr << "TXPC GetImplementationLanguage" << std::endl;
  }
  (*aImplementationLanguage) = nsIProgrammingLanguage::UNKNOWN;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetFlags(PRUint32 *aFlags)
{
  if (verbose) {
    std::cerr << "TXPC GetFlags" << std::endl;
  }
  //(*aFlags) = nsIClassInfo::DOM_OBJECT;
  (*aFlags) = 0;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassIDNoAlloc(nsCID *aClassIDNoAlloc)
{
  if (verbose) {
    std::cerr << "TXPC GetClassIDNoAlloc" << std::endl;
  }
  return NS_ERROR_NOT_AVAILABLE;
}

NS_IMPL_ISUPPORTS2(TXPC, ITXPC, nsIClassInfo);

CPPCallback::CPPCallback(const std::string& mess, bool verb)
  : message(mess), verbose(verb)
{
//  if (verbose) {
    std::cerr << "CPPCallback constructor" << std::endl;
//  }
}

CPPCallback::~CPPCallback()
{
//  if (verbose) {
    std::cerr << "CPPCallback destructor" << std::endl;
//  }
}

NS_IMETHODIMP /* interface ICallback */
CPPCallback::Callback()
{
  if (verbose) {
    std::cout << "CPPCallback: " << message << std::endl;
  }
  return NS_OK;
}

NS_IMPL_ISUPPORTS2(CPPCallback, ITXPCCallback, nsISupportsWeakReference);

static NS_METHOD
TXPCRegistration(nsIComponentManager *aCompMgr, nsIFile *aPath,
  const char *registryLocation, const char *componentType,
  const nsModuleComponentInfo *info)
{
  DBG(std::cerr << "TXPC registration" << std::endl);
  nsresult rv;
  nsCOMPtr<nsIServiceManager> servman =
    do_QueryInterface((nsISupports*)aCompMgr, &rv);
  if (NS_FAILED(rv)) {
    return rv;
  }
  return rv;
}

static NS_METHOD
TXPCUnregistration(nsIComponentManager *aCompMgr, nsIFile *aPath,
  const char *registryLocation, const nsModuleComponentInfo *info)
{
  DBG(std::cerr << "TXPC unregistration" << std::endl);
  nsresult rv;
  nsCOMPtr<nsIServiceManager> servman =
    do_QueryInterface((nsISupports*)aCompMgr, &rv);
  if (NS_FAILED(rv)) {
    return rv;
  }
  return rv;
}

NS_GENERIC_FACTORY_CONSTRUCTOR(TXPC)

static const nsModuleComponentInfo components[] = {
  {
    "TXPC",
    TXPC_CID,
    TXPC_ContractID,
    TXPCConstructor,
    TXPCRegistration,
    TXPCUnregistration,
  }
};

NS_IMPL_NSGETMODULE(TXPCModule, components)

