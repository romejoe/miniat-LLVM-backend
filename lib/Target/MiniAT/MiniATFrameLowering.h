//===-- MiniATFrameLowering.h - Define frame lowering for MiniAT ----*- C++ -*-===//
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
#ifndef MINIAT_FRAMEINFO_H
#define MINIAT_FRAMEINFO_H

#include "MiniAT.h"
#include "llvm/Target/TargetFrameLowering.h"

namespace llvm {
  class MiniATSubtarget;

class MiniATFrameLowering : public TargetFrameLowering {
protected:
  const MiniATSubtarget &STI;

public:
  explicit MiniATFrameLowering(const MiniATSubtarget &sti, unsigned Alignment)
    : TargetFrameLowering(StackGrowsDown, Alignment, 0, Alignment),
      STI(sti) {
  }

  static const MiniATFrameLowering *create(const MiniATSubtarget &ST);

  bool hasFP(const MachineFunction &MF) const override;

};

/// Create MiniATFrameLowering objects.
const MiniATFrameLowering *createMiniATStandardFrameLowering(const MiniATSubtarget &ST);

} // End llvm namespace

#endif