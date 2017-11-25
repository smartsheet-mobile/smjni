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

#ifndef HEADER_JAVA_STRING_INCLUDED
#define HEADER_JAVA_STRING_INCLUDED

#include <smjni/java_ref.h>
#include <smjni/java_exception.h>
#include <smjni/utf_util.h>

namespace smjni
{
    class java_string
    {
    public:
        java_string() = delete;
        java_string(const java_string & str) = delete;
        java_string & operator=(const java_string & src) = delete;
        
        static local_java_ref<jstring> create(JNIEnv * env, const jchar * str, jsize len)
        {
            jstring ret = env->NewString(str, len);
            if (!ret)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("cannot create java string");
            }
            return jattach(env, ret);
        } 
        
        static local_java_ref<jstring> create(JNIEnv * env, const char * str);
        
        static jsize get_length(JNIEnv * env, jstring str)
        {
            if (!str)
                return 0;
            jsize ret = env->GetStringLength(str);
            if (ret < 0)
            {
                java_exception::check(env);
                THROW_JAVA_PROBLEM("invalid string size");
            }
            return ret;
        }
        
        static void get_region(JNIEnv * env, jstring str, jsize start, jsize len, jchar * buf)
        {
            env->GetStringRegion(str, start, len, buf);
            java_exception::check(env);
        }  
    };
}

#endif
