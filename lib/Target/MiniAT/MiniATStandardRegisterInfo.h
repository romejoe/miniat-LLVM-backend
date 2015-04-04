//===-- MiniATStandardRegisterInfo.h - MiniAT32 Register Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT32/64 implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSTANDARDREGISTERINFO_H
#define MINIATSTANDARDREGISTERINFO_H

#include "MiniATRegisterInfo.h"

namespace llvm {
class MiniATStandardInstrInfo;

class MiniATStandardRegisterInfo : public MiniATRegisterInfo {
public:
  MiniATStandardRegisterInfo(const MiniATSubtarget &Subtarget);

  const TargetRegisterClass *intRegClass(unsigned Size) const override;
};

} // end namespace llvm

#endif

