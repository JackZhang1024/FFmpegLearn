# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
# 指定最低CMAKE编译版本
cmake_minimum_required(VERSION 3.4.1)
# 指定引入头文件目录 是基于当前文件CMakeLists.txt所在的路径
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/scr/main/jniLibs/other)

#INCLUDE_DIRECTORIES(src/main/jniLibs/include)
#INCLUDE_DIRECTORIES(scr/main/jniLibs/other)

# 指定共享库的搜索路径
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/main/jniLibs/armeabi-v7a)

# 指定源文件目录
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/src/main/cpp SRC_LIST)

add_library( # Sets the name of the library.
             # 设置库的名称
             media-player
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
             # ${CMAKE_SOURCE_DIR}/src/main/cpp/media-player.cpp
             ${CMAKE_SOURCE_DIR}/src/main/cpp/LKJNICall.cpp
             ${CMAKE_SOURCE_DIR}/src/main/cpp/LKFFmpeg.cpp
             ${SRC_LIST})

# 在系统中查找库
# log-lib是别名 log是真正的要查找的库的名称
find_library(log-lib log)

target_link_libraries( # Specifies the target library.
                       # 指定目标库就是最终要生成的.so 文件
                       # 链接已经编译好的其他库文件到目标库上
                       media-player
                       # 编解码（最重要的库）
                       avcodec-57
                       # 设备信息
                       avdevice-57
                       # 滤镜特效处理库
                       avfilter-6
                       # 封装格式处理库
                       avformat-57
                       # 工具库（大部分库都需要这个库的支持）
                       avutil-55
                       # 后期处理
                       postproc-54
                       # 音频采样数据格式转换
                       swresample-2
                       # 视频像素数据格式转换
                       swscale-4
                       # 链接 android ndk 自带的一些库
                       android
                       math
                       # Links the target library to the log library
                       # included in the NDK.
                       # 方法一 -llog 是用于输出日志用的
                       # -llog
                       # 方法二 利用find_library来查找系统的库 log-lib 是别名 log 是系统的库名
                       ${log-lib}
                     )


