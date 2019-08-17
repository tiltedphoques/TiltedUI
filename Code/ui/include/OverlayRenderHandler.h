#pragma once
#include <include/cef_render_handler.h>

struct OverlayRenderHandler : CefRenderHandler
{
    virtual void Reset() = 0;
    virtual void Render() = 0;
    virtual void Create() = 0;
};
