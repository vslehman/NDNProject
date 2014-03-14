#! /usr/bin/env python
# encoding: utf-8
# WARNING! Do not edit! http://waf.googlecode.com/git/docs/wafbook/single.html#_obtaining_the_waf_file

from waflib.Task import Task
from waflib.TaskGen import extension
class protoc(Task):
	run_str='${PROTOC} ${PROTOC_FLAGS} ${PROTOC_ST:INCPATHS} ${SRC[0].abspath()}'
	color='BLUE'
	ext_out=['.h','pb.cc']
@extension('.proto')
def process_protoc(self,node):
	cpp_node=node.change_ext('.pb.cc')
	hpp_node=node.change_ext('.pb.h')
	self.create_task('protoc',node,[cpp_node,hpp_node])
	self.source.append(cpp_node)
	if'cxx'in self.features and not self.env.PROTOC_FLAGS:
		self.env.PROTOC_FLAGS='--cpp_out=%s'%node.parent.get_bld().abspath()
	use=getattr(self,'use','')
	if not'PROTOBUF'in use:
		self.use=self.to_list(use)+['PROTOBUF']
def configure(conf):
	conf.check_cfg(package="protobuf",uselib_store="PROTOBUF",args=['--cflags','--libs'])
	conf.find_program('protoc',var='PROTOC')
	conf.env.PROTOC_ST='-I%s'
