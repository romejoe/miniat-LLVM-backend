//===-- MiniATInstrInfo.cpp - MiniAT Instruction Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATInstrInfo.h"

#include "MiniATTargetMachine.h"
#include "MiniATMachineFunction.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "MiniATGenInstrInfo.inc"

// Pin the vtable to this file.
void MiniATInstrInfo::anchor() {}

//@MiniATInstrInfo {
MiniATInstrInfo::MiniATInstrInfo(const MiniATSubtarget &STI)
    : 
      Subtarget(STI) {}

const MiniATInstrInfo *MiniATInstrInfo::create(MiniATSubtarget &STI) {
  return llvm::createMiniATStandardInstrInfo(STI);
}

