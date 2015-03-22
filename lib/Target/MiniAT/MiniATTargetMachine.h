//===-- MiniATTargetMachine.h - Define TargetMachine for MiniAT ---*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MiniAT specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//
//

#ifndef MINIATTARGETMACHINE_H
#define MINIATTARGETMACHINE_H

#include "llvm/Target/TargetMachine.h"

namespace llvm {

  class Module;

  class MiniATTargetMachine : public LLVMTargetMachine{
/*
  public:
  		MiniATTargetMachine(const Target &T, StringRef TT,
                     StringRef CPU, StringRef FS, const TargetOptions &Options,
                     Reloc::Model RM, CodeModel::Model CM,
                     CodeGenOpt::Level OL);

  const MiniATInstrInfo *getInstrInfo() const override {
    return getSubtargetImpl()->getInstrInfo();
  }
  const MiniATFrameLowering *getFrameLowering() const override {
    return getSubtargetImpl()->getFrameLowering();
  }
  const MiniATSubtarget *getSubtargetImpl() const override { return &Subtarget; }
  const MiniATTargetLowering *getTargetLowering() const override {
    return getSubtargetImpl()->getTargetLowering();
  }
  const MiniATSelectionDAGInfo* getSelectionDAGInfo() const override {
    return getSubtargetImpl()->getSelectionDAGInfo();
  }
  const TargetRegisterInfo *getRegisterInfo() const override {
    return getSubtargetImpl()->getRegisterInfo();
  }
  const DataLayout *getDataLayout() const override {
    return getSubtargetImpl()->getDataLayout();
  }

  // Pass Pipeline Configuration
  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  void addAnalysisPasses(PassManagerBase &PM) override;*/
    };

} // End llvm namespace

#endif