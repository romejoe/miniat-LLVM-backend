//===-- MiniATSEFrameLowering.h - MiniAT32/64 frame lowering --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSE_FRAMEINFO_H
#define MINIATSE_FRAMEINFO_H

#include "MiniATFrameLowering.h"

namespace llvm {

class MiniATSEFrameLowering : public MiniATFrameLowering {
public:
  explicit MiniATSEFrameLowering(const MiniATSubtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &MF) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

};

} // End llvm namespace

#endif