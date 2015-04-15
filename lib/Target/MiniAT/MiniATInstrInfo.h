//===-- MiniATInstrInfo.h - MiniAT Instruction Information ----------*- C++ -*-===//
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

#ifndef MINIATINSTRUCTIONINFO_H
#define MINIATINSTRUCTIONINFO_H

#include "MiniAT.h"
#include "MiniATRegisterInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Target/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "MiniATGenInstrInfo.inc"

namespace llvm {

class MiniATInstrInfo : public MiniATGenInstrInfo {
  virtual void anchor();
protected:
  const MiniATSubtarget &Subtarget;
public:
  explicit MiniATInstrInfo(const MiniATSubtarget &STI);

  static const MiniATInstrInfo *create(MiniATSubtarget &STI);

  /// getRegisterInfo - TargetInstrInfo is a superset of MRegister info.  As
  /// such, whenever a client has an instance of instruction info, it should
  /// always be able to get register info as well (through this method).
  ///
  virtual const MiniATRegisterInfo &getRegisterInfo() const = 0;

  virtual MachineInstr* emitFrameIndexDebugValue(MachineFunction &MF,
          int FrameIx, uint64_t Offset,
          const MDNode *MDPtr,
          DebugLoc DL) const;


    void storeRegToStackSlot(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator MBBI,
            unsigned SrcReg, bool isKill, int FrameIndex,
            const TargetRegisterClass *RC,
            const TargetRegisterInfo *TRI) const override {
        storeRegToStack(MBB, MBBI, SrcReg, isKill, FrameIndex, RC, TRI, 0);
    }

    void loadRegFromStackSlot(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator MBBI,
            unsigned DestReg, int FrameIndex,
            const TargetRegisterClass *RC,
            const TargetRegisterInfo *TRI) const override {
        loadRegFromStack(MBB, MBBI, DestReg, FrameIndex, RC, TRI, 0);
    }

    virtual void storeRegToStack(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator MI,
            unsigned SrcReg, bool isKill, int FrameIndex,
            const TargetRegisterClass *RC,
            const TargetRegisterInfo *TRI,
            int64_t Offset) const = 0;

    virtual void loadRegFromStack(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator MI,
            unsigned DestReg, int FrameIndex,
            const TargetRegisterClass *RC,
            const TargetRegisterInfo *TRI,
            int64_t Offset) const = 0;

    void copyPhysReg(MachineBasicBlock &MBB,
            MachineBasicBlock::iterator MI, DebugLoc DL,
            unsigned DestReg, unsigned SrcReg,
            bool KillSrc) const;
};
const MiniATInstrInfo *createMiniATStandardInstrInfo(const MiniATSubtarget &STI);
}

#endif

