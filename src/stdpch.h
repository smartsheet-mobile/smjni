/*
 Copyright 2014 Smartsheet Inc.
 Copyright 2019 SmJNI Contributors
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef HEADER_STDPCH_H_INCLUDED
#define HEADER_STDPCH_H_INCLUDED

#include <jni.h>

#if __has_include(<pthread.h>)

    #include <pthread.h>
    #define USE_PTHREADS 1

#elif __has_include(<windows.h>)
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <windows.h>

    #define USE_WINTHREADS 1

#else

    #error Please define threading for your platform

#endif

#include <memory>
#include <mutex>
#include <string>
#include <exception>
#include <utility>
#include <vector>


#endif //HEADER_STDPCH_H_INCLUDED
