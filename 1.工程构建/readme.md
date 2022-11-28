**交流QQ群：963944097**

C语言的诞生，让人类进入大规模软件编程时代，但是源文件太多，还要兼容各种操作系统，这个需求便诞生CMake工具，自动管理源代码，自动生成各种操作系统的工程文件。

编译器使用很麻烦，它的参数众多，一般项目源代码众多，更麻烦的是不同的操作系统编译器不一样。CMake专门解决这个问题。不用考虑编译器不一样，CMake自动帮你生成工程文件，你只要写好源代码就行。

**构成构建工具很多，我们选择CMake，目标是实现管理源代码和自动实现跨平台。力求一份代码跑全部平台。实现一个人团队。**
1. 不同的操作系统，有不同编译器，不同的IDE。可以让我们选择更优秀开发工具，比如VS或者XCode开发项目，而程序可以运行在Linux中。
2. 大幅降低开发成本，不受限平台，不会为了特定平台开发而购买特定平台设备。

## 1.使用CMake做跨平台构建工具。
写一份CMake工程文件，可以自动生成各个平台的工程文件，快速实现在VS，XCode或者AS等零成本切换。

### 1.1.CMake脚本文件简介
CMake文件暂时设计成如下，随着不断地摸索，不断地进行修改和添加。
```powershell
#指定CMAKE版本
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
#项目名称，这个是CMake工程的项目名称
PROJECT(server)
#server是生产执行程序名称，main.c是C语言源代码文件
add_executable(server main.c)
#by linyou
```

### 1.2.工程文件结构
整个工程项目文件结构如下，以后都按照这种形式

```powershell
./CMakeLists.txt  cmake根工程文件 
./os            垮平台目 
./os/win 
./os/linux 
./os/mac 
./os/android 
./src                 文件夹src，专门放源代码 
./src/main.c          main.c C语言源文件 
./external             第三方扩展库 
./build               文件夹build，CMake构建文件和编译器编译生成文件 
#by linyou
```
./ 是我们的工程文件夹。以后工程目录就指这个路径。
我们追求一份代码跑全部平台。以Linux开源平台为标准，如果在windows平台，缺少函数或者库，就模仿Linux接口，开发一套放到os/win目录。./os存放各个操作系统的实现，为我们的业务（./src）提供统一接口。
./src是业务源代码，./external存放第三方扩展库，但是，我们尽可能不使用第三方库。

### 1.3. 第一个例子Hello World
源代码main.c的内容如下
```c
#include <stdio.h>
int main()
{    
  printf("Hello, Linyou! \n");    
  getchar();    
  return 0;
}
//by linyou
```
环境搭建，我们支持全部平台，需要确保你的操作系统安装了编译器。
1. linux使用gcc编译器，yum进行安装。参看网上教程。
2. windows使用cl.exe编译器，安装Microsoft Visual Studio免费社区版会自带。
3. mac/ios使用llvm编译器， 安装Xcode就会自动安装。
4. android 使用ndk编译器，安装Android Studio，需要选择下载make，安卓比较特殊，它是构建工程后配置CMake脚本，不能用CMake自动生成安卓工程。

通过上面可以看出，太多编译器了，工程文件五花八门，幸好有CMake，它把自动帮你做好，你只需要安心写代码就可以。

**安装CMake工具，确保在终端执行cmake -version有信息，我们把windows的命令行工具cmd也叫终端。因为我们以Linux开源系统作为标准。CMake也有可视化版本，可以更方便选择编译器和开发IDE。**

我们在windows上开发写代码，做好的程序上线运行在Linux上。

使用终端(cmd命令行工具)cd到我们的工程目录（./）。此时我们可以直接执行cmake命令，因为在我们的工程目录下有一份CMakeLists.txt文件，cmake会自动默认路径是 ./，自动找到CMakeLists.txt，但是这样不好，cmake生成的构建文件跟我们的文件混到一起。

所以，我们cd到构建目录./build。此时执行cmake会失败，因为在./build目录没有CMakeLists.txt文件，我们只要加参数 ../即可，cmake会跳到父目录，自动找到CMakeLists.txt

### 1.4最后执行编译
windows下，会有server.sln文件，鼠标双击就可以在Visual Studio上编译执行。
Linux下，会有Makefile文件，直接执行make，生成执行程序server，执行server即可。
mac下也类似，
Android是把CMake脚本内置到工程文件中，无需上述操作，只需在IDE上执行编译即可以。
在linux系统命令方式例子：
```powershell
cd ./build 
cmake .. 
make ./server
```

## 2.CMake构建深化
上面只是我们的一个简单的例子，我们接下来继续完善我们的CMakeLists.txt文件。
需求：我们需要打印cmake信息，查看cmake的变量，在cmake上按不同的系统选择不同的源文件，希望自动添加源文件。我们想尽办法自动化。

在cmake上打印信息使用  message(STATUS "Hello，Linyou ")
```powershell
#指定CMAKE版本 
CMAKE_MINIMUM_REQUIRED(VERSION 3.8) 
#项目名称，这个是CMake工程的项目名称 
PROJECT(server) 
message(STATUS "Hello，Linyou ")
 #server是生产执行程序名称，main.c是C语言源代码文件 
 add_executable(server main.c) 
 #by linyou
```
执行cmake ../，就可以看到cmake输出日志

**cmake的全部变量存放在内部变量VARIABLES上，把它打印出来就可以看到它的内容**
```powershell
CMAKE_MINIMUM_REQUIRED(VERSION 3.8) 
PROJECT(server) 
#获取内部变量VARIABLES 
get_cmake_property(_variableNames VARIABLES) 
#对key进行排序，方便看信息 
list (SORT _variableNames) 
#迭代打印信息 
foreach (_variableName ${_variableNames})    
	message(STATUS "${_variableName}=${${_variableName}}") 
endforeach() 
add_executable(server main.c) 
#by linyou
```
下面选择性列出一些信息，用${XXX}形式可以单独访问，如${CMAKE_PROJECT_NAME}。CMake有大量对编译器的信息。我们需要确保文件的路径在哪里，cmake能不能找到它，cmake生成的工程文件能不能让编译器找到我们的源代码。以及生成的执行程序能不能找到我们的动态库。

```powershell
CMAKE_BINARY_DIR=D:/svn/project/build 
CMAKE_CACHEFILE_DIR=d:/svn/project/build 
CMAKE_COMMAND=D:/Program Files/CMake/bin/cmake.exe 
CMAKE_CURRENT_BINARY_DIR=D:/svn/project/build 
CMAKE_CURRENT_LIST_DIR=D:/svn/project 
CMAKE_CURRENT_LIST_FILE=D:/svn/project/CMakeLists.txt 
CMAKE_CXX_COMPILER=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/bin/Hostx64/x64/cl.exe 
CMAKE_C_COMPILER=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/bin/Hostx64/x64/cl.exe 
CMAKE_C_STANDARD_LIBRARIES=kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
CMAKE_FILES_DIRECTORY=/CMakeFiles 
CMAKE_FIND_LIBRARY_PREFIXES=;lib 
CMAKE_FIND_LIBRARY_SUFFIXES=.lib;.a 
CMAKE_HOME_DIRECTORY=D:/svn/project 
CMAKE_LINKER=C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.33.31629/bin/Hostx64/x64/link.exe 
CMAKE_PROJECT_NAME=server 
CMAKE_SOURCE_DIR=D:/svn/project
CMAKE_SYSTEM_NAME=Windows 
PROJECT_NAME=server 
PROJECT_SOURCE_DIR=D:/svn/project
```
最后一个需求，自动添加源文件。
file(GLOB_RECURSE ALL_SRC "src/*.h" "src/*.c")这个指令的效果是，获取./src目录下全部C语言源文件.c和头文件.h。然后把它们存放到ALL_SRC 变量。

1. 加文件使用include_directories，加入包含文件目录，它的文件就可以出现在#include <xx.h>上。
2. 使用link_libraries告诉编译器使用静态库的名称，要找到该名称的静态库文件，就需要使用link_directories，它会告诉编译器去哪里找静态库名称对应的库文件。
3. add_executable指定执行程序名称，以及需要编译的源文件，#include <xx.h>一般指定相对路径，需要找具体源文件在哪里，需要include_directories指定，include_directories就会告诉编译器，这里有源文件。

编译器是先对每个源文件进行单独编译，生产.o文件，编译过程中遇到include文件，再导入头文件，最后链接，找静态库，合并程序。

最后我们的CMakeLists.txt如下，它将伴随我们的整个框架设计。

```powershell
CMAKE_MINIMUM_REQUIRED(VERSION 3.12.1) 
PROJECT(server) 
set(OS_FILE_PATH) 
set(OS_LIST) 
MESSAGE(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}") 
if(CMAKE_SYSTEM_NAME MATCHES "Linux") 
 	set(OS_FILE_PATH "os/linux")    
	MESSAGE(STATUS "current platform: Linux") 
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")    
	# 导入windows相关的静态库    
   link_libraries(ws2_32)    
set(OS_FILE_PATH "os/win")    
	MESSAGE(STATUS "current platform: Windows") 
elseif(CMAKE_SYSTEM_NAME MATCHES "Android") 
	set(OS_FILE_PATH "os/android")    
	MESSAGE(STATUS "current platform: Android") 
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin") 
	set(OS_FILE_PATH "os/mac")    
	MESSAGE(STATUS "current platform: Apple") 
else()      
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")   
endif() 
# 导入${OS_FILE_PATH}目录，它的文件就可以出现在#include <xx.h>上include_directories(${OS_FILE_PATH}) 
# 自动查找${OS_FILE_PATH}目录下的源文件文件和头文件 
file(GLOB_RECURSE OS_LIST "${OS_FILE_PATH}/*.cpp" "${OS_FILE_PATH}/*.cc" "${OS_FILE_PATH}/*.h" "${OS_FILE_PATH}/*.hpp" "${OS_FILE_PATH}/*.c") 
set(SRC_FILE_PATH src) 
# 加入包含文件目录，它的文件就可以出现在#include <xx.h>上 
include_directories(${SRC_FILE_PATH}) 
#自动查找源文件 
file(GLOB_RECURSE SRC_LIST 
     "${SRC_FILE_PATH}/*.cpp" 
     "${SRC_FILE_PATH}/*.cc" 
     "${SRC_FILE_PATH}/*.h" 
     "${SRC_FILE_PATH}/*.hpp"   
     "${SRC_FILE_PATH}/*.c" 
    ) 
#执行程序和源代码 
add_executable(server ${SRC_LIST} ${OS_LIST}) 
#可以有多个add_executable，这样可以生成多个工程项目，就可以编译多个项目，获得多个执行程序 
# add_executable(server1 ${SRC_LIST} ${OS_LIST}) 
# add_executable(server2 ${SRC_LIST} ${OS_LIST})
```
现在，我们只要把源文件放入到src，执行cmake ..，就可以开心写代码了。

## 3.特别注意
二进制指令使用困难，开发一个软件项目，需要耗费巨量人力，所以，有人发明了gcc编译器，把人类容易理解的语言翻译成二进制，这个语言就是C/C++语言，脱离了编译器，C/C++语言代码是没有存在意义。gcc编译器的诞生，标志着人类进入大规模软件编程时代。

大规模软件编程时代，单单一个软件项目，就有成千上万个源代码文件，管理这些源文件耗费人力。更大的问题来自跨平台问题，不同的操作系统平台，编译器不一样，工程文件不一样，需要额外的人力管理工程文件。cmake这个工具诞生，自动管理源代码，根据不同的操作系统，自动生成对应的工程文件。gcc编译器也可以在windows上使用，但是每个操作系统厂商重点支持自己的编译器。

软件开发，总是开发更方便的工具给其他人使用，让大家方便和降低成本。

交流QQ群：963944097