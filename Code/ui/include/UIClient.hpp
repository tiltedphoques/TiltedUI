#pragma once

#include <include/cef_client.h>
#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    class UIView;

    class UIClient final :
        public CefClient,
        public CefLifeSpanHandler,
        public CefLoadHandler,
        public CefContextMenuHandler
    {
    public:
        explicit UIClient(UIView* apView) noexcept;

        [[nodiscard]] CefRefPtr<CefBrowser> GetBrowser() const noexcept;
        [[nodiscard]] const std::wstring& GetCursorPathPNG() const noexcept;
        [[nodiscard]] const std::wstring& GetCursorPathDDS() const noexcept;

        bool IsReady() const;

        //TP_NOCOPYMOVE(UIClient);
        IMPLEMENT_REFCOUNTING(UIClient);
    private:
        // impl: CefClient
        CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
        CefRefPtr<CefRenderHandler> GetRenderHandler() override;
        CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
        // impl: CefLifeSpanHandler
        void OnAfterCreated(CefRefPtr<CefBrowser> aBrowser) override;
        void OnBeforeClose(CefRefPtr<CefBrowser> aBrowser) override;
        // impl: CefContextMenuHandler
        void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;
        // impl: CefClient
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
        // impl: CefLoadHandler
        void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType) override;
        void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
        void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl) override;

        CefRefPtr<CefBrowser> m_pBrowser;
        UIView* m_pRenderer;

        std::wstring m_cursorPathPNG;
        std::wstring m_cursorPathDDS;
        bool m_ready = false;
    };
}
