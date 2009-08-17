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

def getOptions(options):
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
      #print 'xxx = ', xxx
      self.outputs = o
      return xxx

   def run(self):
      cmd = 'cd %s && %s --options %s %s' % (self.inputs[0].parent.abspath(), self.env['ERROR_PARSER'], self.inputs[0].file(), self.inputs[1].file())
      proc = pproc.Popen(cmd, shell=True)
      #print cmd
      #print self.inputs[0].file()
      #color = 'RED'
      #Utils.pprint(color, 'Re-run waf')
      
      return proc.returncode

   def post_run(self):
      return Task.Task.post_run(self)
      
@feature('errorparser')
def process_parser(self):
   if not getattr(self, 'option_file', None):
      raise ValueError, 'option_file parameter is missing'

   if not getattr(self, 'xml_file', None):
      raise ValueError, 'xml_file parameter is missing'

   opt_node = self.path.find_resource(self.option_file)
   if not opt_node: raise ValueError, 'option file not found'

   xml_node = self.path.find_resource(self.xml_file)
   if not xml_node: raise ValueError, 'xml file not found'
   
   # the task instance
   tsk = self.create_task('parser')
   tsk.set_inputs( [opt_node, xml_node] )

   outfiles = getOptions(opt_node.nice_path())
   
   outputs = []
   for f in outfiles:
      out = self.path.find_resource(Utils.split_path(f))
      if not out: raise ValueError, f + ' file not found'
      outputs.append(out)
      
   if len(outputs) > 0:
      tsk.set_outputs( outputs )

def detect(conf):
   conf.find_program('errorParser', var='ERROR_PARSER', mandatory=True)

