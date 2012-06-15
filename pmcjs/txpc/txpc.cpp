
#include <iostream>

#define MOZILLA_STRICT_API

#include "nsIGenericFactory.h"
#include "nsCOMPtr.h"
#include "nsXPCOM.h"
#include "nsIServiceManager.h"
#include "nsIClassInfo.h"
#include "nsMemory.h"
#include "nsCRT.h"
#include "itxpc.h"

#define DBG(x)


#define TXPC_CID \
{ 0x6d097b3d, 0x12d5, 0x41fe, \
{ 0xaf, 0xea, 0xb8, 0xc2, 0xb8, 0x45, 0xfc, 0x3e}}

#define TXPC_ContractID "txpc"

class TXPC: public ITXPC, public nsIClassInfo {

 public:

  TXPC();
  virtual ~TXPC();

  NS_DECL_ITXPC
  NS_DECL_NSICLASSINFO
  NS_DECL_ISUPPORTS

 private:
 
  PRUint32 value;

};

TXPC::TXPC()
  : value(0)
{
  DBG(std::cerr << "TXPC constructor" << std::endl);
  NS_INIT_ISUPPORTS();
}

TXPC::~TXPC()
{
  DBG(std::cerr << "TXPC destructor" << std::endl);
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

static const nsIID itxpc_iid = ITXPC_IID;

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetInterfaces(PRUint32 *count, nsIID * **array)
{
  DBG(std::cerr << "TXPC GetInterfaces" << std::endl);
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
  DBG(std::cerr << "TXPC GetHelperForLanguage" << std::endl);
  (*retval) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetContractID(char * *aContractID)
{
  DBG(std::cerr << "TXPC GetContractID" << std::endl);
  (*aContractID) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassDescription(char * *aClassDescription)
{
  DBG(std::cerr << "TXPC GetClassDescription" << std::endl);
  (*aClassDescription) = nsCRT::strdup("TXPC");
  return (*aClassDescription) ? NS_OK : NS_ERROR_OUT_OF_MEMORY;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassID(nsCID * *aClassID)
{
  DBG(std::cerr << "TXPC GetClassID" << std::endl);
  (*aClassID) = nsnull;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetImplementationLanguage(PRUint32 *aImplementationLanguage)
{
  DBG(std::cerr << "TXPC GetImplementationLanguage" << std::endl);
  (*aImplementationLanguage) = nsIProgrammingLanguage::UNKNOWN;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetFlags(PRUint32 *aFlags)
{
  DBG(std::cerr << "TXPC GetFlags" << std::endl);
  //(*aFlags) = nsIClassInfo::DOM_OBJECT;
  (*aFlags) = 0;
  return NS_OK;
}

NS_IMETHODIMP /* interface nsIClassInfo */
TXPC::GetClassIDNoAlloc(nsCID *aClassIDNoAlloc)
{
  DBG(std::cerr << "TXPC GetClassIDNoAlloc" << std::endl);
  return NS_ERROR_NOT_AVAILABLE;
}


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

NS_IMPL_ISUPPORTS2(TXPC, ITXPC, nsIClassInfo);

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

