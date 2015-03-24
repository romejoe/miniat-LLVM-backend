//===-- MiniATSEInstrInfo.cpp - MiniAT32/64 Instruction Information -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT32/64 implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATSEInstrInfo.h"

#include "MiniATMachineFunction.h"
#include "MiniATTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

MiniATSEInstrInfo::MiniATSEInstrInfo(const MiniATSubtarget &STI)
    : MiniATInstrInfo(STI),
      RI(STI) {}

const MiniATRegisterInfo &MiniATSEInstrInfo::getRegisterInfo() const {
  return RI;
}

const MiniATInstrInfo *llvm::createMiniATSEInstrInfo(const MiniATSubtarget &STI) {
  return new MiniATSEInstrInfo(STI);
}

