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

#ifndef MINIATSTANDARDISELLOWERING_H
#define MINIATSTANDARDISELLOWERING_H

#include "MiniATISelLowering.h"
#include "MiniATRegisterInfo.h"

namespace llvm {
  class MiniATStandardTargetLowering : public MiniATTargetLowering  {
  public:
    explicit MiniATStandardTargetLowering(MiniATTargetMachine &TM,
                                  const MiniATSubtarget &STI);
  private:
  };
}

#endif // MiniATISEISELLOWERING_H

