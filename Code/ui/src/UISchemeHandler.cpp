
#include <TiltedCore/Filesystem.hpp>
#include "UISchemeHandler.hpp"

namespace TiltedPhoques
{
    class OverlayResourceHandler final : public CefResourceHandler
    {
    public:
        bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override
        {
            return false;
        }

        bool ProcessRequest(CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback) override
        {
            return false;
        }

        void GetResponseHeaders(CefRefPtr<CefResponse> response, int64& response_length, CefString& redirectUrl) override
        {
            
        }

        bool Skip(int64 bytes_to_skip, int64& bytes_skipped, CefRefPtr<CefResourceSkipCallback> callback) override
        {
            return false;
            
        }

        bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override
        {
            return false;
        }

        bool ReadResponse(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback) override
        {
            return false;
        }

        void Cancel() override
        {

        }

        IMPLEMENT_REFCOUNTING(OverlayResourceHandler);
    };

    CefRefPtr<CefResourceHandler> UISchemeFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& scheme_name, CefRefPtr<CefRequest> request)
    {
        if (scheme_name == "tp")
            return new OverlayResourceHandler();

        return nullptr;
    }
}
