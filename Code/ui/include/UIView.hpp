#pragma once

#include <mutex>
#include <include/cef_client.h>
#include <TiltedCore/Meta.hpp>

namespace TiltedPhoques
{
    class UIView :
        public CefRenderHandler,
        public CefClient,
        public CefLifeSpanHandler,
        public CefLoadHandler,
        public CefContextMenuHandler
    {
        friend class UIApp;
    public:
        UIView();
        ~UIView();

        bool OpenUrl(const std::string_view url, HWND aHwnd);

        void SetVisible(bool aToggle);
        bool IsVisible() const;

        [[nodiscard]] CefRefPtr<CefBrowser> GetBrowser() const noexcept;
        bool IsReady() const;


        void ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept;

        virtual void Create() = 0;
        virtual void Render() = 0;
        virtual void Reset() = 0;

        virtual void* GetTextureHandle() { return nullptr; }

        CefString& GetName() noexcept;

        void KillBrowser();
    private:
        // impl: CefClient
        CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
        CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
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

    protected:
        CefRefPtr<CefBrowser> m_pBrowser;

        bool m_bVisible = false;
        bool m_bBeingKilled = false;
        bool m_bIsMainView = false;

        CefString m_name;
        std::mutex m_viewLock;
    };
}
