//===-- XCoreTargetInfo.cpp - XCore Target Implementation -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MiniAT.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target MiniATTarget;
extern "C" void LLVMInitializeMiniATTargetInfo() {
  // Register the target.
  RegisterTarget<Triple::MiniAT, false> X(MiniATTarget, "MiniAT", "MiniAT");
}