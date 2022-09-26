workspace "OpenGLWorkspace"
    architecture "x64"
    startproject "OpenGL-Engine"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {} -- Lua table
IncludeDir["Glad"] = "OpenGL-Engine/vendor/Glad/include"
IncludeDir["GLFW"] = "OpenGL-Engine/vendor/GLFW/include"
IncludeDir["ImGui"] = "OpenGL-Engine/vendor/imgui"
IncludeDir["glm"] = "OpenGL-Engine/vendor/glm"
IncludeDir["stb_image"] = "OpenGL-Engine/vendor/stb_image"
IncludeDir["spdlog"] = "OpenGL-Engine/vendor/spdlog/include"

group "Dependencies"
    -- Include projects with the premake5.lua file in it
    include "OpenGL-Engine/vendor/Glad"
    include "OpenGL-Engine/vendor/GLFW"
    include "OpenGL-Engine/vendor/imgui"
group ""

project "OpenGL-Engine"
    location "OpenGL-Engine"
    language "C++"
    kind "ConsoleApp"
    cppdialect "C++17"
    staticruntime "on"
    entrypoint "WinMainCRTStartup"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "pch.h"
    pchsource "%{prj.name}/src/pch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
		"%{prj.name}/src",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb_image}",
        "%{IncludeDir.spdlog}"
    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib",
        "Dxgi.lib",
        "Dxgi.dll"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        defines { "ENABLE_ASSERTS" }
        
        filter "configurations:Release"
        runtime "Release"
        optimize "on"
        defines { "ENABLE_ASSERTS" }

    filter "configurations:Dist"
        runtime "Release"
        kind "WindowedApp"
        optimize "on"
        defines { "DIST_BUILD" }
    
    --TODO: CLEAN, REBUILD COMMANDS
    postbuildcommands {
        "{COPY} imgui.ini %{cfg.targetdir}",
        "{COPY} assets %{cfg.targetdir}/assets"
    }