@echo on

set VulkanLocation=D:\Tools\SDK\VulkanSDK\1.0.30.0\Source\lib\Vulkan-1.lib
:: set GLSLLibraries=D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\glslang.lib D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\OGLCompiler.lib D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\OSDependent.lib D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\HLSL.lib D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\SPIRV.lib D:\Tools\SDK\VulkanSDK\1.0.30.0\glslang\libMTD\SPVRemapper.lib
:: %GLSLLibraries%
:: %GLSLLibraries%
set VulkanCompilerFlags=-D VULKAN=1 -D VK_USE_PLATFORM_WIN32_KHR=1

set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -FC -Z7 /EHsc
set CommonCompilerFlags=-D _ITERATOR_DEBUG_LEVEL=2 -D KANTI_INTERNAL=1 -D KANTI_SLOW=1 -D KANTI_WIN32=1 %VulkanCompilerFlags% %CommonCompilerFlags% 
set CommonLinkerFlags=-incremental:no -opt:ref User32.lib Winmm.lib winmm.lib Ole32.lib %VulkanLocation% 

IF NOT EXIST Build mkdir Build
pushd Build
cl %CommonCompilerFlags% -I..\iaca-win64\ ..\Code\KantiEngine.cpp -FmKantiEngine.map -LD /link -incremental:no -opt:ref -PDB:KantiEngine_%random%.pdb -EXPORT:VulkanInitialize -EXPORT:VulkanUpdateAndRender %VulkanLocation%

cl %CommonCompilerFlags% ..\Code\Win32KantiEngine.cpp /link %CommonLinkerFlags%
popd