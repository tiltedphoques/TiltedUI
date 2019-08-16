#pragma once

#include <OverlayRenderHandler.h>
#include <Meta.h>
#include <SpriteBatch.h>
#include <wrl.h>
#include <mutex>
#include <Signal.h>

namespace DirectX
{
    struct SpriteBatch;
    struct CommonStates;
}

struct OverlayRenderHandlerD3D11 : OverlayRenderHandler
{
    struct Renderer
    {
        virtual ~Renderer() = default;
        [[nodiscard]] virtual IDXGISwapChain* GetSwapChain() const noexcept = 0;

        TP_NOCOPYMOVE(Renderer);

        Signal<void(IDXGISwapChain*)> OnLost;
        Signal<void(IDXGISwapChain*)> OnRender;
    };

    OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept;
    virtual ~OverlayRenderHandlerD3D11();

    TP_NOCOPYMOVE(OverlayRenderHandlerD3D11);

    void Render(IDXGISwapChain* apSwapChain);
    void Lost(IDXGISwapChain* apSwapChain);

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
    Renderer* m_pRenderer;

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;

    std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;
    std::unique_ptr<DirectX::CommonStates> m_pStates;
};
