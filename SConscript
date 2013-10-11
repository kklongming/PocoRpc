#!/usr/bin/env python
# encoding: utf-8

import os

release = ARGUMENTS.get('release', 0)

env = Environment()

# print env.Dump()

CPPPATH = ['/usr/local/include', os.path.abspath('src')]
CPPDEFINES = []
LIBPATH = ['/usr/local/lib']
LIBS = ['pthread', 'protobuf', 'PocoFoundation', 'PocoUtil', 'PocoCrypto', 'PocoNet', 'gflags', 'glog']
STATICLIBS = []
if (int(release) == 0):
    CPPFLAGS = ['-g', '-fPIC', '-O2']
else:
    CPPFLAGS = ['-fPIC', '-O2']
LINKFLAGS = ['-rdynamic']

# env['CXX'] = 'clang++'
# env['CC'] = 'clang'
env['CPPPATH'] = CPPPATH
env['CPPDEFINES'] = CPPDEFINES
env['CPPFLAGS'] = CPPFLAGS
env['LIBPATH'] = LIBPATH
env['LIBS'] = LIBS
env['STATICLIBS'] = STATICLIBS
env['LINKFLAGS'] = LINKFLAGS

Export('env')

SConscript(['src/rpc_proto/SConscript',
            'src/rpc_def/SConscript',
            'src/rpclib/SConscript',
            'src/server/SConscript',
            'src/client/SConscript',
            'src/temptest/SConscript'
            ])

