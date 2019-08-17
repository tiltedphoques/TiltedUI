#include <OverlayRenderHandlerD3D11.h>

#include <SpriteBatch.h>
#include <DirectXColors.h>
#include <SimpleMath.h>
#include <CommonStates.h>
#include <WICTextureLoader.h>


OverlayRenderHandlerD3D11::OverlayRenderHandlerD3D11(Renderer* apRenderer) noexcept
    : m_pRenderer(apRenderer)
{
}

OverlayRenderHandlerD3D11::~OverlayRenderHandlerD3D11() = default;

void OverlayRenderHandlerD3D11::Render()
{
    // First of all we flush our deferred context in case we have updated the texture
    {
        std::unique_lock<std::recursive_mutex> _(m_textureLock);

        Microsoft::WRL::ComPtr<ID3D11CommandList> pCommandList;
        const auto result = m_pContext->FinishCommandList(FALSE, &pCommandList);

        if (result == S_OK)
        {
            if (pCommandList)
            {
                void* p;
                m_pRenderer->GetSwapChain()->GetDevice(IID_ID3D11Device, &p);
                auto pDevice = static_cast<ID3D11Device*>(p);

                Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
                pDevice->GetImmediateContext(pContext.ReleaseAndGetAddressOf());

                pContext->ExecuteCommandList(pCommandList.Get(), TRUE);
            }
        }
    }

    if (IsVisible())
    {
        m_pSpriteBatch->Begin(DirectX::SpriteSortMode_Deferred, m_pStates->NonPremultiplied());
        m_pSpriteBatch->Draw(m_pTextureView.Get(), DirectX::SimpleMath::Vector2(0.f, 0.f), nullptr, DirectX::Colors::White, 0.f);
        m_pSpriteBatch->Draw(m_pCursorTexture.Get(), DirectX::SimpleMath::Vector2(100, 100), nullptr, DirectX::Colors::White, 0.f, DirectX::SimpleMath::Vector2(m_cursorX, m_cursorY), m_width / 1920.f);
        m_pSpriteBatch->End();
    }
}

void OverlayRenderHandlerD3D11::Reset()
{
    Create();
}

void OverlayRenderHandlerD3D11::Create()
{
    void* p;
    m_pRenderer->GetSwapChain()->GetDevice(IID_ID3D11Device, &p);
    auto pDevice = static_cast<ID3D11Device*>(p);

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
    pDevice->GetImmediateContext(pContext.ReleaseAndGetAddressOf());

    if (!pContext)
        return;

    m_pSpriteBatch = std::make_unique<DirectX::SpriteBatch>(pContext.Get());
    m_pStates = std::make_unique<DirectX::CommonStates>(pDevice);

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pRTV;

    pContext->OMGetRenderTargets(1, pRTV.ReleaseAndGetAddressOf(), nullptr);
    if (pRTV)
    {
        Microsoft::WRL::ComPtr<ID3D11Resource> pSrcResource;
        pRTV->GetResource(&pSrcResource);

        if (pSrcResource)
        {
            {
                std::unique_lock<std::recursive_mutex> _(m_textureLock);

                if (FAILED(pDevice->CreateDeferredContext(0, m_pContext.ReleaseAndGetAddressOf())))
                    return;

                Microsoft::WRL::ComPtr<ID3D11Texture2D> pSrcBuffer;
                pSrcResource.As(&pSrcBuffer);

                D3D11_TEXTURE2D_DESC desc;
                pSrcBuffer->GetDesc(&desc);

                m_width = desc.Width;
                m_height = desc.Height;

                D3D11_TEXTURE2D_DESC textDesc;
                textDesc.Width = desc.Width;
                textDesc.Height = desc.Height;
                textDesc.MipLevels = textDesc.ArraySize = 1;
                textDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                textDesc.SampleDesc.Count = 1;
                textDesc.SampleDesc.Quality = 0;
                textDesc.Usage = D3D11_USAGE_DYNAMIC;
                textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
                textDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                textDesc.MiscFlags = 0;

                const auto res = pDevice->CreateTexture2D(&textDesc, nullptr, m_pTexture.ReleaseAndGetAddressOf());

                if (FAILED(res))
                    return;

                D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
                SRVDesc.Format = textDesc.Format;

                SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
                SRVDesc.Texture2D.MipLevels = 1;

                if (FAILED(pDevice->CreateShaderResourceView(m_pTexture.Get(), &SRVDesc, m_pTextureView.ReleaseAndGetAddressOf())))
                    return;
            }

            DirectX::CreateWICTextureFromFile(pDevice, L"Data\\Online\\UI\\assets\\images\\cursor.png", nullptr, m_pCursorTexture.ReleaseAndGetAddressOf());
        }
    }
}

void OverlayRenderHandlerD3D11::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect = CefRect(0, 0, m_width, m_height);
}

void OverlayRenderHandlerD3D11::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type,
    const RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (type == PET_VIEW)
    {
        const auto oldWidth = m_width;
        const auto oldHeight = m_height;

        m_width = width;
        m_height = height;

        if (oldWidth != m_width || oldHeight != m_height)
        {
            browser->GetHost()->WasResized();
            Create();
        }

        D3D11_MAPPED_SUBRESOURCE mappedResource;

        if (!m_pTexture)
        {
            Create();
        }

        std::unique_lock<std::recursive_mutex> _(m_textureLock);

        m_pContext->Map(m_pTexture.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
        std::copy_n(reinterpret_cast<const uint32_t*>(buffer), width * height, reinterpret_cast<uint32_t*>(mappedResource.pData));
        m_pContext->Unmap(m_pTexture.Get(), 0);
    }
}
