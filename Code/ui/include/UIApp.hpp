#pragma once

#include <mutex>
#include <TiltedCore/Meta.hpp>
#include <include/cef_app.h>

#include "UIClient.hpp"
#include "UIView.hpp"

namespace TiltedPhoques
{
    struct OverlayCreateInfo {
        std::wstring processName = L"TPProcess.exe";
        bool highDPI = false;
    };

    class UIApp final :
        public CefApp,
        public CefBrowserProcessHandler,
        public CefRenderProcessHandler
    {
        friend class UIView;
    public:
        enum class DrawCommand
        {
            kRender,
            kCreate,
            kReset
        };

        UIApp() noexcept;
        ~UIApp();

        bool Initialize(const OverlayCreateInfo& aInfo) noexcept;

        // this is best not to be called manually since the dtor can ensure
        // proper shutdown order
        void Shutdown() noexcept;

        static void PumpMessageLoop() noexcept;

        void DoDrawCommand(DrawCommand aCommand) noexcept;

        UIView* FindView(const CefString& aName) noexcept;

        TP_NOCOPYMOVE(UIApp);
        IMPLEMENT_REFCOUNTING(UIApp);
    private:
        // a frame needs to be registered to be updated
        void RegisterView(UIView *apView) noexcept;
        void UnregisterView(UIView *apView) noexcept;
    private:
        // impl CefApp
        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
        // impl CefApp
        void OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine) override;
        void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
        // impl CefBrowserProcessHandler
        void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> aCommandLine) override;
        // impl CefRenderProcessHandler
        void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;
        void OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) override;

        bool m_bReady = false;
        std::mutex m_FrameBucketLock;
        std::vector<UIView*> m_Frames;
    };

    UIApp* GetUiApplication();
}
