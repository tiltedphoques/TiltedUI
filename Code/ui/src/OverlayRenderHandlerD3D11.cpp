#include <OverlayRenderHandlerD3D11.h>

#include <DirectXColors.h>
#include <SimpleMath.h>

OverlayRenderHandlerD3D11::OverlayRenderHandlerD3D11(IDXGISwapChain* apSwapChain) noexcept
    : m_pSwapChain(apSwapChain)
{
    
}

OverlayRenderHandlerD3D11::~OverlayRenderHandlerD3D11()
{
    if (m_pTextureView)
    {
        m_pTextureView->Release();
    }

    if (m_pTexture)
    {
        m_pTexture->Release();
    }
}

void OverlayRenderHandlerD3D11::Render(void* apSprite)
{
    auto pSpriteBatch = static_cast<DirectX::SpriteBatch*>(apSprite);

    void* p;
    m_pSwapChain->GetDevice(IID_ID3D11Device, &p);
    auto pDevice = static_cast<ID3D11Device*>(p);

    ID3D11DeviceContext* pContext = nullptr;
    pDevice->GetImmediateContext(&pContext);

    if (!pContext)
        return;

    ID3D11CommandList* pCommandList = nullptr;
    const auto result = pContext->FinishCommandList(FALSE, &pCommandList);

    if (result == S_OK)
    {
        if (pCommandList)
        {
            pContext->ExecuteCommandList(pCommandList, TRUE);
            pCommandList->Release();
        }

        std::unique_lock<std::recursive_mutex> _(m_textureLock);
        pSpriteBatch->Draw(m_pTextureView.Get(), DirectX::SimpleMath::Vector2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f);
    }
}

void OverlayRenderHandlerD3D11::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    std::unique_lock<std::recursive_mutex> _(m_textureLock);

    rect = CefRect(0, 0, m_width, m_height);
}

void OverlayRenderHandlerD3D11::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
    const RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (type == PET_VIEW)
    {
        void* p;
        m_pSwapChain->GetDevice(IID_ID3D11Device, &p);
        auto pDevice = static_cast<ID3D11Device*>(p);

        ID3D11DeviceContext* pContext = nullptr;
        pDevice->GetImmediateContext(&pContext);

        if (!pContext)
            return;

        const auto oldWidth = m_width;
        const auto oldHeight = m_height;

        m_width = width;
        m_height = height;

        if (oldWidth != m_width || oldHeight != m_height)
        {
            browser->GetHost()->WasResized();
            CreateResources();
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        std::unique_lock<std::recursive_mutex> _(m_textureLock);

        if (!m_pTexture)
        {
            CreateResources();
        }

        ID3D11CommandList* pCommandList = nullptr;
        if (SUCCEEDED(pContext->FinishCommandList(FALSE, &pCommandList)))
        {
            pCommandList->Release();
        }

        pContext->Map(m_pTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        std::memcpy(mappedResource.pData, buffer, width * height * 4);
        pContext->Unmap(m_pTexture.Get(), 0);
    }
}
