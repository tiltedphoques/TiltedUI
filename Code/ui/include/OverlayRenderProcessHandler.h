#pragma once

#include <algorithm>

#include <include/cef_render_process_handler.h>

#include <OverlayV8Handler.h>
#include <EventsV8Handler.h>

class OverlayRenderProcessHandler : public CefRenderProcessHandler
{
public:

    OverlayRenderProcessHandler(std::string aCoreObjectName = "core");
    ~OverlayRenderProcessHandler() = default;

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId sourceProcess,
        CefRefPtr<CefProcessMessage> message) override;

    void CreateEvent(const std::string& acEventName);
    void CreateFunction(const std::string& acFunctionName);

    IMPLEMENT_REFCOUNTING(OverlayRenderProcessHandler);

private:

    CefRefPtr<CefV8Value> ConvertValue(CefRefPtr<CefValue> apValue);

    CefRefPtr<EventsV8Handler> m_pEventsHandler;

    CefRefPtr<OverlayV8Handler> m_pOverlayHandler;

    CefRefPtr<CefV8Value> m_pCoreObject;

    std::string m_coreObjectName;
};
