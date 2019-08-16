#include <OverlayRenderHandlerD3D9.h>
#include <d3d9.h>
#include <d3dx9tex.h>

OverlayRenderHandlerD3D9::OverlayRenderHandlerD3D9(Renderer* apRenderer) noexcept
    : m_pRenderer(apRenderer)
{
    apRenderer->OnRender.Connect(std::bind(&OverlayRenderHandlerD3D9::Render, this, std::placeholders::_1));
    apRenderer->OnLost.Connect(std::bind(&OverlayRenderHandlerD3D9::Lost, this, std::placeholders::_1));
}

OverlayRenderHandlerD3D9::~OverlayRenderHandlerD3D9() = default;

void OverlayRenderHandlerD3D9::Render(IDirect3DDevice9* apDevice)
{
    D3DXVECTOR3 pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;

    m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
    m_pSprite->Draw(m_pTexture.Get(), nullptr, nullptr, &pos, 0xFFFFFFFF);
    m_pSprite->End();
}

void OverlayRenderHandlerD3D9::Lost(IDirect3DDevice9* apDevice)
{
    CreateResources();
}

void OverlayRenderHandlerD3D9::CreateResources()
{
    m_pTexture.Reset();
    m_pSprite.Reset();

    auto pDevice = m_pRenderer->GetDevice();

    D3DXCreateSprite(pDevice, &m_pSprite);

    D3DVIEWPORT9 viewport;
    pDevice->GetViewport(&viewport);

    m_width = viewport.Width;
    m_height = viewport.Height;

    if (D3DXCreateTexture(pDevice, viewport.Width, viewport.Height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, &m_pTexture) != S_OK)
    {
        // TODO: Error handling
    }
}

void OverlayRenderHandlerD3D9::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect)
{
    rect = CefRect(0, 0, m_width, m_height);
}

void OverlayRenderHandlerD3D9::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height)
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
            CreateResources();
        }

        D3DLOCKED_RECT area;
        m_pTexture->LockRect(0, &area, nullptr, D3DLOCK_DISCARD);

        const auto pDest = static_cast<uint8_t*>(area.pBits);
        std::memcpy(pDest, buffer, width * height * 4);

        m_pTexture->UnlockRect(0);
    }
}
