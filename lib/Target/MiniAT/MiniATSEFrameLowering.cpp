//===-- MiniATSEFrameLowering.cpp - MiniAT Frame Information ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "MiniATSEFrameLowering.h"

#include "MiniATMachineFunction.h"
#include "MiniATSEInstrInfo.h"
#include "MiniATSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

MiniATSEFrameLowering::MiniATSEFrameLowering(const MiniATSubtarget &STI)
    : MiniATFrameLowering(STI, STI.stackAlignment()) {}

//@emitPrologue {
void MiniATSEFrameLowering::emitPrologue(MachineFunction &MF) const {
}
//}

//@emitEpilogue {
void MiniATSEFrameLowering::emitEpilogue(MachineFunction &MF,
                                 MachineBasicBlock &MBB) const {
}
//}

const MiniATFrameLowering *
llvm::createMiniATSEFrameLowering(const MiniATSubtarget &ST) {
  return new MiniATSEFrameLowering(ST);
}

