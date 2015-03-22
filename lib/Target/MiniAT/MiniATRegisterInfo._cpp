//===-- MiniATRegisterInfo.cpp - MiniAT Register Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT implementation of the MRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATRegisterInfo.h"
#include "MiniAT.h"
#include "MiniATInstrInfo.h"
#include "MiniATMachineFunctionInfo.h"
#include "llvm/ADT/BitVector.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-reg-info"

#define GET_REGINFO_TARGET_DESC
#include "MiniATGenRegisterInfo.inc"

MiniATRegisterInfo::MiniATRegisterInfo()
  : MiniATGenRegisterInfo(MiniAT::LR) {
}

// helper functions
static inline bool isImmUs(unsigned val) {
  return val <= 11;
}

static inline bool isImmU6(unsigned val) {
  return val < (1 << 6);
}

static inline bool isImmU16(unsigned val) {
  return val < (1 << 16);
}


static void InsertFPImmInst(MachineBasicBlock::iterator II,
                            const MiniATInstrInfo &TII,
                            unsigned Reg, unsigned FrameReg, int Offset ) {
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc dl = MI.getDebugLoc();

  switch (MI.getOpcode()) {
  case MiniAT::LDWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDW_2rus), Reg)
          .addReg(FrameReg)
          .addImm(Offset)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::STWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::STW_2rus))
          .addReg(Reg, getKillRegState(MI.getOperand(0).isKill()))
          .addReg(FrameReg)
          .addImm(Offset)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::LDAWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDAWF_l2rus), Reg)
          .addReg(FrameReg)
          .addImm(Offset);
    break;
  default:
    llvm_unreachable("Unexpected Opcode");
  }
}

static void InsertFPConstInst(MachineBasicBlock::iterator II,
                              const MiniATInstrInfo &TII,
                              unsigned Reg, unsigned FrameReg,
                              int Offset, RegScavenger *RS ) {
  assert(RS && "requiresRegisterScavenging failed");
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc dl = MI.getDebugLoc();
  unsigned ScratchOffset = RS->scavengeRegister(&MiniAT::GRRegsRegClass, II, 0);
  RS->setUsed(ScratchOffset);
  TII.loadImmediate(MBB, II, ScratchOffset, Offset);

  switch (MI.getOpcode()) {
  case MiniAT::LDWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDW_3r), Reg)
          .addReg(FrameReg)
          .addReg(ScratchOffset, RegState::Kill)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::STWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::STW_l3r))
          .addReg(Reg, getKillRegState(MI.getOperand(0).isKill()))
          .addReg(FrameReg)
          .addReg(ScratchOffset, RegState::Kill)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::LDAWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDAWF_l3r), Reg)
          .addReg(FrameReg)
          .addReg(ScratchOffset, RegState::Kill);
    break;
  default:
    llvm_unreachable("Unexpected Opcode");
  }
}

static void InsertSPImmInst(MachineBasicBlock::iterator II,
                            const MiniATInstrInfo &TII,
                            unsigned Reg, int Offset) {
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc dl = MI.getDebugLoc();
  bool isU6 = isImmU6(Offset);

  switch (MI.getOpcode()) {
  int NewOpcode;
  case MiniAT::LDWFI:
    NewOpcode = (isU6) ? MiniAT::LDWSP_ru6 : MiniAT::LDWSP_lru6;
    BuildMI(MBB, II, dl, TII.get(NewOpcode), Reg)
          .addImm(Offset)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::STWFI:
    NewOpcode = (isU6) ? MiniAT::STWSP_ru6 : MiniAT::STWSP_lru6;
    BuildMI(MBB, II, dl, TII.get(NewOpcode))
          .addReg(Reg, getKillRegState(MI.getOperand(0).isKill()))
          .addImm(Offset)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::LDAWFI:
    NewOpcode = (isU6) ? MiniAT::LDAWSP_ru6 : MiniAT::LDAWSP_lru6;
    BuildMI(MBB, II, dl, TII.get(NewOpcode), Reg)
          .addImm(Offset);
    break;
  default:
    llvm_unreachable("Unexpected Opcode");
  }
}

static void InsertSPConstInst(MachineBasicBlock::iterator II,
                                const MiniATInstrInfo &TII,
                                unsigned Reg, int Offset, RegScavenger *RS ) {
  assert(RS && "requiresRegisterScavenging failed");
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  DebugLoc dl = MI.getDebugLoc();
  unsigned OpCode = MI.getOpcode();

  unsigned ScratchBase;
  if (OpCode==MiniAT::STWFI) {
    ScratchBase = RS->scavengeRegister(&MiniAT::GRRegsRegClass, II, 0);
    RS->setUsed(ScratchBase);
  } else
    ScratchBase = Reg;
  BuildMI(MBB, II, dl, TII.get(MiniAT::LDAWSP_ru6), ScratchBase).addImm(0);
  unsigned ScratchOffset = RS->scavengeRegister(&MiniAT::GRRegsRegClass, II, 0);
  RS->setUsed(ScratchOffset);
  TII.loadImmediate(MBB, II, ScratchOffset, Offset);

  switch (OpCode) {
  case MiniAT::LDWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDW_3r), Reg)
          .addReg(ScratchBase, RegState::Kill)
          .addReg(ScratchOffset, RegState::Kill)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::STWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::STW_l3r))
          .addReg(Reg, getKillRegState(MI.getOperand(0).isKill()))
          .addReg(ScratchBase, RegState::Kill)
          .addReg(ScratchOffset, RegState::Kill)
          .addMemOperand(*MI.memoperands_begin());
    break;
  case MiniAT::LDAWFI:
    BuildMI(MBB, II, dl, TII.get(MiniAT::LDAWF_l3r), Reg)
          .addReg(ScratchBase, RegState::Kill)
          .addReg(ScratchOffset, RegState::Kill);
    break;
  default:
    llvm_unreachable("Unexpected Opcode");
  }
}

bool MiniATRegisterInfo::needsFrameMoves(const MachineFunction &MF) {
  return MF.getMMI().hasDebugInfo() ||
    MF.getFunction()->needsUnwindTableEntry();
}

const MCPhysReg* MiniATRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF)
                                                                         const {
  // The callee saved registers LR & FP are explicitly handled during
  // emitPrologue & emitEpilogue and related functions.
  static const MCPhysReg CalleeSavedRegs[] = {
    MiniAT::R4, MiniAT::R5, MiniAT::R6, MiniAT::R7,
    MiniAT::R8, MiniAT::R9, MiniAT::R10,
    0
  };
  static const MCPhysReg CalleeSavedRegsFP[] = {
    MiniAT::R4, MiniAT::R5, MiniAT::R6, MiniAT::R7,
    MiniAT::R8, MiniAT::R9,
    0
  };
  const TargetFrameLowering *TFI = MF->getTarget().getFrameLowering();
  if (TFI->hasFP(*MF))
    return CalleeSavedRegsFP;
  return CalleeSavedRegs;
}

BitVector MiniATRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();

  Reserved.set(MiniAT::r0);
  Reserved.set(MiniAT::r1);
  Reserved.set(MiniAT::r253);
  Reserved.set(MiniAT::r254);
  Reserved.set(MiniAT::r255);
  return Reserved;
}

bool
MiniATRegisterInfo::requiresRegisterScavenging(const MachineFunction &MF) const {
  return true;
}

bool
MiniATRegisterInfo::trackLivenessAfterRegAlloc(const MachineFunction &MF) const {
  return true;
}

bool
MiniATRegisterInfo::useFPForScavengingIndex(const MachineFunction &MF) const {
  return false;
}

void
MiniATRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                       int SPAdj, unsigned FIOperandNum,
                                       RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");
  MachineInstr &MI = *II;
  MachineOperand &FrameOp = MI.getOperand(FIOperandNum);
  int FrameIndex = FrameOp.getIndex();

  MachineFunction &MF = *MI.getParent()->getParent();
  const MiniATInstrInfo &TII =
          *static_cast<const MiniATInstrInfo*>(MF.getTarget().getInstrInfo());

  const TargetFrameLowering *TFI = MF.getTarget().getFrameLowering();
  int Offset = MF.getFrameInfo()->getObjectOffset(FrameIndex);
  int StackSize = MF.getFrameInfo()->getStackSize();

  #ifndef NDEBUG
  DEBUG(errs() << "\nFunction         : " 
        << MF.getName() << "\n");
  DEBUG(errs() << "<--------->\n");
  DEBUG(MI.print(errs()));
  DEBUG(errs() << "FrameIndex         : " << FrameIndex << "\n");
  DEBUG(errs() << "FrameOffset        : " << Offset << "\n");
  DEBUG(errs() << "StackSize          : " << StackSize << "\n");
  #endif

  Offset += StackSize;

  unsigned FrameReg = getFrameRegister(MF);

  // Special handling of DBG_VALUE instructions.
  if (MI.isDebugValue()) {
    MI.getOperand(FIOperandNum).ChangeToRegister(FrameReg, false /*isDef*/);
    MI.getOperand(FIOperandNum + 1).ChangeToImmediate(Offset);
    return;
  }

  // fold constant into offset.
  Offset += MI.getOperand(FIOperandNum + 1).getImm();
  MI.getOperand(FIOperandNum + 1).ChangeToImmediate(0);
  
  assert(Offset%4 == 0 && "Misaligned stack offset");
  DEBUG(errs() << "Offset             : " << Offset << "\n" << "<--------->\n");
  Offset/=4;
  
  unsigned Reg = MI.getOperand(0).getReg();
  assert(MiniAT::GRRegsRegClass.contains(Reg) && "Unexpected register operand");

  if (TFI->hasFP(MF)) {
    if (isImmUs(Offset))
      InsertFPImmInst(II, TII, Reg, FrameReg, Offset);
    else
      InsertFPConstInst(II, TII, Reg, FrameReg, Offset, RS);
  } else {
    if (isImmU16(Offset))
      InsertSPImmInst(II, TII, Reg, Offset);
    else
      InsertSPConstInst(II, TII, Reg, Offset, RS);
  }
  // Erase old instruction.
  MachineBasicBlock &MBB = *MI.getParent();
  MBB.erase(II);
}


unsigned MiniATRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return MiniAT::254;
}
