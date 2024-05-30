# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-src"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-build"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/tmp"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/src"
  "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Graphics Programming 2/Vulkan/out/build/x64-Debug/_deps/stb-subbuild/stb-populate-prefix/src/stb-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
