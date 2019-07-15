#pragma once

#include <include/cef_app.h>
#include <OverlayRenderProcessHandler.h>
#include <functional>

class OverlayApp : public CefApp
{
public:

    OverlayApp(std::function<OverlayRenderProcessHandler* ()> aFactory);
    ~OverlayApp() = default;

    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

    IMPLEMENT_REFCOUNTING(OverlayApp);

private:

    CefRefPtr<OverlayRenderProcessHandler> m_pRenderProcess;
};

int TiltedUIMain(const char* acpArgs, HINSTANCE aInstance, std::function<OverlayRenderProcessHandler* ()> aFactory);