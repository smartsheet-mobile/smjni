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
#include <smjni/java_registration.h>
#include <smjni/java_exception.h>

using namespace smjni;

void java_registration_base::perform(JNIEnv * jenv, jclass clazz) const
{
    typedef decltype(JNINativeMethod::name) name_type;
    typedef decltype(JNINativeMethod::signature) signature_type;

    if (m_entries.empty())
        return;
    std::vector<JNINativeMethod> methods;
    methods.reserve(m_entries.size());
    for(const entry & entry: m_entries)
    {
        methods.emplace_back(JNINativeMethod{
            (name_type)std::get<0>(entry).c_str(),
            (signature_type)std::get<1>(entry).c_str(),
            std::get<2>(entry)
        });
    }
    int res = jenv->RegisterNatives(clazz, &methods[0], size_to_java(methods.size()));
    if (res != 0)
    {
        java_exception::check(jenv);
        THROW_JAVA_PROBLEM("unable to register native methods, error %d", res);
    }
}
