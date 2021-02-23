
#include "HttpRequest.hpp"

namespace TiltedPhoques
{
    class RequestClient final : public CefURLRequestClient
    {
    public:
        // impl: CefURLRequestClient
        void OnRequestComplete(CefRefPtr<CefURLRequest> request) override
        {

        }

        void OnUploadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override
        {
            m_totalUpload = total;
        }

        void OnDownloadProgress(CefRefPtr<CefURLRequest> request, int64 current, int64 total) override
        {
            m_totalDownload = total;
        }

        void OnDownloadData(CefRefPtr<CefURLRequest> request, const void* data, size_t data_length) override
        {

        }

        bool GetAuthCredentials(bool isProxy, const CefString& host, int port, const CefString& realm, const CefString& scheme, CefRefPtr<CefAuthCallback> callback) override
        {
            // not handled
            return false;
        }
    private:
        IMPLEMENT_REFCOUNTING(RequestClient);

    private:
        int64_t m_totalUpload = 0;
        int64_t m_totalDownload = 0;
        std::string m_data;
    };

    CefRefPtr<CefURLRequest> HttpRequestFactory::CreateGetRequest(std::string_view url)
    {
        auto request = CefRequest::Create();
        request->SetURL(CefString(url.data()));
        request->SetMethod("GET");

        auto client = new RequestClient();
        return CefURLRequest::Create(request, client, nullptr);
    }
}
