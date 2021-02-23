
#include "UIView.hpp"
#include "UIApp.hpp"
#include <include/base/cef_bind.h>

namespace TiltedPhoques
{
    UIView::UIView()
    {
        GetUiApplication()->RegisterView(this);
    }

    class CefClosureTask : public CefTask {
    public:
        explicit CefClosureTask(CefRefPtr<CefBrowser>& browser) : m_browserRef(browser) {}

        // CefTask method
        virtual void Execute() OVERRIDE {

            m_browserRef->GetHost()->CloseBrowser(true);
        }

    private:
        CefRefPtr<CefBrowser> m_browserRef;

        IMPLEMENT_REFCOUNTING(CefClosureTask);
        DISALLOW_COPY_AND_ASSIGN(CefClosureTask);
    };


    static void CloseBrowser(CefRefPtr<CefBrowser> browser)
    {
        browser->GetHost()->CloseBrowser(true);
    }

    UIView::~UIView()
    {
        auto* appInstance = GetUiApplication();
        if (!appInstance)
            __debugbreak();

        m_viewLock.lock();

#if 0
        if (m_pClient)
        {
            bool done = false;
            while (!done) {
                done = m_pClient->GetBrowser()->GetHost()->
                    TryCloseBrowser();
            }
        }
#endif

        if (m_pClient->GetBrowser() && m_pClient->GetBrowser()->GetHost())
        {
            if (!CefCurrentlyOn(TID_UI))
            {
                CefPostTask(TID_UI, new CefClosureTask(m_pClient->GetBrowser()));
            }
            else
            {
                m_pClient->GetBrowser()->GetHost()->CloseBrowser(true);
            }
        }

        m_viewLock.unlock();
        appInstance->UnregisterView(this);
    }

    bool UIView::OpenUrl(const std::string_view url, HWND aHwnd)
    {
        CefBrowserSettings settings{};
        settings.javascript_close_windows = cef_state_t::STATE_DISABLED;
        settings.javascript_access_clipboard = cef_state_t::STATE_DISABLED;
        settings.plugins = cef_state_t::STATE_DISABLED;
        settings.webgl = cef_state_t::STATE_ENABLED;
        settings.file_access_from_file_urls = cef_state_t::STATE_ENABLED;
        settings.universal_access_from_file_urls = cef_state_t::STATE_ENABLED;
        settings.web_security = cef_state_t::STATE_DISABLED;
        settings.windowless_frame_rate = 240;
        m_pClient = new UIClient(this);

        CefWindowInfo info;
        info.SetAsWindowless(aHwnd);

        auto ctx = CefRequestContext::GetGlobalContext();
        return CefBrowserHost::CreateBrowser(info, m_pClient, CefString(url.data()), settings, nullptr, ctx);
    }

    void UIView::SetVisible(bool aToggle)
    {
        m_bVisible = aToggle;
    }

    bool UIView::IsVisible() const
    {
        return m_bVisible;
    }

    void UIView::ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept
    {
        if (!m_pClient)
            return;

        auto pMessage = CefProcessMessage::Create("browser-event");
        auto pArguments = pMessage->GetArgumentList();

        const auto pFunctionArguments = apArguments ? apArguments : CefListValue::Create();

        pArguments->SetString(0, acFunction);
        pArguments->SetList(1, pFunctionArguments);

        if (auto pBrowser = m_pClient->GetBrowser())
        {
            pBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, pMessage);
        }
    }

    CefString& UIView::GetName() noexcept
    {
        return m_name;
    }
}
