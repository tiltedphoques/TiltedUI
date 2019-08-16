#include <OverlayRenderHandlerD3D9.h>
#include <d3d9.h>
#include <d3dx9tex.h>

OverlayRenderHandlerD3D9::OverlayRenderHandlerD3D9(IDirect3DDevice9* apDevice) noexcept
    : m_pTexture(nullptr)
    , m_pDevice(apDevice)
{
    
}

OverlayRenderHandlerD3D9::~OverlayRenderHandlerD3D9()
{
    if (m_pTexture)
        m_pTexture->Release();
}

void OverlayRenderHandlerD3D9::Render(void* apSprite)
{
    auto pSprint = static_cast<LPD3DXSPRITE>(apSprite);

    D3DXVECTOR3 pos;
    pos.x = 0.0f;
    pos.y = 0.0f;
    pos.z = 0.0f;

    pSprint->Draw(m_pTexture, nullptr, nullptr, &pos, 0xFFFFFFFF);

}

void OverlayRenderHandlerD3D9::CreateResources()
{
    if (m_pTexture)
    {
        m_pTexture->Release();
    }

    D3DVIEWPORT9 viewport;
    m_pDevice->GetViewport(&viewport);

    m_width = viewport.Width;
    m_height = viewport.Height;

    if (D3DXCreateTexture(m_pDevice, viewport.Width, viewport.Height, 0, 0, D3DFMT_A8B8G8R8, D3DPOOL_MANAGED, &m_pTexture) != S_OK)
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
