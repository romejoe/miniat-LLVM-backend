//===-- MiniATSEInstrInfo.h - MiniAT32/64 Instruction Information ---*- C++ -*-===//
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

#ifndef MINIATSEINSTRUCTIONINFO_H
#define MINIATSEINSTRUCTIONINFO_H

#include "MiniATInstrInfo.h"
#include "MiniATSERegisterInfo.h"
#include "MiniATMachineFunction.h"

namespace llvm {

class MiniATSEInstrInfo : public MiniATInstrInfo {
  const MiniATSERegisterInfo RI;

public:
  explicit MiniATSEInstrInfo(const MiniATSubtarget &STI);

  const MiniATRegisterInfo &getRegisterInfo() const override;

};

}

#endif

