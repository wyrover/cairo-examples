includeexternal ("premake-vs-include.lua")

workspace "cairo-examples"
    language "C++"
    location "build/%{_ACTION}/%{wks.name}"    
    if _ACTION == "vs2015" then
        toolset "v140_xp"
    end

    

    group "cairo-examples"    

            
        create_wtl_project("test_wtl_cario", "src")
            includedirs
            {
                "3rdparty/cairo"
            }
            links
            {
                "3rdparty/cairo/libcairo-2.lib"
            }

        create_wtl_project("test_double_buffer_cario_gdiplus", "src")            
            includedirs
            {
               
                "3rdparty/cairo"
            }
            links
            {
                "3rdparty/cairo/libcairo-2.lib"
            }