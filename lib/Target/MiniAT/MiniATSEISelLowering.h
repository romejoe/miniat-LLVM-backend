//===-- MiniATISEISelLowering.h - MiniATISE DAG Lowering Interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of MiniATITargetLowering specialized for miniat32/64.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSEISELLOWERING_H
#define MINIATSEISELLOWERING_H

#include "MiniATISelLowering.h"
#include "MiniATRegisterInfo.h"

namespace llvm {
  class MiniATSETargetLowering : public MiniATTargetLowering  {
  public:
    explicit MiniATSETargetLowering(MiniATTargetMachine &TM,
                                  const MiniATSubtarget &STI);
  private:
  };
}

#endif // MiniATISEISELLOWERING_H

