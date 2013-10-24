#!/usr/bin/env python
# encoding: utf-8

import os

release = ARGUMENTS.get('release', 0)

env = Environment()

# print env.Dump()

CPPPATH = ['/usr/local/include', os.path.abspath('.')]
CPPDEFINES = []
LIBPATH = ['/usr/local/lib']
LIBS = ['pthread', 'protobuf', 'PocoFoundation', 'PocoUtil', 'PocoCrypto', 'PocoNet', 'gflags', 'glog']
STATICLIBS = []
if (int(release) == 0):
    CPPFLAGS = ['-g', '-fPIC', '-O3',]
else:
    CPPFLAGS = ['-fPIC', '-O3']
LINKFLAGS = ['-rdynamic']

# env['CXX'] = 'llvm-g++-4.2'
# env['CC'] = 'llvm-gcc-4.2'
env['CPPPATH'] = CPPPATH
env['CPPDEFINES'] = CPPDEFINES
env['CPPFLAGS'] = CPPFLAGS
env['LIBPATH'] = LIBPATH
env['LIBS'] = LIBS
env['STATICLIBS'] = STATICLIBS
env['LINKFLAGS'] = LINKFLAGS

Export('env')

SConscript(['PocoRpc/rpc_proto/SConscript',
            'PocoRpc/rpc_def/SConscript',
            'PocoRpc/rpclib/SConscript',
            'PocoRpc/sample/server/SConscript',
            'PocoRpc/sample/client/SConscript',
            'PocoRpc/temptest/SConscript'
            ])

print("==> 编译参数")
print("\t CPPPATH = %s" % ' '.join(env['CPPPATH']))
print("\t CPPDEFINES = %s" % ' '.join(env['CPPDEFINES']))
print("\t CPPFLAGS = %s" % ' '.join(env['CPPFLAGS']))
print("\t LIBPATH = %s" % ' '.join(env['LIBPATH']))
print("\t LIBS = %s" % ' '.join(env['LIBS']))
print("\t STATICLIBS = %s" % ' '.join(env['STATICLIBS']))
print("\t LINKFLAGS = %s" % ' '.join(env['LINKFLAGS']))

