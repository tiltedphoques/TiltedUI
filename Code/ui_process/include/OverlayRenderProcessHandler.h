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

    void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override final;

    void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override final;

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefProcessId sourceProcess,
        CefRefPtr<CefProcessMessage> message) override final;

    void CreateEvent(const std::string& acEventName) const;
    void CreateFunction(const std::string& acFunctionName) const;

    virtual void OnInitialize() = 0;

    IMPLEMENT_REFCOUNTING(OverlayRenderProcessHandler);

private:

    static CefRefPtr<CefV8Value> ConvertValue(const CefRefPtr<CefValue>& apValue);

    CefRefPtr<EventsV8Handler> m_pEventsHandler;
    CefRefPtr<OverlayV8Handler> m_pOverlayHandler;
    CefRefPtr<CefV8Value> m_pCoreObject;
    std::string m_coreObjectName;
};
