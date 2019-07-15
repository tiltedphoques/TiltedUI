#include <OverlayApp.h>

#include <thread>

OverlayApp::OverlayApp(std::function<OverlayRenderProcessHandler*()> aFactory)
    : m_pRenderProcess(aFactory())
{
}

CefRefPtr<CefRenderProcessHandler> OverlayApp::GetRenderProcessHandler()
{
    return m_pRenderProcess;
}

void ExitCheck(CefRefPtr<CefCommandLine> pArgs)
{
    Sleep(5000);

    auto pid = pArgs->GetSwitchValue("pid");
    DWORD parentId = std::stoul(pid.c_str());

    auto hProcess = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
        FALSE, parentId);

    while (WaitForSingleObject(hProcess, INFINITE) == WAIT_TIMEOUT);

    exit(0);
}

int TiltedUIMain(const char* acpArgs, HINSTANCE aInstance, std::function<OverlayRenderProcessHandler* ()> aFactory)
{
    auto pArgs = CefCommandLine::CreateCommandLine();
    pArgs->InitFromString(acpArgs);

    std::thread t{ ExitCheck, pArgs };
    t.detach();

    CefMainArgs main_args(aInstance);
    CefRefPtr<OverlayApp> App = new OverlayApp(aFactory);

    return CefExecuteProcess(main_args, App, NULL);
}