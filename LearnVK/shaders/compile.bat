@echo off
set glslc=C:\VulkanSDK\1.3.290.0\Bin\glslc.exe

(for %%a in ("*.vert") do (
    echo Compiling shader filename: %%a
    glslc %%a -o %%a.spv
))

(for %%a in ("*.frag") do (
    echo Compiling shader filename: %%a
    glslc %%a -o %%a.spv
))
echo Shaders compilation completed
timeout 5