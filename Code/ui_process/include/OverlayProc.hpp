#pragma once

#include <include/cef_app.h>
#include <OverlayRenderProcessHandler.hpp>
#include <functional>

namespace TiltedPhoques
{
    struct OverlayProc final : CefApp
    {
        explicit OverlayProc(const std::function<OverlayRenderProcessHandler* ()>& aFactory) noexcept;
        ~OverlayProc() = default;

        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

        IMPLEMENT_REFCOUNTING(OverlayProc);

    private:

        CefRefPtr<OverlayRenderProcessHandler> m_pRenderProcess;
    };

    int UIMain(const char* acpArgs, HINSTANCE aInstance, const std::function<OverlayRenderProcessHandler* ()>& acFactory) noexcept;
}
