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
IncludeDir["PerlinNoise"] = "OpenGL-Engine/vendor/PerlinNoise"

group "Dependencies"
    -- Include projects with the premake5.lua file in it
    include "OpenGL-Engine/vendor/Glad"
    include "OpenGL-Engine/vendor/GLFW"
    include "OpenGL-Engine/vendor/imgui"
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
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl",
        "%{prj.name}/vendor/stb_image/**.h",
        "%{prj.name}/vendor/stb_image/**.cpp",
        "%{prj.name}/vendor/PerlinNoise/**.hpp",
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
        "%{IncludeDir.PerlinNoise}"
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

    filter "configurations:Release"
        runtime "Release"
        optimize "on" 
    
    filter "configurations:Dist"
        runtime "Release"
        optimize "on"
    
    --TODO: CLEAN, REBUILD COMMANDS
    postbuildcommands {
        "{COPY} imgui.ini %{cfg.targetdir}",
        "{COPY} assets %{cfg.targetdir}/assets"
    }