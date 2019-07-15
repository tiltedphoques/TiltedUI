#pragma once

#include <include/cef_v8.h>

class OverlayV8Handler : public CefV8Handler
{
public:

    OverlayV8Handler(CefRefPtr<CefBrowser> apBrowser);
    ~OverlayV8Handler() = default;

    bool Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException) override;

    IMPLEMENT_REFCOUNTING(OverlayV8Handler);

private:

    void Dispatch(const CefString& acName, const CefV8ValueList& acArguments);

    CefRefPtr<CefValue> ConvertValue(const CefRefPtr<CefV8Value> acpValue);

    CefRefPtr<CefBrowser> m_pBrowser;
};