#pragma once
#include <include/cef_render_handler.h>

struct OverlayRenderHandler : CefRenderHandler
{
    virtual void CreateResources() = 0;
};
