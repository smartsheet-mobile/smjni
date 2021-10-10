/*
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

#ifndef HEADER_SMJNI_CONFIG_H_INCLUDED
#define HEADER_SMJNI_CONFIG_H_INCLUDED

#ifndef __cplusplus

#error This library requires C++

#endif

#if __cplusplus < 201402L

#error This library requires C++14 or above

#endif

#if defined(_MSC_VER)
    #pragma message("SmJNI is no longer actively maintained. For an actively maintained and supported fork please migrate to SimpleJNI at https://github.com/gershnik/SimpleJNI")
    #define SMJNI_FORCE_INLINE __forceinline
    #define SMJNI_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC warning "SmJNI is no longer actively maintained. For an actively maintained and supported fork please migrate to SimpleJNI at https://github.com/gershnik/SimpleJNI"
    #define SMJNI_FORCE_INLINE [[gnu::always_inline]] inline
    #define SMJNI_NO_INLINE [[gnu::noinline]]
#endif

#endif