//===-- MiniAT.h - Top-level interface for MiniAT representation --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in the LLVM
// MiniAT back-end.
//
//===----------------------------------------------------------------------===//

#ifndef TARGET_MINIAT_H
#define TARGET_MINIAT_H

#include "MCTargetDesc/MiniATMCTargetDesc.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
  class MiniATTargetMachine;
  class FunctionPass;
  
  class ModulePass;
  class TargetMachine;
  class formatted_raw_ostream;

  FunctionPass *createMiniATISelDag(MiniATTargetMachine &TM,
                                   CodeGenOpt::Level OptLevel);


  void initializeMiniATLowerThreadLocalPass(PassRegistry &p);

  FunctionPass *createMiniATFrameToArgsOffsetEliminationPass();
  
  //ModulePass *createMiniATLowerThreadLocalPass();

  ImmutablePass *createMiniATTargetTransformInfoPass(const MiniATTargetMachine *TM);

} // end namespace llvm;

#endif
