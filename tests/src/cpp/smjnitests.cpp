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

#include <smjni/smjni.h>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include "test_util.h"

using namespace smjni;


JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    try
    {
        jni_provider::init(vm);
        JNIEnv * env = jni_provider::get_jni();
        java_runtime::init(env);

        NATIVE_PROLOG
            java_classes::init(env);

            return JNI_VERSION_1_6;
        NATIVE_EPILOG
    }
    catch(std::exception & ex)
    {
        //If we are here there is no way to communicate with
        //Java - something really bad happened.
        //Let's just log and report failure
        fprintf(stderr, "%s\n", ex.what());

    }
    return 0;
}

jint JNICALL TestSmJNI::testMain(JNIEnv * env, jclass, jstringArray args)
{
    java_array_access argsAccess(env, args);
    std::vector<std::string> cppArgs;
    cppArgs.reserve(argsAccess.size() + 1);
    cppArgs.emplace_back("smjnitests");

    for(local_java_ref<jstring> arg: argsAccess)
    {
        cppArgs.push_back(java_string_to_cpp(env, arg));
    }

    std::vector<char *> cArgs;
    cArgs.reserve(cppArgs.size());
    std::transform(cppArgs.begin(), cppArgs.end(), std::back_inserter(cArgs), [] (auto & arg) {
        return &arg[0];
    });

    return Catch::Session().run(cArgs.size(), &cArgs[0]);
}






