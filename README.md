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
yum -y install openssl-devel
yum -y install scons
``` 

* 编译安装Poco

```bash
wget http://pocoproject.org/releases/poco-1.4.6/poco-1.4.6p1-all.tar.gz 
tar -vxf poco-1.4.6p1-all.tar.gz
cd poco-1.4.6p1-all
./configure --config=Linux --omit=Data/MySQL,Data/ODBC,PageCompiler --cflags=-fPIC --no-samples --no-tests --static --shared
make && make install
```

* 编译安装 gflags, glog, protobuf

```bash
wget http://gflags.googlecode.com/files/gflags-2.0.tar.gz
tar -vxf gflags-2.0.tar.gz
cd gflags-2.0
./configure --enable-shared --enable-static
make && make install
```

```bash
wget http://google-glog.googlecode.com/files/glog-0.3.3.tar.gz
tar -vxf glog-0.3.3.tar.gz
cd glog-0.3.3
./configure --enable-shared --enable-static
make && make install
```

```bash
wget http://protobuf.googlecode.com/files/protobuf-2.5.0.tar.bz2
tar -vxf protobuf-2.5.0.tar.bz2
cd protobuf-2.5.0
./configure
make && make install
```

* 添加***usr_local_lib.conf***, 保证能够正常从 **/usr/local/lib** 下加载动态库

```bash
echo "/usr/local/lib" > /etc/ld.so.conf.d/usr_local_lib.conf
ldconfig -v
```

##Ubuntu 下构建编译环境##
* TO DO...