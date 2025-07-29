@ECHO OFF
FOR %%I IN (Assets/Shaders/*.vert) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.tesc) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.tese) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.geom) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.frag) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.comp) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rgen) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rahit) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rchit) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rmiss) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rint) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.rcall) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.task) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
FOR %%I IN (Assets/Shaders/*.mesh) DO CALL %%VULKAN_SDK%%/Bin/glslc.exe Assets/Shaders/%%I -o Assets/Shaders/%%I.spv
PAUSE