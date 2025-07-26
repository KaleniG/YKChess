workspace "YKChess"
  startproject "YKChess"

  configurations 
  { 
    "Debug",
    "DebugDLL",
    "Release",
    "ReleaseDLL",
    "Profile",
    "ProfileDLL",
    "Final",
    "FinalDLL"
  }

  platforms 
  { 
    "Win32",
    "Win64"
  }

  flags
  {
    "MultiProcessorCompile"
  }

  -- platform filters
  filter { "platforms:Win32" }
    system "Windows"
    architecture "x86"
    systemversion "latest"
    defines { "PLATFORM_WINDOWS", "ARCH_X86", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "NOMINMAX" }

  filter { "platforms:Win64" }
    system "Windows"
    architecture "x64"
    systemversion "latest"
    defines { "PLATFORM_WINDOWS", "ARCH_X64", "_CRT_SECURE_NO_WARNINGS", "_SCL_SECURE_NO_WARNINGS", "NOMINMAX" }

  -- Configuration Filters
  filter { "configurations:Debug*" }
    symbols "On"
    optimize "Off"
    defines { "CONFIG_DEBUG" }

  filter { "configurations:Release*" }
    symbols "Off"
    optimize "Full"
    defines { "CONFIG_RELEASE" }

  filter { "configurations:Profile*" }
    symbols "Off"
    optimize "Full"
    defines { "CONFIG_PROFILE" }

  filter { "configurations:Final*" }
    symbols "Off"
    optimize "Full"
    defines { "CONFIG_FINAL" }

group "Deps"
  include "Deps/YKLib/YKLib"
  include "Deps/glfw"
group ""

group "Impl"
  include "YKChess"
group ""
