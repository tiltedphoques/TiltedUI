
#include "UIApp.hpp"
#include "UIViewD3D11.hpp"
#include "UISchemeHandler.hpp"

#include <TiltedCore/Platform.hpp>
#include <TiltedCore/Filesystem.hpp>

namespace TiltedPhoques
{
    static UIApp* gAppInstance = nullptr;

    UIApp::UIApp() noexcept
    {
        gAppInstance = this;
    }

    UIApp::~UIApp()
    {
        Shutdown();
        gAppInstance = nullptr;
    }

    bool UIApp::Initialize(const OverlayCreateInfo &aCreateInfo) noexcept
    {

        if (aCreateInfo.highDPI)
        {
            CefEnableHighDPISupport();
        }

        CefMainArgs args(GetModuleHandleW(nullptr));

        const auto currentPath = TiltedPhoques::GetPath();

        CefSettings settings;
        settings.no_sandbox = true;
        settings.multi_threaded_message_loop = true;
        settings.windowless_rendering_enabled = true;
#ifdef DEBUG
        settings.log_severity = LOGSEVERITY_VERBOSE;
        settings.remote_debugging_port = 8384;
#else
        settings.log_severity = LOGSEVERITY_VERBOSE;
#endif

        CefString(&settings.locale).FromString("en-US");
        CefString(&settings.log_file).FromWString(currentPath / L"logs" / L"cef_debug.log");
        CefString(&settings.cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.framework_dir_path).FromWString(currentPath);
        CefString(&settings.root_cache_path).FromWString(currentPath / L"cache");
        CefString(&settings.resources_dir_path).FromWString(currentPath);
        CefString(&settings.locales_dir_path).FromWString(currentPath / L"locales");
        CefString(&settings.browser_subprocess_path).FromWString(currentPath / aCreateInfo.processName);

        m_bReady = CefInitialize(args, settings, this, nullptr);
        // support the "tilted phoques" scheme for local resources
        //CefRegisterSchemeHandlerFactory("tp", "", new UISchemeFactory());


        return m_bReady;
    }

    void UIApp::Shutdown() noexcept
    {
        if (m_bReady) {
            //CefClearSchemeHandlerFactories();
            CefShutdown();

            m_bReady = false;
        }
    }

    void UIApp::RegisterView(UIView *apFrame) noexcept
    {
        std::lock_guard lock(m_FrameBucketLock);
        TP_UNUSED(lock);

        m_Frames.push_back(apFrame);
    }

    void UIApp::UnregisterView(UIView* apFrame) noexcept
    {
        std::lock_guard lock(m_FrameBucketLock);
        TP_UNUSED(lock);

        // isn't modern c++ a joy to read :D
        auto it = std::find_if(m_Frames.begin(), m_Frames.end(), [&](UIView* apItem) {
            return apFrame == apItem;
        });

        if (it != m_Frames.end())
            m_Frames.erase(it);
    }

    UIView* UIApp::FindView(const CefString &aName) noexcept
    {
        std::lock_guard lock(m_FrameBucketLock);
        TP_UNUSED(lock);

        auto it = std::find_if(m_Frames.begin(), m_Frames.end(), [&](UIView* apItem) {
            return aName == apItem->GetName();
        });

        if (it != m_Frames.end())
            return *it;

        return nullptr;
    }

    void UIApp::DoDrawCommand(DrawCommand aCommand) noexcept
    {
        for (const auto& frame : m_Frames)
        {
            switch (aCommand)
            {
            case DrawCommand::kRender:
                frame->Render();
                break;
            case DrawCommand::kCreate:
                frame->Create();
                break;
            case DrawCommand::kReset:
                frame->Reset();
                break;
            default:
                break;
            }
        }
    }

    void UIApp::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
    {
        registrar->AddCustomScheme("tp",
            CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_SECURE | CEF_SCHEME_OPTION_CORS_ENABLED | CEF_SCHEME_OPTION_FETCH_ENABLED);
    }

    void UIApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> aCommandLine)
    {
        aCommandLine->AppendSwitchWithValue("pid", std::to_string(GetCurrentProcessId()));
    }

    void UIApp::OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine)
    {
        aCommandLine->AppendSwitchWithValue("default-encoding", "utf-8");
        aCommandLine->AppendSwitchWithValue("autoplay-policy", "no-user-gesture-required");
    }

    void UIApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {
        //CefRefPtr<CefV8Value> window = context->GetGlobal();
    }

    void UIApp::OnContextReleased(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
    {

    }

    UIApp* GetUiApplication()
    {
        return gAppInstance;
    }
}
