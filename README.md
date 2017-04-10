![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameServer/master/doc/asserts/imgs/ArkGameServer.png)

##### Copyright © [https://github.com/ArkGame](https://github.com/ArkGame "Ark Game")

##### QQ Group:540961187

## What is it?

ArkGameServer is a lightweight, fast, scalable, distributed plugin framework with server to create game server easily. Greatly inspired by OGRE and Bigworld.

## Features
- General abstract data system
- Interface-oriented programming (IOP)
- Event-driven and data-driven
- Extensible apps, plugins, and modules
- High concurrency and performance by using actor model (Theron)
- Based on the event-driven and attribute-driven can make business more clear and easy to maintenance
- Based on C++ standard, easy to handle and learn
- Cross platform (Include Windows and Linux)
- Distributed server architecture
- Use Excel as configuration files, easier for users
- With existed Unity3D client for rapid development

### FAQ
Please create a new issue to ask a question.

### Unity3D client
[Unity3D Client](https://github.com/ArkGame/ArkClient-Unity3D)

## Architecture

### App Architecture:
![App Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Doc/asserts/imgs/AppArchitecture.png)

### Server Architecture
![Server Architecture](https://raw.githubusercontent.com/ArkGame/ArkGameFrame/master/Doc/asserts/imgs/ServerArchitecture.png)

## Get the Sources:

git clone https://github.com/ArkGame/ArkGameServer.git

or

svn checkout https://github.com/ArkGame/ArkGameServer


## Dependencies

- libevent 2.0.22
- easylogging++ 9.80
- mysql++ 3.2.2
- google protobuf 2.5.0
- pybind11 1.2
- redis-cpp-client 
- Theron 6.00.01
- curl 7.37.1

## Supported Compilers

* GCC >= 4.8 (**<font color=red>Recommend</font> Tested in Ubuntu 15.04**)
* MSVC >= VS2015 (**<font color=red>Recommend</font> Tested in Win7/10**)
* MSVC >= VS2012+ & GCC >= 4.4+ (By using ```boost```, define macro ```HAVE_BOOST```, then you need to add boost include files and libraries yourself, just to replace ```boost::variant``` and ```boost::lexical_cast```)

## Build and Install
### MSVC >= 2015

1. Git pull all source
2. Run ```Dep/build_dep.bat```
3. Open the solution: ```Frame/Ark.sln```
4. Build the solution```Ark.sln```
5. Run ```GenerateConfigXML.bat``` to generate configuration files
6. Run the binary file by ```Bin/Server/Debug/Run.bat```

### CodeBlocks >= 13.1(Only in linux)

1. Git pull all source
2. Run ```Dep/build_dep.sh```
3. Open the solution with CodeBlocks: ```Frame/Ark.workspace```
4. Check ```C++11``` option in CodeBlocks compiler setting
5. Build ```Ark.workspace```
6. Run ```bash ./GenerateConfigXML.sh``` to generate configuration files
6. Run the binary file by ```Bin/Server/Debug/rund.sh```

> ## :exclamation: Fixing some problem
> ### CMake
> 1. Git pull all source
> 2. Install cmake[>= 2.8] please choose options for installing: **Add CMake to the system PATH for all users and restart your computer**
> 3. Install VS2015 or gcc[>= 4.8]
> 4. Run **install.bat** or **install.sh** to build NF
> 5. Run the binary file by **_Out/Server/Debug/run.bat** or **_Out/Server/Debug/rund_cmake.sh**
> 6. (Optional)For Release Version, after above, run **BuildNF.VS2015.Release.X64.bat** or **BuildNF.CMake.Release.sh**, then run the binary file by **_Out/Server/Release/run.bat** or **_Out/Server/Release/run_cmake.sh**

## Documents
[User Manual](https://arkgame.github.io/ArkGameFrame/)

[English](https://github.com/ArkGame/ArkGameFrame/blob/master/Doc/doc_EN.md)

[Mandarin](https://github.com/ArkGame/ArkGameFrame/blob/master/Doc/doc_ZH.md)

## Samples
Please see ```Frame/Samples```.

## License
The ArkGameServer project is currently available under the [Apache License](https://github.com/ArkGame/ArkGameServer/blob/master/LICENSE).