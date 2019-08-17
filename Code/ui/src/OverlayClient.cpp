#include <OverlayClient.h>

#include <OverlayContextHandler.h>

OverlayClient::OverlayClient(OverlayRenderHandler* apHandler) noexcept
    : m_pRenderHandler(apHandler)
    , m_pLoadHandler(new OverlayLoadHandler)
    , m_pBrowser(nullptr)
    , m_pContextMenuHandler(new OverlayContextHandler)
{
}

CefRefPtr<CefRenderHandler> OverlayClient::GetRenderHandler()
{
    return m_pRenderHandler;
}

CefRefPtr<CefLoadHandler> OverlayClient::GetLoadHandler()
{
    return m_pLoadHandler;
}

CefRefPtr<CefLifeSpanHandler> OverlayClient::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefContextMenuHandler> OverlayClient::GetContextMenuHandler()
{
    return m_pContextMenuHandler;
}

void OverlayClient::SetBrowser(const CefRefPtr<CefBrowser>& aBrowser) noexcept
{
    m_pBrowser = aBrowser;
}

CefRefPtr<CefBrowser> OverlayClient::GetBrowser() const noexcept
{
    return m_pBrowser;
}

void OverlayClient::Create() const noexcept
{
    if (m_pRenderHandler)
        m_pRenderHandler->Create();
}

void OverlayClient::Render() const noexcept
{
    if (m_pRenderHandler)
        m_pRenderHandler->Render();
}

void OverlayClient::Reset() const noexcept
{
    if (m_pRenderHandler)
        m_pRenderHandler->Reset();
}

void OverlayClient::OnAfterCreated(CefRefPtr<CefBrowser> aBrowser)
{
    SetBrowser(aBrowser);
}

void OverlayClient::OnBeforeClose(CefRefPtr<CefBrowser> aBrowser)
{
    SetBrowser(nullptr);
}

bool OverlayClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
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

bool OverlayClient::IsReady() const
{
    return m_pBrowser && m_pLoadHandler->IsReady();
}
