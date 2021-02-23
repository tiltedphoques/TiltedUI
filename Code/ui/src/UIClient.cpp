
#include "UIClient.hpp"
#include "UIView.hpp"
#include <TiltedCore/Filesystem.hpp>

namespace TiltedPhoques
{
    UIClient::UIClient(UIView* apFrame) noexcept
        : m_pRenderer(apFrame)
        , m_pBrowser(nullptr)
    {
        const auto currentPath = TiltedPhoques::GetPath();

        m_cursorPathPNG = (currentPath / "assets" / "images" / "cursor.png").wstring();
        m_cursorPathDDS = (currentPath / "assets" / "images" / "cursor.dds").wstring();
    }

    CefRefPtr<CefRenderHandler> UIClient::GetRenderHandler()
    {
        return m_pRenderer;
    }

    CefRefPtr<CefBrowser> UIClient::GetBrowser() const noexcept
    {
        return m_pBrowser;
    }

    const std::wstring& UIClient::GetCursorPathPNG() const noexcept
    {
        return m_cursorPathPNG;
    }

    const std::wstring& UIClient::GetCursorPathDDS() const noexcept
    {
        return m_cursorPathDDS;
    }

    void UIClient::OnAfterCreated(CefRefPtr<CefBrowser> aBrowser)
    {
        m_pBrowser = aBrowser;
    }

    void UIClient::OnBeforeClose(CefRefPtr<CefBrowser> aBrowser)
    {
        m_pBrowser = nullptr;
    }

    void UIClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transitionType)
    {
        if (frame->IsMain() && transitionType == TT_EXPLICIT)
        {
            m_ready = false;
        }
    }

    void UIClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
    {
        if (frame->IsMain())
        {
            m_ready = true;
        }
    }

    void UIClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString& errorText, const CefString& failedUrl)
    {
        if (errorCode == ErrorCode::ERR_ABORTED)
            return;

        // TODO: load frame text
    }

    void UIClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
    {
        model->Clear();
    }

    bool UIClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
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

    bool UIClient::IsReady() const
    {
        return m_pBrowser && m_ready;
    }
}
