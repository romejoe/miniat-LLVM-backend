//===-- MiniATRegisterInfo.h - MiniAT Register Information Impl -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATREGISTERINFO_H
#define MINIATREGISTERINFO_H

#include "MiniAT.h"
#include "llvm/Target/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "MiniATGenRegisterInfo.inc"

namespace llvm {
class MiniATSubtarget;
class TargetInstrInfo;
class Type;

struct MiniATRegisterInfo : public MiniATGenRegisterInfo {
  const MiniATSubtarget &Subtarget;

  MiniATRegisterInfo(const MiniATSubtarget &Subtarget);

  /// getRegisterNumbering - Given the enum value for some register, e.g.
  /// MiniAT::LR, return the number that it corresponds to (e.g. 14).
  static unsigned getRegisterNumbering(unsigned RegEnum);

  /// Code Generation virtual methods...
  const uint16_t *getCalleeSavedRegs(const MachineFunction* MF = 0) const;
  const uint32_t *getCallPreservedMask(CallingConv::ID) const;

  BitVector getReservedRegs(const MachineFunction &MF) const;

  /// Stack Frame Processing Methods
  void eliminateFrameIndex(MachineBasicBlock::iterator II,
                           int SPAdj, unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  /// Debug information queries.
  unsigned getFrameRegister(const MachineFunction &MF) const;

  /// \brief Return GPR register class.
  virtual const TargetRegisterClass *intRegClass(unsigned Size) const = 0;
};

} // end namespace llvm

#endif

