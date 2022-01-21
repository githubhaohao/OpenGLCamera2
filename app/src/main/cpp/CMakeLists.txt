# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

include_directories(
        util
        render
        glm
        context
        example
        thirdparty
)

add_library( # Sets the name of the library.
        byteflow_render

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        JniImpl.cpp
        render/GLByteFlowRender.cpp
        render/GLUtils.cpp
        context/RenderContext.cpp
        example/ConveyorBeltExample_1.cpp
        example/BluelineChallengeExample.cpp
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# 设置 thirdPartyLibs 目录的值，后面会引用这个变量${thirdPartyLibs}
set(thirdPartyLibs "${CMAKE_SOURCE_DIR}/../thirdPartyLibs")

add_library(opencv_java3 SHARED IMPORTED)
set_target_properties(opencv_java3
        PROPERTIES
        IMPORTED_LOCATION
        ${thirdPartyLibs}/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java3.so)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        byteflow_render

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        GLESv3
        android
        opencv_java3
        )