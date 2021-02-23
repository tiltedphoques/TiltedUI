#pragma once

#include <include/cef_urlrequest.h>

namespace TiltedPhoques
{
    class HttpRequestCallback
    {
    public:
        virtual void OnDownload() = 0;
    };

    class HttpRequestFactory
    {
    public:
        static CefRefPtr<CefURLRequest> CreateGetRequest(std::string_view aUrl);
    };
}
