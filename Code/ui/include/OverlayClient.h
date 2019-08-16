#pragma once

#include <include/cef_client.h>
#include <OverlayLoadHandler.h>
#include <OverlayRenderHandler.h>

#include <Meta.h>

struct OverlayClient : CefClient, CefLifeSpanHandler, CefContextMenuHandler
{
    explicit OverlayClient(OverlayRenderHandler* apHandler) noexcept;

    TP_NOCOPYMOVE(OverlayClient);

    CefRefPtr<CefRenderHandler> GetRenderHandler() override;
    CefRefPtr<CefLoadHandler> GetLoadHandler() override;
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;

    [[nodiscard]] CefRefPtr<CefBrowser> GetBrowser() const noexcept;

    void CreateResources() noexcept;

    void OnAfterCreated(CefRefPtr<CefBrowser> aBrowser) override;
    void OnBeforeClose(CefRefPtr<CefBrowser> aBrowser) override;
    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

    bool IsReady() const;

    IMPLEMENT_REFCOUNTING(OverlayClient);

private:

    void SetBrowser(const CefRefPtr<CefBrowser>& aBrowser) noexcept;

    CefRefPtr<OverlayRenderHandler> m_pRenderHandler;
    CefRefPtr<OverlayLoadHandler> m_pLoadHandler;
    CefRefPtr<CefBrowser> m_pBrowser;
    CefRefPtr<CefContextMenuHandler> m_pContextMenuHandler;

};
