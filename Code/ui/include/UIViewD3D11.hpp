#pragma once

#include <wrl.h>
#include <mutex>

#include <TiltedCore/Signal.hpp>
#include <cef_render_handler.h>
#include "UIView.hpp"

namespace DirectX
{
    class SpriteBatch;
    class CommonStates;
}

struct IDXGISwapChain;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11DeviceContext;
struct ID3D11Device;

namespace TiltedPhoques
{
    class UIViewD3D11 final : public UIView
    {
    public:
        struct Renderer
        {
            Renderer() = default;
            virtual ~Renderer() = default;
            [[nodiscard]] virtual IDXGISwapChain* GetSwapChain() const noexcept = 0;

            TP_NOCOPYMOVE(Renderer);
        };

        // regular constructor
        UIViewD3D11(Renderer* apRenderer) noexcept;
        UIViewD3D11(IDXGISwapChain* apSwapchain, ID3D11Texture2D* apCustomTexture = nullptr) noexcept;

        ~UIViewD3D11();

        void Create() override;
        void Render() override;
        void Reset() override;

        //TP_NOCOPYMOVE(UIViewD3D11);
        IMPLEMENT_REFCOUNTING(UIViewD3D11);
    protected:

        void GetRenderTargetSize();
        void CreateRenderTexture();
    private:
        // impl: CefRenderHandler
        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

        uint32_t m_width{ 1280 };
        uint32_t m_height{ 720 };

        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCursorTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
        std::mutex m_createLock;

        Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;
        std::unique_ptr<DirectX::CommonStates> m_pStates;
    };
}
