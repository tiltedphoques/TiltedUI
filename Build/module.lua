function CreateUIProject(basePath, coreBasePath)
    project ("UI")
        kind ("StaticLib")
        language ("C++")

        includedirs
        {
            basePath .. "/ThirdParty/CEF/",
            basePath .. "/Code/ui/include/",
            coreBasePath .. "/Code/core/include/",
        }

        files
        {
            basePath .. "/Code/ui/include/**.h",
            basePath .. "/Code/ui/src/**.cpp",
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
            basePath .. "/Code/ui_process/include/**.h",
            basePath .. "/Code/ui_process/src/**.cpp",
        }
        
        filter { "architecture:*86" }
            libdirs { basePath .. "/ThirdParty/CEF/lib/Win32" }
            
        filter { "architecture:*64" }
            libdirs { basePath .. "/ThirdParty/CEF/lib/Win64" }
            
        filter { "configurations:Release" }
            links 
            {
                "libcef_r",
                "libcef_dll_wrapper_r"
            }
        
        filter { "configurations:Debug" }
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
