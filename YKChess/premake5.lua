VULKAN_SDK = os.getenv("VULKAN_SDK")

project "YKChess"
  location "."
  language "C++"
  cppdialect "C++latest"
  staticruntime "On"

  targetdir "%{wks.location}/Bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  objdir "%{wks.location}/Bin-Int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

  files
  {
    "Source/**.cpp",
    "Source/**.h",
    "Source/**.inl",
    "premake5.lua"
  }

  includedirs
  {
    "Source",
    "%{wks.location}/Deps/YKLib/YKLib/Source",
    "%{VULKAN_SDK}/Include",
    "%{wks.location}/Deps/glfw/include",
    "%{wks.location}/Deps/imgui"
  }

  links
  {
    "YKLib",
    "vulkan-1.lib",
    "GLFW",
    "imgui"
  }

  libdirs
  {
    "%{VULKAN_SDK}/Lib"
  }

  filter { "configurations:Debug*" }
    kind "ConsoleApp"

  filter { "configurations:Release*" }
    kind "ConsoleApp"

  filter { "configurations:Profile*" }
    kind "ConsoleApp"

  filter { "configurations:Final*" }
    kind "WindowedApp"
    entrypoint "mainCRTStartup"

  filter {}