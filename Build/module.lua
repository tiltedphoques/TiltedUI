function CreateUIProject(basePath, coreBasePath)
    project ("UI")
        kind ("StaticLib")
        language ("C++")
        
        links
        {
            "dxguid.lib",
            "d3d9.lib",
            "d3d11.lib",
            "d3dx9.lib",
            "MSVCRT.LIB",
            "DirectXTK.lib"
        }

        includedirs
        {
            basePath .. "/ThirdParty/DirectXTK/include/",
            basePath .. "/ThirdParty/CEF/",
            basePath .. "/Code/ui/include/",
            coreBasePath .. "/Code/core/include/",
            "$(DXSDK_DIR)/Include",
        }

        files
        {
            basePath .. "/Code/ui/include/**.hpp",
            basePath .. "/Code/ui/src/**.cpp",
        }
        
        filter { "architecture:*86" }
            libdirs 
            {
                "$(DXSDK_DIR)/Lib/x86",
                "$(VsInstallDir)DIA SDK/lib",
                basePath .. "/ThirdParty/DirectXTK/lib/x32",
                basePath .. "/ThirdParty/CEF/lib/Win32"
            }

        filter { "architecture:*64" }
            libdirs 
            {
                "$(DXSDK_DIR)/Lib/x64",
                "$(VsInstallDir)DIA SDK/lib/amd64",
                basePath .. "/ThirdParty/DirectXTK/lib/x64",
                basePath .. "/ThirdParty/CEF/lib/Win64"
            }
            
        filter {}
            

        filter { "configurations:not *Debug" }
            links 
            {
                "libcef_r",
                "libcef_dll_wrapper_r"
            }
        
        filter { "configurations:*Debug" }
            links 
            {
                "libcef_d",
                "libcef_dll_wrapper_d"
            }
        
        filter {}
        
        links
        {
            "Core"
        }
end
   
function CreateUIProcessProject(basePath, coreBasePath)   
    project ("UIProcess")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            basePath .. "/ThirdParty/CEF/",
            basePath .. "/Code/ui_process/include/",
            coreBasePath .. "/Code/core/include/",
        }

        files
        {
            basePath .. "/Code/ui_process/include/**.hpp",
            basePath .. "/Code/ui_process/src/**.cpp",
        }
        
        filter { "architecture:*86" }
            libdirs { basePath .. "/ThirdParty/CEF/lib/Win32" }
            
        filter { "architecture:*64" }
            libdirs { basePath .. "/ThirdParty/CEF/lib/Win64" }
            
        filter { "configurations:not *Debug" }
            links 
            {
                "libcef_r",
                "libcef_dll_wrapper_r"
            }
        
        filter { "configurations:*Debug" }
            links 
            {
                "libcef_d",
                "libcef_dll_wrapper_d"
            }
        
        filter {}
        
        links
        {
            "Core"
        }
end
