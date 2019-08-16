#pragma once

#include <OverlayRenderHandler.h>
#include <Meta.h>
#include <SpriteBatch.h>
#include <wrl.h>
#include <mutex>

struct OverlayRenderHandlerD3D11 : OverlayRenderHandler
{
    OverlayRenderHandlerD3D11(IDXGISwapChain* apSwapChain) noexcept;
    virtual ~OverlayRenderHandlerD3D11();

    TP_NOCOPYMOVE(OverlayRenderHandlerD3D11);

    // will be cast to DirectX::SpriteBatch
    void Render(void* apSprite) override;

    void CreateResources() override;

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

    IMPLEMENT_REFCOUNTING(OverlayRenderHandlerD3D11);

private:
    int m_width{0};
    int m_height{0};
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
    std::recursive_mutex m_textureLock;
    IDXGISwapChain* m_pSwapChain;
};
