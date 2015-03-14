//===-- MiniATTargetMachine.cpp - Define TargetMachine for the X86 -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the X86 specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#include "MiniATTargetMachine.h"
#include "MiniAT.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/PassManager.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;





void MiniATTargetMachine::anchor() { }

/// MiniATTargetMachine ctor - Create an MiniAT target.
///
MiniATTargetMachine::MiniATTargetMachine(
  const Target &T
  , StringRef TT
  , StringRef CPU
  , StringRef FS
  , const TargetOptions &Options
  , Reloc::Model RM
  , CodeModel::Model CM
  , CodeGenOpt::Level OL
)
  : LLVMTargetMachine(
      T
      , TT
      , CPU
      , FS
      , Options
      , RM
      , CM
      , OL
    )/*,
  Subtarget(
    TT
    , CPU
    , FS
    , *this
    , Options.StackAlignmentOverride
  )*/ {
  initAsmInfo();
}


void MiniATTargetMachine::addAnalysisPasses(PassManagerBase &PM) {
  // Add first the target-independent BasicTTI pass, then our miniat pass. This
  // allows the X86 pass to delegate to the target independent layer when
  // appropriate.
  PM.add(createBasicTargetTransformInfoPass(this));
  PM.add(createMiniATTargetTransformInfoPass(this));

  //TODO
}

namespace {
  class MiniATPassConfig : public TargetPassConfig {
  public:
    MiniATPassConfig(MiniATTargetMachine *TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

    MiniATTargetMachine &getMiniATTargetMachine() const {
      return getTM<MiniATTargetMachine>();
    }

    bool addInstSelector() override;
    bool addPreEmitPass() override;
  };
} // namespace

TargetPassConfig *MiniATTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new MiniATPassConfig(this, PM);
}

bool MiniATPassConfig::addInstSelector() {
  addPass(createMiniATISelDag(getMiniATTargetMachine(), getOptLevel()));
  return false;
}

bool MiniATPassConfig::addPreEmitPass() {
  addPass(createMiniATFrameToArgsOffsetEliminationPass());
  return false;
}

extern "C" void LLVMInitializeMiniATAsmPrinter() {
  RegisterAsmPrinter<MiniATAsmPrinter> X(MiniATTarget);
}

extern "C" void LLVMInitializeMiniATTarget() {
  RegisterTargetMachine<MiniATTargetMachine> X(MiniATTarget);
}