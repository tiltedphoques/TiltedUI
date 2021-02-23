#pragma once

#include <include/cef_scheme.h>

namespace TiltedPhoques
{
    class UISchemeFactory : public CefSchemeHandlerFactory
    {
    public:
        CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request);

        IMPLEMENT_REFCOUNTING(UISchemeFactory);
    };
}
