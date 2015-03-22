##===- lib/Target/miniat/Makefile -----------------------------*- Makefile -*-===##
#
#                     The LLVM Compiler Infrastructure
#
# This file is distributed under the University of Illinois Open Source
# License. See LICENSE.TXT for details.
#
##===----------------------------------------------------------------------===##

LEVEL = ../../..
LIBRARYNAME = LLVMMiniATCodeGen
TARGET = MiniAT

# Make sure that tblgen is run, first thing.
BUILT_SOURCES = \
		MiniATGenRegisterInfo.inc \
		MiniATGenInstrInfo.inc \
		MiniATGenAsmWriter.inc \
		MiniATGenDAGISel.inc \
		MiniATGenCallingConv.inc #\
		#MiniATGenDisassemblerTables.inc \
		#MiniATGenSubtargetInfo.inc

DIRS = TargetInfo MCTargetDesc 
# InstPrinter 

include $(LEVEL)/Makefile.common
