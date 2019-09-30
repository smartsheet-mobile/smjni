#
# Copyright 2019 SmJNI Contributors
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

project(smjnitests)

add_subdirectory("../../.." ${CMAKE_CURRENT_BINARY_DIR}/smjni)

add_library(smjnitests SHARED EXCLUDE_FROM_ALL)

target_link_libraries(smjnitests PRIVATE
    smjni
)

set_property(TARGET smjnitests PROPERTY CXX_STANDARD 17)
set_property(TARGET smjnitests PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET smjnitests PROPERTY CXX_VISIBILITY_PRESET hidden)
set_property(TARGET smjnitests PROPERTY VISIBILITY_INLINES_HIDDEN ON)

target_sources(smjnitests PRIVATE
    smjnitests.cpp
)

install(TARGETS smjnitests DESTINATION ${CMAKE_CURRENT_LIST_DIR}/../../build/native)
