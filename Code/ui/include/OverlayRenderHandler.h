#pragma once
#include <include/cef_render_handler.h>

struct OverlayRenderHandler : CefRenderHandler
{
    virtual void CreateResources() = 0;
    virtual void Render(void* apSprite) = 0;
};
