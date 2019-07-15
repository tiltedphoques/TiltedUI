#include <OverlayApp.h>

OverlayApp::OverlayApp()
    : m_pRenderProcess(new OverlayRenderProcessHandler())
{
}

CefRefPtr<CefRenderProcessHandler> OverlayApp::GetRenderProcessHandler()
{
    return m_pRenderProcess;
}
