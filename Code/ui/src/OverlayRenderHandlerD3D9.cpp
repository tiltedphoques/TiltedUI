#include <OverlayRenderHandlerD3D9.h>
#include <OverlayClient.h>

#include <d3d9.h>
#include <d3dx9tex.h>

OverlayRenderHandlerD3D9::OverlayRenderHandlerD3D9(Renderer* apRenderer) noexcept
    : m_pRenderer(apRenderer)
{
}

OverlayRenderHandlerD3D9::~OverlayRenderHandlerD3D9() = default;

void OverlayRenderHandlerD3D9::Render()
{
    D3DXVECTOR3 pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;

    std::scoped_lock _(m_textureLock);

    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    m_pSprite->Draw(m_pTexture.Get(), nullptr, nullptr, &pos, 0xFFFFFFFF);
    m_pSprite->End();
}

void OverlayRenderHandlerD3D9::Reset()
{
    Create();
}

void OverlayRenderHandlerD3D9::Create()
{
    std::scoped_lock _(m_textureLock);

    const auto pDevice = m_pRenderer->GetDevice();

    GetRenderTargetSize();

    D3DXCreateSprite(pDevice, m_pSprite.ReleaseAndGetAddressOf());

    CreateRenderTexture();
}

void OverlayRenderHandlerD3D9::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect = CefRect(0, 0, m_width, m_height);
}

void OverlayRenderHandlerD3D9::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
{
    if (type == PET_VIEW)
    {
        std::scoped_lock _(m_textureLock);

        if (!m_pTexture)
            CreateRenderTexture();

        D3DLOCKED_RECT area;
        m_pTexture->LockRect(0, &area, nullptr, D3DLOCK_DISCARD);

        const auto pDest = static_cast<uint8_t*>(area.pBits);
        std::memcpy(pDest, buffer, width * height * 4);

        m_pTexture->UnlockRect(0);
    }
}

void OverlayRenderHandlerD3D9::CreateRenderTexture()
{
    if (D3DXCreateTexture(m_pRenderer->GetDevice(), m_width, m_height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, m_pTexture.ReleaseAndGetAddressOf()) != S_OK)
    {
        // TODO: Error handling
    }
}

void OverlayRenderHandlerD3D9::GetRenderTargetSize()
{
    D3DVIEWPORT9 viewport;
    m_pRenderer->GetDevice()->GetViewport(&viewport);

    if ((m_width != viewport.Width || m_height != viewport.Height) && m_pParent)
    {
        m_width = viewport.Width;
        m_height = viewport.Height;

        {
            std::scoped_lock _(m_textureLock);
            m_pTexture.Reset();
        }

        m_pParent->GetBrowser()->GetHost()->WasResized();
    }
}
