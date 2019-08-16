#pragma once

#include <OverlayRenderHandler.h>
#include <Meta.h>

struct IDirect3DTexture9;
struct IDirect3DDevice9;

struct OverlayRenderHandlerD3D9 : OverlayRenderHandler
{
    explicit OverlayRenderHandlerD3D9(IDirect3DDevice9* apDevice) noexcept;
    virtual ~OverlayRenderHandlerD3D9();

    TP_NOCOPYMOVE(OverlayRenderHandlerD3D9);

    // Will be cast to LPD3DXSPRITE
    void Render(void* apSprite) override;

    void CreateResources() override;

    void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) override;
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;

    IMPLEMENT_REFCOUNTING(OverlayRenderHandlerD3D9);

private:
    int m_width{ 0 };
    int m_height{ 0 };
    IDirect3DTexture9* m_pTexture;
    IDirect3DDevice9* m_pDevice;
};
