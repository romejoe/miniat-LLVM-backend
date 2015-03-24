//===-- MiniATMCAsmInfo.h - MiniAT asm properties ----------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the MiniATMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATTARGETASMINFO_H
#define MINIATTARGETASMINFO_H

#include "llvm/MC/MCAsmInfo.h"

namespace llvm {
  class StringRef;
  class Target;

  class MiniATMCAsmInfo : public MCAsmInfo { 
    void anchor();
  public:
    explicit MiniATMCAsmInfo(StringRef TT);
  };

} // namespace llvm

#endif
