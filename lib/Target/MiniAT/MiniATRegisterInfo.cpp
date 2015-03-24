//===-- MiniATRegisterInfo.cpp - MINIAT Register Information -== --------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MINIAT implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#define DEBUG_TYPE "miniat-reg-info"

#include <llvm/ADT/BitVector.h>
#include "MiniATRegisterInfo.h"

#include "MiniAT.h"
#include "MiniATSubtarget.h"
#include "MiniATMachineFunction.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

#define GET_REGINFO_TARGET_DESC
#include "MiniATGenRegisterInfo.inc"

using namespace llvm;

MiniATRegisterInfo::MiniATRegisterInfo(const MiniATSubtarget &ST)
  : MiniATGenRegisterInfo(MiniAT::r0), Subtarget(ST) {}
//TODO:check above MiniAT::r0

//===----------------------------------------------------------------------===//
// Callee Saved Registers methods
//===----------------------------------------------------------------------===//
/// MiniAT Callee Saved Registers
// In MiniATCallConv.td,
// def CSR_O32 : CalleeSavedRegs<(add LR, FP,
//                                   (sequence "S%u", 2, 0))>;
// llc create CSR_O32_SaveList and CSR_O32_RegMask from above defined.
const uint16_t* MiniATRegisterInfo::
getCalleeSavedRegs(const MachineFunction *MF) const
{
  return CSR_O32_SaveList;
}

const uint32_t*
MiniATRegisterInfo::getCallPreservedMask(CallingConv::ID) const
{
  return CSR_O32_RegMask;
}

// pure virtual method
//@getReservedRegs {
BitVector MiniATRegisterInfo::
getReservedRegs(const MachineFunction &MF) const {
//@getReservedRegs body {
  static const uint16_t ReservedCPURegs[] = {
          MiniAT::r0, //Zero Register
          MiniAT::r1, //Predicate Register
          MiniAT::r253, //RSP
          MiniAT::r254, //SP
          MiniAT::r255 //PC
  };
  BitVector Reserved(getNumRegs());
  typedef TargetRegisterClass::iterator RegIter;

  for (unsigned I = 0; I < array_lengthof(ReservedCPURegs); ++I)
    Reserved.set(ReservedCPURegs[I]);

  return Reserved;
}

//@eliminateFrameIndex {
//- If no eliminateFrameIndex(), it will hang on run. 
// pure virtual method
// FrameIndex represent objects inside a abstract stack.
// We must replace FrameIndex with an stack/frame pointer
// direct reference.
void MiniATRegisterInfo::
eliminateFrameIndex(MachineBasicBlock::iterator II, int SPAdj,
                    unsigned FIOperandNum, RegScavenger *RS) const {
}
//}

// pure virtual method
unsigned MiniATRegisterInfo::
getFrameRegister(const MachineFunction &MF) const {
  const TargetFrameLowering *TFI = MF.getSubtarget().getFrameLowering();

  return TFI->hasFP(MF) ? 512 :
                          (MiniAT::r254);
}

