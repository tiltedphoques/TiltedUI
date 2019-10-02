#pragma once

#include <include/cef_load_handler.h>
#include <Meta.hpp>

namespace TiltedPhoques
{
    struct OverlayLoadHandler final : CefLoadHandler
    {
        OverlayLoadHandler() = default;
        virtual ~OverlayLoadHandler() = default;

        TP_NOCOPYMOVE(OverlayLoadHandler);

        void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
        void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
        bool OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& failedUrl, CefString& errorText);

        [[nodiscard]] bool IsReady() const noexcept { return m_ready; }

        IMPLEMENT_REFCOUNTING(OverlayLoadHandler);

    private:

        bool m_ready{ false };
    };
}
