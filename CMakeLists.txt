cmake_minimum_required(VERSION 3.15)

# This file is part of the Aaru Data Preservation Suite.
# Copyright (c) 2019-2023 Natalia Portillo.
#
# This library is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of the
# License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.

IF(APPLE)
    IF("${CMAKE_OSX_ARCHITECTURES}" STREQUAL "")
        SET(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "Build architectures for Mac OS X" FORCE)
    ENDIF()
ENDIF(APPLE)

project("Aaru.Checksums.Native" C)

if("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC" AND "${CMAKE_C_COMPILER_ARCHITECTURE_ID}" MATCHES "ARMV7")
    set(CMAKE_C_STANDARD 11)
else()
    set(CMAKE_C_STANDARD 90)
endif()

message("Detected system processor: ${CMAKE_SYSTEM_PROCESSOR}")
message("Detected vs platform name: ${CMAKE_C_COMPILER_ARCHITECTURE_ID}")
message("Detected compiler: ${CMAKE_C_COMPILER_ID}")
message("Detected build type: ${CMAKE_BUILD_TYPE}")
message("Detected platform: ${CMAKE_C_PLATFORM_ID}")

if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
    if("${CMAKE_C_COMPILER_ID}" MATCHES "MSVC")
        add_compile_options("/O2" "/fp:fast")
        if(${CMAKE_C_COMPILER_ARCHITECTURE_ID} MATCHES "X86" OR ${CMAKE_C_COMPILER_ARCHITECTURE_ID} MATCHES "x64")
            add_compile_options("/arch:SSE2")
        endif()
    else()
        add_compile_options(-ffast-math -O3)

        if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "i686" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "AMD64")
            if(NOT "${CMAKE_C_COMPILER_ID}" MATCHES "AppleClang")
                add_compile_options(-march=core2 -mtune=westmere -mfpmath=sse)
            endif()

            add_compile_options(-msse3)

            if(NOT "${CMAKE_C_PLATFORM_ID}" MATCHES "MinGW")
                add_compile_options(-flto)
            endif()
        elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
            if(NOT "${CMAKE_C_COMPILER_ID}" MATCHES "AppleClang")
                add_compile_options(-march=armv8-a)
            endif()

            if(NOT "${CMAKE_C_PLATFORM_ID}" MATCHES "MinGW")
                add_compile_options(-flto)
            endif()
        elseif(${CMAKE_SYSTEM_PROCESSOR} MATCHES "armv7l")
            if(NOT "${CMAKE_C_COMPILER_ID}" MATCHES "AppleClang")
                add_compile_options(-march=armv7)
            endif()
        endif()
    endif()
endif()

add_library("Aaru.Checksums.Native" SHARED adler32.h adler32.c crc16.h crc16.c crc16_ccitt.h crc16_ccitt.c crc32.c crc32.h crc64.c crc64.h fletcher16.h fletcher16.c fletcher16_avx2.c fletcher16_neon.c fletcher16_ssse3.c fletcher32.h fletcher32.c fletcher32_avx2.c fletcher32_neon.c fletcher32_ssse3.c library.h spamsum.c spamsum.h crc32_clmul.c crc64_clmul.c simd.c simd.h adler32_ssse3.c adler32_avx2.c adler32_neon.c crc32_arm_simd.c crc32_vmull.c crc32_simd.h arm_vmull.c arm_vmull.h crc64_vmull.c library.c)

add_subdirectory(tests)
