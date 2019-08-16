#include <OverlayBrowserProcessHandler.h>

void OverlayBrowserProcessHandler::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    command_line->AppendSwitchWithValue("pid", std::to_string(GetCurrentProcessId()));
}
