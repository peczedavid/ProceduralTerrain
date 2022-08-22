workspace "OpenGL-Engine"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "OpenGL-Engine"
    location "Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }


    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on" 

    filter "configurations:Release"
        runtime "Release"
        optimize "on" 