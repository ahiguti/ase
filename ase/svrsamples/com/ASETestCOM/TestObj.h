
#pragma once
#include "resource.h"


// ITestObj
[
  object,
  uuid("F192B126-268B-4EFE-8167-9C6F00247B23"),
  dual,
  helpstring("ITestObj Interface"),
  pointer_default(unique)
]
__interface ITestObj : IDispatch
{
  [id(1), helpstring("Method SetInt")] HRESULT SetInt([in] LONG x);
  [id(2), helpstring("Method get_int")] HRESULT get_int([out,retval] LONG* r);
  [id(3), helpstring("Method AddInt")] HRESULT AddInt([in] LONG x);
  [id(4), helpstring("Method SetString")] HRESULT SetString([in] BSTR x);
  [id(5), helpstring("Method get_string")] HRESULT get_string([out,retval] BSTR* r);
  [id(6), helpstring("Method AddString")] HRESULT AddStrlen([in] BSTR x);
  [id(7), helpstring("Method SetCallback")] HRESULT SetCallback([in] IDispatch* x);
  [id(8), helpstring("Method CallbackInt")] HRESULT CallbackInt([in] LONG count, [in] LONG x, [in] LONG y, [in] LONG z, [out,retval] LONG* r);
  [id(9), helpstring("Method CallbackString")] HRESULT CallbackString([in] LONG count, [in] BSTR x, [in] BSTR y, [in] BSTR z, [out,retval] BSTR* r);
};


// CTestObj

[
  coclass,
  default(ITestObj),
  threading(apartment),
  aggregatable(never),
  vi_progid("ASETestCOM.TestObj"),
  progid("ASETestCOM.TestObj.1"),
  version(1.0),
  uuid("8B55D1DB-E059-41A7-A00A-B06369E96F24"),
  helpstring("TestObj Class")
]
class ATL_NO_VTABLE CTestObj :
  public ITestObj
{

public:

  CTestObj()
    : intval(0)
  {
  }

  DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

private:

  LONG intval;
  CComBSTR strval;
  CComPtr<IDispatch> callback;

public:

  STDMETHOD(SetInt)(LONG x);
  STDMETHOD(get_int)(LONG* r);
  STDMETHOD(AddInt)(LONG x);
  STDMETHOD(SetString)(BSTR x);
  STDMETHOD(get_string)(BSTR* r);
  STDMETHOD(AddStrlen)(BSTR x);
  STDMETHOD(SetCallback)(IDispatch* x);
  STDMETHOD(CallbackInt)(LONG count, LONG x, LONG y, LONG z, LONG* r);
  STDMETHOD(CallbackString)(LONG count, BSTR x, BSTR y, BSTR z, BSTR* r);

};

