//===-- llvm/Target/MiniATTargetObjectFile.h - MiniAT Object Info ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TARGET_MINIAT_TARGETOBJECTFILE_H
#define LLVM_TARGET_MINIAT_TARGETOBJECTFILE_H

#include <llvm/CodeGen/TargetLoweringObjectFileImpl.h>

namespace llvm {

  class MiniATTargetObjectFile : public TargetLoweringObjectFileELF {
    const MCSection *SmallDataSection;
    const MCSection *SmallBSSSection;
    const TargetMachine *TM;
  public:



    void Initialize(MCContext &Ctx, const TargetMachine &TM) override;

  };
} // end namespace llvm

#endif