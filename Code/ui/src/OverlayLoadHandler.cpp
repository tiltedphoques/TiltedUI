#include <OverlayLoadHandler.hpp>

namespace TiltedPhoques
{
    void OverlayLoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
    {
        if (frame->IsMain() && transition_type == TT_EXPLICIT)
        {
            m_ready = false;
        }
    }

    void OverlayLoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
    {
        if (frame->IsMain())
        {
            m_ready = true;
        }
    }

    bool OverlayLoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefLoadHandler::ErrorCode errorCode, const CefString& failedUrl, CefString& errorText)
    {
        return false;
    }
}
