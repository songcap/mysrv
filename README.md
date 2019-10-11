#mysrv c++服务端程序

##开发环境

Ubuntu16.04 LTS
gcc
cmake

##项目路径
bin -- 二进制文件
build --cmake中间文件
cmake -- cmake函数的文件夹
CMakeList.txt -- cmake 的定义文件
lib -- 库的输出路径
src -- 源代码目录
tests --测试代码

##日志系统
1)
	log4J
	logger（定义日志类别）
	   |
	   |--------Formatter（日志格式）
	   |
	Appender （日志输出地）
##配置系统

Config --> Yaml
使用yaml—cpp github开源的工具
安装方式
mkdir build && cd build && cmake .. && make install
```cpp
YAML::Node node = YAML::LoadFile(filename)
node.IsMap() //如果是map就遍历所有的元素

node.IsSequence() //还是遍历

node.IsScalar() // 直接输出
```

##协程库封装

##socket函数库

##http协议开发

##分布协议

##推荐系统
