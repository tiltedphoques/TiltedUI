
#include <SpriteBatch.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
#include <CommonStates.h>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include <OverlayClient.hpp>
#include <OverlayRenderHandlerD3D11.hpp>

#include <algorithm>
#include <cstring>

namespace
{
    // CEF paints BGRA, 4 bytes per pixel
    constexpr uint32_t cBytesPerPixel = 4;
    // The cursor image is drawn at its own size on a 1080 pixels tall screen
    constexpr float cCursorReferenceHeight = 1080.f;
}

namespace TiltedPhoques
{
    OverlayRenderHandlerD3D11::OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept
        : m_pRenderer(apRenderer)
    {
        // So we need to lock this until we have the window dimension as a background CEF thread will attempt to get it before we have it
        m_createLock.lock();
        m_isCreateLockLocked = true;
    }

    OverlayRenderHandlerD3D11::~OverlayRenderHandlerD3D11() = default;

    void OverlayRenderHandlerD3D11::Render()
    {
        // We need a context first
        if (!m_pImmediateContext || !m_pSpriteBatch)
        {
            Create();

            if (!m_pImmediateContext || !m_pSpriteBatch)
                return;
        }

        GetRenderTargetSize();

        if (!m_pTexture)
            CreateRenderTexture();

        UploadDirtyPixels();

        if (IsVisible())
        {
            m_pSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pStates->NonPremultiplied());

            if (m_pTextureView && m_hasTextureContent)
                m_pSpriteBatch->Draw(m_pTextureView.Get(), DirectX::SimpleMath::Vector2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f);

            if (IsCursorVisible() && m_pCursorTexture)
            {
                // Scaled with the height so the cursor keeps its size on ultrawide screens
                const float cursorScale = static_cast<float>(m_height) / cCursorReferenceHeight;
                m_pSpriteBatch->Draw(m_pCursorTexture.Get(), DirectX::SimpleMath::Vector2(m_cursorX, m_cursorY), nullptr, DirectX::Colors::White, 0.f, DirectX::SimpleMath::Vector2(0, 0), cursorScale);
            }

            m_pSpriteBatch->End();
        }
    }

    void OverlayRenderHandlerD3D11::Reset()
    {
        m_pTexture.Reset();
        m_pCursorTexture.Reset();
        m_pTextureView.Reset();
        m_pDevice.Reset();
        m_pImmediateContext.Reset();
        m_textureWidth = 0;
        m_textureHeight = 0;
        m_hasTextureContent = false;

        m_pSpriteBatch = nullptr;
        m_pStates = nullptr;

        Create();
    }

    void OverlayRenderHandlerD3D11::Create()
    {
        m_pDevice = m_pRenderer->GetDevice();
        m_pImmediateContext = m_pRenderer->GetDeviceContext();

        if (!m_pImmediateContext || !m_pDevice)
            return;

        GetRenderTargetSize();

        m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(m_pImmediateContext.Get());
        m_pStates = std::make_unique<DirectX::CommonStates>(m_pDevice.Get());

        if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathPNG().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf())))
        {
            DirectX::CreateDDSTextureFromFile(m_pDevice.Get(), m_pParent->GetCursorPathDDS().c_str(), nullptr, m_pCursorTexture.ReleaseAndGetAddressOf());
        }

        if (!m_pTexture)
            CreateRenderTexture();
    }

    void OverlayRenderHandlerD3D11::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
    {
        std::scoped_lock _(m_createLock);

        rect = CefRect(0, 0, static_cast<int>(m_width.load()), static_cast<int>(m_height.load()));
    }

    void OverlayRenderHandlerD3D11::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
        const RectList& dirtyRects, const void* buffer, int width, int height)
    {
        // A paint of the previous size is dropped, CEF paints the whole view again after WasResized()
        if (type != PET_VIEW || width <= 0 || height <= 0 || static_cast<uint32_t>(width) != m_width || static_cast<uint32_t>(height) != m_height)
            return;

        const auto* pSource = static_cast<const uint8_t*>(buffer);
        const size_t rowSize = static_cast<size_t>(width) * cBytesPerPixel;

        std::scoped_lock _(m_pixelLock);

        if (m_pixelWidth != static_cast<uint32_t>(width) || m_pixelHeight != static_cast<uint32_t>(height))
        {
            m_pixels.resize(rowSize * static_cast<size_t>(height));
            m_pixelWidth = static_cast<uint32_t>(width);
            m_pixelHeight = static_cast<uint32_t>(height);

            std::memcpy(m_pixels.data(), pSource, m_pixels.size());
            AddDirtyRect(0, 0, m_pixelWidth, m_pixelHeight);
            return;
        }

        for (const CefRect& dirtyRect : dirtyRects)
        {
            const int left = std::clamp(dirtyRect.x, 0, width);
            const int top = std::clamp(dirtyRect.y, 0, height);
            const int right = std::clamp(dirtyRect.x + dirtyRect.width, left, width);
            const int bottom = std::clamp(dirtyRect.y + dirtyRect.height, top, height);

            if (left == right || top == bottom)
                continue;

            const size_t columnOffset = static_cast<size_t>(left) * cBytesPerPixel;
            const size_t copySize = static_cast<size_t>(right - left) * cBytesPerPixel;

            for (int row = top; row < bottom; ++row)
            {
                const size_t rowStart = static_cast<size_t>(row) * rowSize + columnOffset;
                std::memcpy(m_pixels.data() + rowStart, pSource + rowStart, copySize);
            }

            AddDirtyRect(static_cast<uint32_t>(left), static_cast<uint32_t>(top), static_cast<uint32_t>(right), static_cast<uint32_t>(bottom));
        }
    }

    bool OverlayRenderHandlerD3D11::GetScreenSize(uint32_t& aWidth, uint32_t& aHeight) const noexcept
    {
        // The back buffer has the size of the window's client area, which mouse coordinates are relative to.
        // The render target bound when the UI is drawn is not always the back buffer: while the game loads it
        // can be a 512x512 texture, and sizing the view after it made CEF lay out and repaint the whole UI twice.
        IDXGISwapChain* pSwapChain = m_pRenderer->GetSwapChain();
        DXGI_SWAP_CHAIN_DESC swapChainDesc{};

        if (pSwapChain && SUCCEEDED(pSwapChain->GetDesc(&swapChainDesc)) && swapChainDesc.BufferDesc.Width > 0 && swapChainDesc.BufferDesc.Height > 0)
        {
            aWidth = swapChainDesc.BufferDesc.Width;
            aHeight = swapChainDesc.BufferDesc.Height;
            return true;
        }

        // Without a swap chain, fall back to the bound render target
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRenderTargetView;

        m_pImmediateContext->OMGetRenderTargets(1, pRenderTargetView.ReleaseAndGetAddressOf(), nullptr);
        if (!pRenderTargetView)
            return false;

        Microsoft::WRL::ComPtr<ID3D11Resource> pSrcResource;
        pRenderTargetView->GetResource(pSrcResource.ReleaseAndGetAddressOf());

        Microsoft::WRL::ComPtr<ID3D11Texture2D> pSrcBuffer;
        if (!pSrcResource || FAILED(pSrcResource.As(&pSrcBuffer)))
            return false;

        D3D11_TEXTURE2D_DESC desc;
        pSrcBuffer->GetDesc(&desc);

        aWidth = desc.Width;
        aHeight = desc.Height;
        return aWidth > 0 && aHeight > 0;
    }

    void OverlayRenderHandlerD3D11::GetRenderTargetSize()
    {
        uint32_t width = 0;
        uint32_t height = 0;

        if (!GetScreenSize(width, height))
            return;

        if ((m_width == width && m_height == height) || !m_pParent)
            return;

        m_width = width;
        m_height = height;

        // We now know the size of the viewport, we can let CEF get it
        if (m_isCreateLockLocked)
        {
            m_createLock.unlock();
            m_isCreateLockLocked = false;
        }

        m_pTexture.Reset();
        m_pTextureView.Reset();
        m_hasTextureContent = false;

        if (m_pParent->GetBrowser())
            m_pParent->GetBrowser()->GetHost()->WasResized();
    }

    void OverlayRenderHandlerD3D11::CreateRenderTexture()
    {
        const uint32_t width = m_width;
        const uint32_t height = m_height;

        if (!m_pDevice || width == 0 || height == 0)
            return;

        // Default usage: the texture is only written with UpdateSubresource(), which keeps the pixels
        // outside the updated box, unlike mapping a dynamic texture with D3D11_MAP_WRITE_DISCARD
        D3D11_TEXTURE2D_DESC textDesc;
        textDesc.Width = width;
        textDesc.Height = height;
        textDesc.MipLevels = textDesc.ArraySize = 1;
        textDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        textDesc.SampleDesc.Count = 1;
        textDesc.SampleDesc.Quality = 0;
        textDesc.Usage = D3D11_USAGE_DEFAULT;
        textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        textDesc.CPUAccessFlags = 0;
        textDesc.MiscFlags = 0;

        if (FAILED(m_pDevice->CreateTexture2D(&textDesc, nullptr, m_pTexture.ReleaseAndGetAddressOf())))
            return;

        D3D11_SHADER_RESOURCE_VIEW_DESC sharedResourceViewDesc = {};
        sharedResourceViewDesc.Format = textDesc.Format;
        sharedResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        sharedResourceViewDesc.Texture2D.MipLevels = 1;

        if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture.Get(), &sharedResourceViewDesc, m_pTextureView.ReleaseAndGetAddressOf())))
        {
            m_pTexture.Reset();
            return;
        }

        m_textureWidth = width;
        m_textureHeight = height;
        m_hasTextureContent = false;

        // The new texture is empty, so the whole view goes up on the next upload
        std::scoped_lock _(m_pixelLock);

        if (m_pixelWidth == width && m_pixelHeight == height)
            AddDirtyRect(0, 0, width, height);
    }

    void OverlayRenderHandlerD3D11::UploadDirtyPixels() noexcept
    {
        if (!m_pTexture)
            return;

        std::scoped_lock _(m_pixelLock);

        // While the view is being resized, keep the dirty area until CEF paints at the texture's size
        if (!m_hasDirtyPixels || m_pixelWidth != m_textureWidth || m_pixelHeight != m_textureHeight)
            return;

        const uint32_t rowSize = m_pixelWidth * cBytesPerPixel;
        const uint8_t* pSource = m_pixels.data() + static_cast<size_t>(m_dirtyTop) * rowSize + static_cast<size_t>(m_dirtyLeft) * cBytesPerPixel;
        const D3D11_BOX box{ m_dirtyLeft, m_dirtyTop, 0, m_dirtyRight, m_dirtyBottom, 1 };

        m_pImmediateContext->UpdateSubresource(m_pTexture.Get(), 0, &box, pSource, rowSize, 0);

        m_hasDirtyPixels = false;
        m_hasTextureContent = true;
    }

    void OverlayRenderHandlerD3D11::AddDirtyRect(uint32_t aLeft, uint32_t aTop, uint32_t aRight, uint32_t aBottom) noexcept
    {
        if (!m_hasDirtyPixels)
        {
            m_dirtyLeft = aLeft;
            m_dirtyTop = aTop;
            m_dirtyRight = aRight;
            m_dirtyBottom = aBottom;
            m_hasDirtyPixels = true;
            return;
        }

        m_dirtyLeft = std::min(m_dirtyLeft, aLeft);
        m_dirtyTop = std::min(m_dirtyTop, aTop);
        m_dirtyRight = std::max(m_dirtyRight, aRight);
        m_dirtyBottom = std::max(m_dirtyBottom, aBottom);
    }
}
