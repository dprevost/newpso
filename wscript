#! /usr/bin/env python
# encoding: utf-8
# :mode=python:  - For jedit

# Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

VERSION='0.5.0'
APPNAME='Photon'
srcdir = '.'
blddir = 'build'

import Options, unittestw
#import Utils #,
#import Build
from Configure import conf

import sys, os
 
def set_options(opt):

   opt.add_option('--enable-dbc', action='store_true', default=True, dest='use_dbc',
      help='enable contract-validation checks to the code. You might want to disable this on production systems for performance reasons.')
   opt.add_option('--disable-dbc', action='store_false', dest='use_dbc',
      help='disable contract-validation checks to the code.')
   opt.add_option('--block-size', type='string', dest='block_size',default='4k',
      help='set the size of Photon blocks (values must be either 2k, 4k or 8k)')
   opt.add_option('--more-tests', action='store_true', default=False,dest='more_tests',
      help='Set this flag to including all the tests. Must be added when running configure')
   opt.add_option('--less-tests', action='store_false', dest='more_tests',
      help='Set this flag to exclude some long-running tests. Must be removed when running configure')
   opt.add_option('--enable-trace', action='store_true', default=False, dest='use_trace',
      help='enable trace (debugging). You should not use this on production systems for performance reasons.')
   opt.add_option('--disable-trace', action='store_false', dest='use_trace',
      help='disable trace.')
   opt.tool_options('gcc')
   opt.tool_options('g++')
   opt.tool_options('javaw')

def configure(conf):

   if sys.platform == 'win32':
      conf.check_tool('msvc')
      conf.env.append_value('LIB', ['kernel32', 'advapi32', 'wsock32'])
   else:
      conf.check_tool('gcc')
      conf.check_tool('g++')

   conf.check_tool('python')
   conf.check_python_version((2,6,0))
   conf.check_tool('java')
   conf.check_jni_headers()

   conf.check_java_class('org.junit.Assert')

   conf.check_tool('unittest')
   conf.check_tool('misc')
   conf.check_tool('doxygen', tooldir='Configure')
   conf.check_tool('errorparser', tooldir='Configure')

   conf.define('PACKAGE_VERSION', "0.5.0")

	# this one requires cppunit
#	conf.check_cfg(package='cppunit', args='--cflags --libs')
#	if 'LIB_CPPUNIT' in conf.env:
#		if 'dl' not in conf.env['LIB_CPPUNIT']:
#			l = conf.check(lib='dl', uselib_store='CPPUNIT')
#		Utils.pprint('CYAN', "To run unit tests use 'waf check'")
#	else:
#		conf.fatal('Install cppunit')

   conf.sub_config('Configure')

   conf.find_program('doxygen', var='DOXYGEN')
   conf.find_program('latex', var='LATEX')
   conf.find_program('docbook2x-man', var='DOCBOOK2X_MAN')
   conf.find_program('db2x_docbook2man', var='DOCBOOK2X_MAN')
   conf.find_program('db2pdf', var='DB2PDF')
   conf.find_program('errorParser', var='ERROR_PARSER')

   if sys.platform != 'win32':
      conf.check_cfg(package='libxml-2.0', args='--cflags --libs', mandatory=True)

   block_size = Options.options.block_size
   if (block_size == '2k'):
      conf.define('PSON_BLOCK_SIZE',  2048)
      conf.define('PSON_BLOCK_SHIFT',   11)
   elif (block_size == '4k'):
      conf.define('PSON_BLOCK_SIZE',  4096)
      conf.define('PSON_BLOCK_SHIFT',   12)
   elif (block_size == '8k'):
      conf.define('PSON_BLOCK_SIZE',  8192)
      conf.define('PSON_BLOCK_SHIFT',   13)
   else:
      conf.fatal('Invalid value! Values allowed: 2k, 4k or 8k')
   
   # create the second environment, set the variant and set its name
   env2 = conf.env.copy()
   env2.set_variant('release')
   conf.set_env_name('release', env2)

   conf.setenv('release')
   if sys.platform == 'win32':
      conf.env.CCFLAGS = conf.env['CCFLAGS_RELEASE']
   else:
      conf.env.CCFLAGS = ['-O2']
   #top_bld = os.path.join(conf.blddir, conf.env.variant())
   #conf.define('BUILD_DIR', top_bld)
    
   conf.setenv('default')
   if sys.platform == 'win32':
      conf.env.CCFLAGS = conf.env['CCFLAGS_DEBUG']
      #['/Od', '/MDd', '-DDEBUG']
   else:
      conf.env.CCFLAGS = ['-O0', '-g3', '-DDEBUG']
   #top_bld = os.path.join(conf.blddir, conf.env.variant())
   #conf.define('BUILD_DIR', top_bld)

   conf.write_config_header('src/config.h', conf.setenv('default') )
   conf.write_config_header('src/config.h', conf.setenv('release') )
   
def build(bld):
   #bld.add_subdirs(['src', 'doc'])
   bld.add_subdirs(['src'])
   print bld.env['MORE_TESTS']
   print sys.platform, ' ', os.name

   print bld.env['WX_FLAGS']
   
def check(context):
   print 'ok check'
   # Unit tests are run when "check" target is used
   ut = UnitTest.unit_test()
   #ut.change_to_testfile_dir = True
   #ut.want_to_see_test_output = True
   ut.run()
   ut.print_results()


def shutdown():
   if sys.platform != 'win32':
      os.system('find . -name \'*~\' -exec rm {} \;' )


