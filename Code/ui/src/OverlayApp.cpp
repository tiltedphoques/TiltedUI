#include <OverlayApp.h>
#include <filesystem>

OverlayApp::OverlayApp(std::unique_ptr<RenderProvider> apRenderProvider, std::wstring aProcessName) noexcept
    : m_pBrowserProcessHandler(new OverlayBrowserProcessHandler)
    , m_pRenderProvider(std::move(apRenderProvider))
    , m_processName(std::move(aProcessName))
{
}

void OverlayApp::Initialize() noexcept
{
    if (m_pGameClient) 
        return;

    CefMainArgs args(GetModuleHandleA(nullptr));

    std::error_code ec;
    const auto currentPath = std::filesystem::current_path(ec);

    CefSettings settings;

    settings.no_sandbox = true;
    settings.multi_threaded_message_loop = true;
    settings.windowless_rendering_enabled = true;

#ifdef DEBUG
    settings.log_severity = LOGSEVERITY_VERBOSE;
    settings.remote_debugging_port = 8384;
#else
    settings.log_severity = LOGSEVERITY_DEFAULT;
#endif

    CefString(&settings.log_file).FromWString(currentPath / L"logs" / L"cef_debug.log");
    CefString(&settings.cache_path).FromWString(currentPath / L"cache");

    CefString(&settings.browser_subprocess_path).FromWString(currentPath / m_processName);

    CefInitialize(args, settings, this, nullptr);

    m_pGameClient = new OverlayClient(m_pRenderProvider->Create());

    CefBrowserSettings browserSettings{};

    browserSettings.file_access_from_file_urls = STATE_ENABLED;
    browserSettings.universal_access_from_file_urls = STATE_ENABLED;
    browserSettings.web_security = STATE_DISABLED;
    browserSettings.windowless_frame_rate = 60;

    CefWindowInfo info;
    info.SetAsWindowless(m_pRenderProvider->GetWindow());

    CefBrowserHost::CreateBrowser(info, m_pGameClient.get(), (currentPath / L"Data" / L"Online" / L"UI" / L"index.html").wstring(), browserSettings, nullptr, nullptr);
}

void OverlayApp::ExecuteAsync(const std::string& acFunction, const CefRefPtr<CefListValue>& apArguments) const noexcept
{
    if (!m_pGameClient) 
        return;

    auto pMessage = CefProcessMessage::Create("browser-event");
    auto pArguments = pMessage->GetArgumentList();

    const auto pFunctionArguments = apArguments ? apArguments : CefListValue::Create();

    pArguments->SetString(0, acFunction);
    pArguments->SetList(1, pFunctionArguments);

    auto pBrowser = m_pGameClient->GetBrowser();
    if (pBrowser)
    {
        pBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, pMessage);
    }
}

void OverlayApp::InjectKey(cef_key_event_type_t aType, uint32_t aModifiers, uint16_t aKey, uint16_t aScanCode) noexcept
{
}

void OverlayApp::InjectMouseButton(uint16_t aX, uint16_t aY, cef_mouse_button_type_t aButton, bool aUp, uint32_t aModifier) noexcept
{
}

void OverlayApp::InjectMouseMove(uint16_t aX, uint16_t aY, uint32_t aModifier) noexcept
{
}

void OverlayApp::InjectMouseWheel(uint16_t aX, uint16_t aY, uint16_t aDelta, uint32_t aModifier) noexcept
{
}

void OverlayApp::OnBeforeCommandLineProcessing(const CefString& aProcessType, CefRefPtr<CefCommandLine> aCommandLine)
{
}

