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

#include "stdpch.h"
#include <smjni/java_string.h>
#include <smjni/java_type_traits.h>

using namespace smjni;

local_java_ref<jstring> smjni::java_string_create(JNIEnv * env, const char * str)
{
    std::vector<jchar> utf16_str;
    utf8_to_utf16(str, str + (str ? strlen(str) : 0), std::back_inserter(utf16_str));
    
    jsize length = size_to_java(utf16_str.size()); 
    const jchar * start = length != 0 ? &utf16_str[0] : nullptr;
    return java_string_create(env, start, length);
}

local_java_ref<jstring> smjni::java_string_create(JNIEnv * env, const std::string & str)
{
    std::vector<jchar> utf16_str;
    utf8_to_utf16(str.begin(), str.end(), std::back_inserter(utf16_str));

    jsize length = size_to_java(utf16_str.size()); 
    const jchar * start = length != 0 ? &utf16_str[0] : nullptr;
    return java_string_create(env, start, length);
}

std::string smjni::java_string_to_cpp(JNIEnv * env, const auto_java_ref<jstring> & str)
{
    java_string_access access(env, str);
    std::string ret;
    utf16_to_utf8(access.begin(), access.end(), std::back_inserter(ret));
    return ret;
}