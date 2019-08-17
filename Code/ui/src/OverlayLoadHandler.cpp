#include <OverlayLoadHandler.h>

void OverlayLoadHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    
}

void OverlayLoadHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
}

bool OverlayLoadHandler::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefLoadHandler::ErrorCode errorCode, const CefString& failedUrl, CefString& errorText)
{
}
