/*
 Copyright 2014 Smartsheet.com, Inc.
 
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

using namespace smjni;

local_java_ref<jstring> java_string::create(JNIEnv * env, const char * str)
{
    std::vector<jchar> utf16_str;
    utf8_to_utf16(str, str + (str ? strlen(str) : 0), std::back_inserter(utf16_str));
    
    jsize length = utf16_str.size(); 
    const jchar * start = length != 0 ? &utf16_str[0] : nullptr;
    return create(env, start, length);
}