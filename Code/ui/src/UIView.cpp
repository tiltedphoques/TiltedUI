
#include "UIView.hpp"
#include "UIApp.hpp"

#include <include/base/cef_bind.h>
#include <include/wrapper/cef_closure_task.h>

namespace TiltedPhoques
{
    static void DoCloseBrowser(CefRefPtr<CefBrowser> browser)
    {
        browser->GetHost()->CloseBrowser(true);
    }

    UIView::UIView()
    {
        GetUiApplication()->RegisterView(this);
    }

    UIView::~UIView()
    {
        auto* appInstance = GetUiApplication();
        if (!appInstance)
            __debugbreak();

        m_viewLock.lock();
        m_bShouldWork = false;

        if (m_pBrowser && m_pBrowser->GetHost())
        {
            if (!CefCurrentlyOn(TID_UI))
            {
                CefPostTask(TID_UI, base::Bind(&DoCloseBrowser, GetBrowser()));
            }
            else
            {
                m_pBrowser->GetHost()->CloseBrowser(true);
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

        CefWindowInfo info;
        info.SetAsWindowless(aHwnd);

        auto ctx = CefRequestContext::GetGlobalContext();
        bool result =  CefBrowserHost::CreateBrowser(info, this, CefString(url.data()), settings, nullptr, ctx);

        SetVisible(true);

        return result;
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
        auto pMessage = CefProcessMessage::Create("browser-event");
        auto pArguments = pMessage->GetArgumentList();

        const auto pFunctionArguments = apArguments ? apArguments : CefListValue::Create();

        pArguments->SetString(0, acFunction);
        pArguments->SetList(1, pFunctionArguments);

        if (auto pBrowser = GetBrowser())
        {
            pBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, pMessage);
        }
    }

    CefRefPtr<CefBrowser> UIView::GetBrowser() const noexcept
    {
        return m_pBrowser;
    }

    void UIView::OnAfterCreated(CefRefPtr<CefBrowser> aBrowser)
    {
        m_pBrowser = aBrowser;
    }

    void UIView::OnBeforeClose(CefRefPtr<CefBrowser> aBrowser)
    {
        m_pBrowser = {};
    }

    bool UIView::DoClose(CefRefPtr<CefBrowser> browser)
    {
        return false;
    }

    void UIView::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType)
    {
#if 0
        if (frame->IsMain() && transitionType == TT_EXPLICIT)
        {
            m_ready = false;
        }
#endif
    }

    void UIView::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
    {
#if 0
        if (frame->IsMain())
        {
            m_ready = true;
        }
#endif
    }

    void UIView::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
    {
        if (errorCode == ErrorCode::ERR_ABORTED)
            return;

        // TODO: load frame text
    }

    void UIView::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
    {
        model->Clear();
    }

    bool UIView::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
    {
        if (message->GetName() == "ui-event")
        {
            auto pArguments = message->GetArgumentList();

            auto eventName = pArguments->GetString(0).ToString();
            auto eventArgs = pArguments->GetList(1);

            return true;
        }

        return false;
    }

    bool UIView::IsReady() const
    {
        return m_pBrowser; //&& m_ready;
    }

    CefString& UIView::GetName() noexcept
    {
        return m_name;
    }
}
