#! /usr/bin/env python
# encoding: UTF-8
# Thomas Nagy 2008

#from fnmatch import fnmatchcase
import os, os.path
import pproc
import Task, Utils, Node, Constants
from TaskGen import feature
import xml.dom.minidom

def getValue(nodelist):
   '''
   Retrieve the value associated with a specific xml tag
   '''
   value = ""
   for node in nodelist:
      if node.nodeType == node.TEXT_NODE:
         value = node.data
         return value.strip()
   return value


class parser_task(Task.Task):

   vars = ['ERROR_PARSER']
   color = 'BLUE'
   before = 'cc'
   quiet = True

   def runnable_status(self):
      '''
      Removed self.output from the calculation of the signature, otherwise
      waf thinks that the task needs to be run every time.
      Don't know what I'm doing wrong but it works now, so leave it alone...
      
      '''
      o = self.outputs
      self.outputs = []
      xxx = Task.Task.runnable_status(self)
      self.outputs = o
      return xxx

   def run(self):
      cmd = 'cd %s && %s --options %s %s' % (self.inputs[0].parent.abspath(), self.env['ERROR_PARSER'], self.inputs[0].file(), self.inputs[1].file())
      proc = pproc.Popen(cmd, shell=True,stdout=pproc.PIPE, stderr=pproc.PIPE)
      out, err = proc.communicate()
      if proc.returncode != 0:
         color = 'RED'
         Utils.pprint(color, err)
      
      return proc.returncode

   def post_run(self):
      for node in self.outputs:
         sg = Utils.h_file(node.abspath())
         variant = node.variant(self.env)
         self.generator.bld.node_sigs[variant][node.id] = sg
      
      return Task.Task.post_run(self)

   def getOptions(self,options):
      '''
      Extract all the generated output files (by error Parser) as they are 
      defined in the xml option file.
   
      Note: this function is incomplete. Java, python and c# are not handled yet.
      '''
      outputFiles = []
      dom = xml.dom.minidom.parse(options)

      nodes = dom.getElementsByTagName('header_dirname')
      if nodes.length > 0:
         dir_name = getValue(nodes.item(0).childNodes)

         nodes = dom.getElementsByTagName('header_name')
         if nodes.length > 0:
            outputFiles.append(dir_name + os.sep + getValue(nodes.item(0).childNodes))

      nodes = dom.getElementsByTagName('errmsg_dirname')
      if nodes.length > 0:
         dir_name = getValue(nodes.item(0).childNodes)

         nodes = dom.getElementsByTagName('errmsg_header_name')
         if nodes.length > 0:
            outputFiles.append(dir_name + os.sep + getValue(nodes.item(0).childNodes))
      else:
         nodes = dom.getElementsByTagName('errmsg_header_name')
         if nodes.length > 0:
            outputFiles.append(getValue(nodes.item(0).childNodes))

      nodes = dom.getElementsByTagName('errmsg_c_fullname')
      if nodes.length > 0:
         outputFiles.append(getValue(nodes.item(0).childNodes))

      dom.unlink()
      return outputFiles

def detect(conf):
   conf.find_program('errorParser', var='ERROR_PARSER', mandatory=True)

