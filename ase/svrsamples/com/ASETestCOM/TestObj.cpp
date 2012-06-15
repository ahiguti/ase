
#include "stdafx.h"
#include "TestObj.h"


// CTestObj


STDMETHODIMP CTestObj::SetInt(LONG x)
{
  intval = x;
  return S_OK;
}

STDMETHODIMP CTestObj::get_int(LONG* r)
{
  (*r) = intval;
  return S_OK;
}

STDMETHODIMP CTestObj::AddInt(LONG x)
{
  intval += x;
  return S_OK;
}

STDMETHODIMP CTestObj::SetString(BSTR x)
{
  strval = x;
  return S_OK;
}

STDMETHODIMP CTestObj::get_string(BSTR* r)
{
  strval.CopyTo(r);
  return S_OK;
}

STDMETHODIMP CTestObj::AddStrlen(BSTR x)
{
  intval += SysStringLen(x);
  return S_OK;
}

STDMETHODIMP CTestObj::SetCallback(IDispatch* x)
{
  callback = x;
  return S_OK;
}

STDMETHODIMP CTestObj::CallbackInt(LONG count, LONG x, LONG y, LONG z,
  LONG* r)
{
  (*r) = 0;
  if (callback == NULL) {
    return E_INVALIDARG;
  }
  WCHAR *name = L"CallInt";
  DISPID dispid = 0;
  HRESULT hr = callback->GetIDsOfNames(IID_NULL, &name, 1,
    LOCALE_SYSTEM_DEFAULT, &dispid);
  if (FAILED(hr)) {
    return hr;
  }
  WORD flags = DISPATCH_PROPERTYGET | DISPATCH_METHOD;
  DISPPARAMS params;
  memset(&params, 0, sizeof(params));
  params.cArgs = 3;
  VARIANTARG args[3];
  params.rgvarg = args;
  params.rgvarg[0].vt = VT_I4;
  params.rgvarg[0].lVal = x;
  params.rgvarg[1].vt = VT_I4;
  params.rgvarg[1].lVal = y;
  params.rgvarg[2].vt = VT_I4;
  params.rgvarg[2].lVal = z;
  CComVariant result;
  for (LONG i = 0; i < count; ++i) {
    CComVariant r;
    hr = callback->invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, flags,
      &params, &r, NULL, NULL);
    if (FAILED(hr)) {
      return hr;
    }
    result = r;
  }
  USHORT fl = 0;
  CComVariant vr;
  hr = VariantChangeType(&vr, &result, fl, VT_I4);
  if (SUCCEEDED(hr)) {
    (*r) = vr.intVal;
  }
  return hr;
}

STDMETHODIMP CTestObj::CallbackString(LONG count, BSTR x, BSTR y, BSTR z,
  BSTR* r)
{
  (*r) = 0;
  if (callback == NULL) {
    return E_INVALIDARG;
  }
  WCHAR *name = L"CallString";
  DISPID dispid = 0;
  HRESULT hr = callback->GetIDsOfNames(IID_NULL, &name, 1,
    LOCALE_SYSTEM_DEFAULT, &dispid);
  if (FAILED(hr)) {
    return hr;
  }
  WORD flags = DISPATCH_PROPERTYGET | DISPATCH_METHOD;
  DISPPARAMS params;
  memset(&params, 0, sizeof(params));
  params.cArgs = 3;
  VARIANTARG args[3];
  params.rgvarg = args;
  params.rgvarg[0].vt = VT_BSTR;
  params.rgvarg[0].bstrVal = x;
  params.rgvarg[1].vt = VT_BSTR;
  params.rgvarg[1].bstrVal = y;
  params.rgvarg[2].vt = VT_BSTR;
  params.rgvarg[2].bstrVal = z;
  CComVariant result;
  for (LONG i = 0; i < count; ++i) {
    CComVariant r;
    hr = callback->invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, flags,
      &params, &r, NULL, NULL);
    if (FAILED(hr)) {
      return hr;
    }
    result = r;
  }
  USHORT fl = VARIANT_ALPHABOOL;
  VARIANT vr;
  VariantInit(&vr);
  hr = VariantChangeTypeEx(&vr, &result, LOCALE_SYSTEM_DEFAULT, fl, VT_BSTR);
  if (SUCCEEDED(hr)) {
    (*r) = vr.bstrVal;
  }
  return hr;
}
