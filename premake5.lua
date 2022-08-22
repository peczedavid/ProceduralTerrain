workspace "OpenGLWorkspace"
    architecture "x64"
    startproject "OpenGL-Engine"

    configurations
    {
        "Debug",
        "Release",
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {} -- Lua table
IncludeDir["Glad"] = "OpenGL-Engine/vendor/Glad/include"

group "Dependencies"
    -- Include projects with the premake5.lua file in it
    include "OpenGL-Engine/vendor/Glad"
group ""

project "OpenGL-Engine"
    location "OpenGL-Engine"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
		"%{prj.name}/src",
        "%{IncludeDir.Glad}"
    }

    links
    {
        "Glad",
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on" 

    filter "configurations:Release"
        runtime "Release"
        optimize "on" 