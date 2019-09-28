#
# Copyright 2014 Smartsheet.com, Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

cmake_minimum_required(VERSION 3.5)
project(smjni)

if (NOT ANDROID)
    find_package(JNI)

    set(JNI_INCLUDE_DIRS ${JAVA_INCLUDE_PATH} ${JAVA_INCLUDE_PATH2} CACHE PATH "JNI include dirs")
else()
    set(JNI_INCLUDE_DIRS "")
endif()



file(GLOB SOURCE_FILES "src/*.cpp")

add_library(smjni STATIC ${SOURCE_FILES})

set_property(TARGET smjni PROPERTY CXX_STANDARD 14)
set_property(TARGET smjni PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET smjni PROPERTY CXX_VISIBILITY_PRESET hidden)
set_property(TARGET smjni PROPERTY VISIBILITY_INLINES_HIDDEN ON)

target_compile_options(smjni 
    PUBLIC 
    $<$<CXX_COMPILER_ID:Clang>:-Wno-c++17-compat-mangling>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wno-c++17-compat-mangling>
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>

    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4;/wd4100;/wd4127>
)

target_include_directories(smjni 

    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${JNI_INCLUDE_DIRS}
)

if(SMJNI_INCLUDE_TESTS)
add_subdirectory(tests/src/cpp)
endif()