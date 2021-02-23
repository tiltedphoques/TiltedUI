#pragma once

#include <mutex>
#include "UIClient.hpp"

namespace TiltedPhoques
{
    class UIView : public CefRenderHandler
    {
        friend class UIApp;
    public:
        UIView();
        ~UIView();

        bool OpenUrl(const std::string_view url, HWND aHwnd);

        void SetVisible(bool aToggle);
        bool IsVisible() const;

        void ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept;

        virtual void Create() = 0;
        virtual void Render() = 0;
        virtual void Reset() = 0;

        virtual void* GetTextureHandle() { return nullptr; }

        CefString& GetName() noexcept;

    protected:
        CefRefPtr<UIClient> m_pClient;

        CefString m_name;
        bool m_bVisible = true;
        bool m_bMainFrame = false;

        std::mutex m_viewLock;
    };
}
