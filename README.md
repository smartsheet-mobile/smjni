# SmJNI library #

A powerful lighweight C++ wrapper for JNI

## Purpose

Writing C++ code for JNI is hard, error prone and unsafe. The purpose of this library is to make it easy and safe.
It targets two main scenarios

1. Call Java code from C++
2. Implement native Java methods in C++

The approach this library is taking is different from SWIG and JNA. Unlike either it does not attempt to easily
expose existing C or C++ code to Java. Trying to do so is, in our opinion, rarely a good idea. Instead it does
exactly the opposite: easily exposes Java to C++.
With this library if you want to simply connect existing C++ code to Java you will need to write your own wrappers.
However, doing so will become easy and straightforward. As will be more sophisticated things like implementing
classes partially natively or using Java from within C++ application.

### Goals

*    Never have to figure out things like "([BLjava/lang/String;)Ljava/lang/Throwable;"
*    Never have to call vararg (...) functions hoping that you got the arguments right. And debug weird behavior when you didn't...
*    Type safety: never have to worry "is this jobject what I think it is?"
*    Proper error handling. Use exceptions instead of error-prone and ill-defined manual checks.
     If Java call threw an exception it should automatically become C++ exception. You should also be able to easily
     convert it back to Java when returning from JNI call. 
*    Dealing with JNI local/global reference stuff should be safe and easy by default. 
*    It should be possible to access Java arrays via STL collection interface. 
*    You should never have to deal with *modified UTF-8*. All string operation should either use UTF-16 or standard UTF-8
*    It should be possible to get the JNIEnv in arbitrary C++ code without the trouble of passing it through every call.
*    You shouldn't need to find Java classes again and again every time you want to use them.
*     All of the above should be accomplished without sacrificing performance compared to "raw" JNI use. However, if there
     is an unavoidable choice between marginal performance gain and correctness/safety the later should win.
*     All of the JNI functionality required to write correct code should be available in the library. There should never be a need
     to manually invoke JNIEnv methods. Conversely dangerous (e.g. ThrowNew) and unnecessary JNI methods should not
     be exposed.
*    Modularity: it should be possible to mix and match parts of library with hand written JNI code if desired. It should not
     force all or nothing approach.

### Non-Goals

*    Automatically expose C or C++ code to Java
*    Completely hide JNI fundamentals: existence of per thread JNIEnv *, global/local reference semantics etc. from C++ code
*    Provide C++ wrappers for common Java classes
*    Provide Java wrappers for common C++ classes
*    Support old C++ compilers and libraries. This library requires C++14. 

## Building

Smjni originated on Android so it comes with Android.mk and build.gradle for ndk-build and experimental Gradle plugin respectively.
In addition it provides portable CMake script that *should* work everywhere.
Instructions for users of other build systems are provided at the end of this section

### Android 

Smjni requires to be built with RTTI and exceptions enabled. It should work with GNU STL or  LLVM libc++. It might or might not work with STLPort. 
As on every othr platfrom C++14 mode is required.

#### Using ndk-build

Clone this repository somewhere

In your Android.mk you should have the following settings in addition to other flags

```make
LOCAL_CPP_FEATURES := rtti exceptions
LOCAL_CPPFLAGS :=  -std=gnu++14
```

Specify dependency on smjni as follows
```make
LOCAL_STATIC_LIBRARIES += smjni
```

and at the end add 
```make
include /path/to/smjni/Android.mk
```

#### Using experimental gradle plugin

Clone this repository somewhere

In your settings.gradle specify

```groovy
include ...all the other projects you have..., ':smjni'
project(':smjni').projectDir = new File('/path/to/smjni')
```

In your JNI module build.gradle specify the following in addition to other JNI settings you already have

```groovy
model {
    android {
        ndk {
            cppFlags.add("-std=gnu++14")
            cppFlags.add("-fvisibility=hidden")
            cppFlags.add("-frtti")
            cppFlags.add("-fexceptions")
        }
        sources {
            main {
                jni {
                    dependencies {
                        project ":smjni" linkage "static"
                    }
                }
            }
        }
}
```

### CMake

Clone this repository somewhere

Add the following to your CMakeLists.txt

```cmake
add_subdirectory("/path/to/smjni" ${CMAKE_CURRENT_BINARY_DIR}/smjni)

#set relevant C++14 flag for your compiler here
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

target_link_libraries(your_target_name smjni ${JAVA_JVM_LIBRARY})
```

### Other build systems

You will need to 

* Build all *.cpp files under src directory and link them with your code
* Add inc directory to your include path

Build requires C++14 and a modern standard library.

