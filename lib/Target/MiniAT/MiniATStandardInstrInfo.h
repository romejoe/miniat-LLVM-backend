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

};

}

#endif

