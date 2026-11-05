#pragma once

#include <wrl.h>
#include <atomic>
#include <mutex>
#include <vector>

#include <OverlayRenderHandler.hpp>
#include <TiltedCore/Signal.hpp>

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
    // CEF paints go into a CPU copy of the view; the render thread uploads only the repainted area, once per frame
    struct OverlayRenderHandlerD3D11 : OverlayRenderHandler
    {
        struct Renderer
        {
            Renderer() = default;
            virtual ~Renderer() = default;
            [[nodiscard]] virtual IDXGISwapChain* GetSwapChain() const noexcept = 0;
            [[nodiscard]] virtual ID3D11Device* GetDevice() const noexcept = 0;
            [[nodiscard]] virtual ID3D11DeviceContext* GetDeviceContext() const noexcept = 0;

            TP_NOCOPYMOVE(Renderer);
        };

        explicit OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept;
        virtual ~OverlayRenderHandlerD3D11();

        TP_NOCOPYMOVE(OverlayRenderHandlerD3D11);

        void Create() override;
        void Render() override;
        void Reset() override;

        void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
        void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

        IMPLEMENT_REFCOUNTING(OverlayRenderHandlerD3D11);

    protected:

        void UpdateViewSize();
        void CreateRenderTexture();

    private:

        // Size of the swap chain's back buffer, or of the bound render target when there is no swap chain
        [[nodiscard]] bool GetScreenSize(uint32_t& aWidth, uint32_t& aHeight) const noexcept;
        void UploadDirtyPixels() noexcept;
        // Call with m_pixelLock held
        void AddDirtyRect(uint32_t aLeft, uint32_t aTop, uint32_t aRight, uint32_t aBottom) noexcept;

        // Size of the CEF view, read by CEF's threads
        std::atomic<uint32_t> m_width{ 0 };
        std::atomic<uint32_t> m_height{ 0 };

        std::mutex m_createLock;
        bool m_isCreateLockLocked = false;
        Renderer* m_pRenderer;

        // CPU copy of the view, written by OnPaint on CEF's thread and uploaded by Render on the render thread
        std::mutex m_pixelLock;
        std::vector<uint8_t> m_pixels;
        uint32_t m_pixelWidth{ 0 };
        uint32_t m_pixelHeight{ 0 };
        bool m_hasDirtyPixels = false;
        uint32_t m_dirtyLeft{ 0 };
        uint32_t m_dirtyTop{ 0 };
        uint32_t m_dirtyRight{ 0 };
        uint32_t m_dirtyBottom{ 0 };

        // Render thread only
        Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pCursorTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTextureView;
        uint32_t m_textureWidth{ 0 };
        uint32_t m_textureHeight{ 0 };
        bool m_hasTextureContent = false;

        Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice;
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pImmediateContext;

        std::unique_ptr<DirectX::SpriteBatch> m_pSpriteBatch;
        std::unique_ptr<DirectX::CommonStates> m_pStates;
    };
}
