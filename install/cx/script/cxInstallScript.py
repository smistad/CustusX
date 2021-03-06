#!/usr/bin/env python

#####################################################
# Unix setup script
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2012.01.19
#
# Description:
#
# Script for installation of new workstation for CustusX
# based on description from:
# http://cxserver.sintef.no:16080/wiki/index.php/Installeringsveiviser
#
# Run cxInstaller --help to get usage info.
#
# prerequisites:
# boost, cmake, cppunit, eclipse
# Need a github key installed.
#
# Default values:
# - change the class Common
#
# default folder setup (feel free to modify)
#
# --root---external---ITK---ITK
#    |         |       |----build_Debug  
#    |         |       |----build_Release
#    |         |       |----build32_Debug
#    |         |
#    |         |------VTK---VTK
#    |                 |----build_Debug  
#    |                 |----build_Release
#    |                 |----build32_Debug
#    |-----workspace
#              |------CustusX---CustusX
#                      |--------build_Debug  
#                      |--------build_Release
#                      |--------build32_Debug
#             
#################################################             

import subprocess
import argparse
import re
import sys
import os.path
import urllib
import getpass
import platform

from cx.utils.cxShell import *
from cx.utils.cxPrintFormatter import PrintFormatter

import cx.build.cxInstallData
import cx.build.cxComponents
import cx.build.cxComponentAssembly

import cxBuildScript

class Controller(cxBuildScript.BuildScript):
    '''
    '''

    def getDescription(self):                  
        return '''
Installer script for CustusX and its components.

Root folder is cxrepo/../.. = %s

Available components are:
   %s.
''' % (self.controlData().getRootDir(),
       '\n   '.join(self.assembly.getLibnames()))
       
    def setDefaults(self):                
        super(Controller, self).setDefaults()
        self.controlData().setBuildType("Debug")
        shell.setRedirectOutput(False)

    def addArgParsers(self):        
        super(Controller, self).addArgParsers()
        self.additionalParsers.append(self.controlData().getArgParser_core_build())
        self.additionalParsers.append(self.controlData().getArgParser_extended_build())
        self.additionalParsers.append(self.getArgParser())
       
    def applyArgumentParsers(self, arguments):
        arguments = super(Controller, self).applyArgumentParsers(arguments)
        (self.options, arguments) = self.getArgParser().parse_known_args(arguments)
        print 'Options: ', self.options
        return arguments

    def getArgParser(self):
        p = argparse.ArgumentParser(add_help=False)
        p.add_argument('--checkout', '--co', action='store_true', help='checkout all selected components')
        p.add_argument('--configure_clean', action='store_true', help='delete build folder(s), configure all selected components')
        p.add_argument('--configure', '--conf', action='store_true', help='configure all selected components')
        p.add_argument('-m', '-b', '--make', action='store_true', help='build all selected components')
        p.add_argument('-a', '--all', action='store_true', help='select all components')
        p.add_argument('-f', '--full', action='store_true', help='checkout, configure, make')
        p.add_argument('-c', '--clean', action='store_true', help='make clean')
        p.add_argument('-s', '--silent_mode', action='store_true', help='execute script without user interaction')
        p.add_argument('components', nargs='*', help='list of all components to process')                                
        return p

    def _promptToContinue(self, silent_mode):
        if not silent_mode:
            raw_input("\nPress enter to continue or ctrl-C to quit:")

    def run(self):
        options = self.options
        assembly = self.assembly
                
        if options.all:
            assembly.selectAllLibraries()
        else:
            assembly.selectLibraries(options.components)

        # display help if no components selected
        if len(assembly.getSelectedLibraries())==0:
            PrintFormatter.printInfo("No libraries selected, exiting...")
            self.argumentParser.print_help()
            return

        # info + verification
        assembly.controlData.printSettings()
        print 'Use the following components:\n ', '\n  '.join(assembly.getSelectedLibraries())
        
        print ''
        print '*********************************************************************'
        print 'The superbuild will check out and build in [repo/../..], expanded to:'
        print ''
        print '    %s' % assembly.controlData.getRootDir()
        print ''
        print '*********************************************************************'
        print ''
        
        self._promptToContinue(options.silent_mode)
        
        assembly.process(checkout = options.full or options.checkout, 
                         configure_clean = options.configure_clean, 
                         configure = options.full or options.configure, 
                         clean = options.clean, 
                         build = options.full or options.make)        
        
        #self.cxBuilder.finish()
        PrintFormatter.finish()


