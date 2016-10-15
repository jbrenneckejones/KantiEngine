@echo on

set VulkanLocation=D:\Tools\SDK\VulkanSDK\1.0.26.0\Source\lib\Vulkan-1.lib
set GLSLLibraries=D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\glslang.lib D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\OGLCompiler.lib D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\OSDependent.lib D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\HLSL.lib D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\SPIRV.lib D:\Tools\SDK\VulkanSDK\1.0.26.0\glslang\libMTD\SPVRemapper.lib
set VulkanCompilerFlags=-D VK_USE_PLATFORM_WIN32_KHR=1

set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4189 -wd4100 -wd4996 -wd4201 -FC -Z7 /EHsc
set CommonCompilerFlags=-D _ITERATOR_DEBUG_LEVEL=2 -D KANTI_INTERNAL=1 -D KANTI_SLOW=1 -D KANTI_WIN32=1 %VulkanCompilerFlags% %CommonCompilerFlags% 
set CommonLinkerFlags=-incremental:no -opt:ref User32.lib Winmm.lib %VulkanLocation% %GLSLLibraries%

IF NOT EXIST Build mkdir Build
pushd Build
cl %CommonCompilerFlags% ..\Code\Win32KantiEngine.cpp /link %CommonLinkerFlags%
popd