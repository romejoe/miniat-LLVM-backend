//===-- MiniATTargetMachine.cpp - Define TargetMachine for MiniAT -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about MiniAT target spec.
//
//===----------------------------------------------------------------------===//

#include "MiniATTargetMachine.h"
#include "MiniAT.h"
#include "MiniATSubtarget.h"
#include "MiniATTargetObjectFile.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

#define DEBUG_TYPE "miniat"

extern "C" void LLVMInitializeMiniATTarget() {
  // Register the target.
  RegisterTargetMachine<MiniATTargetMachine> X(TheMiniATTarget);
}

// DataLayout --> Big-endian, 32-bit pointer/ABI/alignment
// The stack is always 8 byte aligned
// On function prologue, the stack is created by decrementing
// its pointer. Once decremented, all references are done with positive
// offset from the stack/frame pointer, using StackGrowsUp enables
// an easier handling.
// Using CodeModel::Large enables different CALL behavior.
MiniATTargetMachine::
MiniATTargetMachine(const Target &T, StringRef TT,
                    StringRef CPU, StringRef FS, const TargetOptions &Options,
                    Reloc::Model RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL)
  : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL)
    , TLOF(make_unique<MiniATTargetObjectFile>())
    , Subtarget(nullptr), DefaultSubtarget(TT, CPU, FS, RM, this) {
  Subtarget = &DefaultSubtarget;
  initAsmInfo();
}

MiniATTargetMachine::~MiniATTargetMachine() {}

void MiniATStandardTargetMachine::anchor() { }

MiniATStandardTargetMachine::
MiniATStandardTargetMachine(const Target &T, StringRef TT,
                            StringRef CPU, StringRef FS, const TargetOptions &Options,
                            Reloc::Model RM, CodeModel::Model CM,
                            CodeGenOpt::Level OL)
  : MiniATTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL) {}

void MiniATStandadTargetMachine::anchor() { }

} // namespace

TargetPassConfig *MiniATTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new MiniATPassConfig(this, PM);
}