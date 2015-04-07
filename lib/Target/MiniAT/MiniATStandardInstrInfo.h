//===-- MiniATStandardInstrInfo.h - MiniAT32/64 Instruction Information ---*- C++ -*-===//
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

#ifndef MINIATSTANDARDINSTRUCTIONINFO_H
#define MINIATSTANDARDINSTRUCTIONINFO_H

#include "MiniATInstrInfo.h"
#include "MiniATStandardRegisterInfo.h"
#include "MiniATMachineFunction.h"

namespace llvm {

class MiniATStandardInstrInfo : public MiniATInstrInfo {
  const MiniATStandardRegisterInfo RI;

public:
  explicit MiniATStandardInstrInfo(const MiniATSubtarget &STI);

  const MiniATRegisterInfo &getRegisterInfo() const override;

  void storeRegToStack(MachineBasicBlock &MBB,
          MachineBasicBlock::iterator MI,
          unsigned SrcReg, bool isKill, int FrameIndex,
          const TargetRegisterClass *RC,
          const TargetRegisterInfo *TRI,
          int64_t Offset) const override;

  void loadRegFromStack(MachineBasicBlock &MBB,
          MachineBasicBlock::iterator MI,
          unsigned DestReg, int FrameIndex,
          const TargetRegisterClass *RC,
          const TargetRegisterInfo *TRI,
          int64_t Offset) const override;

  bool expandPostRAPseudo(MachineBasicBlock::iterator MI) const override;

  /// Adjust SP by Amount bytes.
  void adjustStackPtr(MiniATFunctionInfo *Cpu0FI, unsigned SP, int64_t Amount,
          MachineBasicBlock &MBB, MachineBasicBlock::iterator I)
          const;

  /// Emit a series of instructions to load an immediate. If NewImm is a
  /// non-NULL parameter, the last instruction is not emitted, but instead
  /// its immediate operand is returned in NewImm.
  unsigned loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
          MachineBasicBlock::iterator II, DebugLoc DL,
          unsigned *NewImm) const;
private:
  void ExpandPRet(
          MachineBasicBlock &MBB
          , MachineBasicBlock::iterator I
          , unsigned Opc
  ) const;
};

}

#endif

