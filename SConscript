#!/usr/bin/env python
# encoding: utf-8

import os

env = Environment()

# print env.Dump()

CPPPATH = ['/usr/local/include', os.path.abspath('src')]
CPPDEFINES = []
LIBPATH = ['/usr/local/lib']
LIBS = ['pthread', 'protobuf', 'PocoFoundation', 'PocoUtil', 'PocoCrypto', 'PocoNet', 'gflags', 'glog']
STATICLIBS = []
CPPFLAGS = ['-g', '-fPIC', '-O2']
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

