### 生成编译文件：
- cmake ../ -G Ninja -DCMAKE_BUILD_TYPE=Release -DADS_VERSION=5.14.2
- cmake ../ -G Ninja -DCMAKE_BUILD_TYPE=Debug -DADS_VERSION=5.14.2
### 编译
- NINJA
### Debug 信息输出
- 修改CMakeLists.txt文件注释WIN32 (#WIN32)