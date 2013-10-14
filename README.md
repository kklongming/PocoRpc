PocoRpc: 一个基于Google Protobuf 和 Poco 的C++ RPC框架
============

#如何构建编译环境#
#主要依赖项#
* [Poco](http://pocoproject.org/index.html) C++ Library
* [Google Protobuf](https://code.google.com/p/protobuf/)
* [Google glog](https://code.google.com/p/google-glog/)
* [Google gflags](https://code.google.com/p/gflags/?redir=1)
* [SCons](http://www.scons.org/) 代码构建工具

##Mac下构建编译环境##
* 安装[XCode](https://itunes.apple.com/us/app/xcode/id497799835)
* 安装 **_command line tools for xcode_**, 请[参考这里](http://lunae.cc/setup-command-line-tools-for-xcode)
* 安装[Homebrew](http://brew.sh/) 
* 安装依赖项 

```bash
brew install poco gflags glog protobuf
brew install python scons  
```

##Centos6下构建编译环境##
* 安装开发工具

```bash
yum -y groupinstall  "Development tools"
``` 

* TO DO...


##Ubuntu 下构建编译环境##
* TO DO...