#pragma once

#include <include/cef_v8.h>

#include <unordered_map>
#include <algorithm>

typedef std::unordered_map<std::string, std::pair<CefRefPtr<CefV8Context>, CefRefPtr<CefV8Value>>> callbacks_t;

class EventsV8Handler : public CefV8Handler
{
public:

    EventsV8Handler(CefRefPtr<CefV8Context> apContext);
    ~EventsV8Handler() = default;

    bool Execute(const CefString& acName, CefRefPtr<CefV8Value> apObject, const CefV8ValueList& acArguments, CefRefPtr<CefV8Value>& aReturnValue, CefString& aException) override;

    const callbacks_t& GetCallbacks() const;

    IMPLEMENT_REFCOUNTING(EventsV8Handler);

private:

    callbacks_t m_callbacks;

    CefRefPtr<CefV8Context> m_pContext;
};