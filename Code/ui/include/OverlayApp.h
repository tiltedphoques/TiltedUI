#pragma once

#include <include/cef_app.h>
#include <OverlayRenderProcessHandler.h>

class OverlayApp : public CefApp
{
public:

    OverlayApp();
    ~OverlayApp() = default;

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    IMPLEMENT_REFCOUNTING(OverlayApp);

private:

    CefRefPtr<OverlayRenderProcessHandler> m_pRenderProcess;
};
