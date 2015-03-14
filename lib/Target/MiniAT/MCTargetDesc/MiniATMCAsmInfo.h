//===-- MiniATMCAsmInfo.h - XCore asm properties ----------------*- C++ -*--===//
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

#ifndef XCORETARGETASMINFO_H
#define XCORETARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
  class StringRef;
  class Target;

  class MiniATMCAsmInfo : public MCAsmInfoELF { //probably don't need the ELF
    void anchor() override;
  public:
    explicit MiniATMCAsmInfo(StringRef TT);
  };

} // namespace llvm

#endif
