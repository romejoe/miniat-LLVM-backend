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

Target llvm::TheMiniATTarget;

extern "C" void LLVMInitializeMiniATTargetInfo() {
  // Register the target.
  RegisterTarget<Triple::miniat, /*HasJIT=*/false>
  	X(TheMiniATTarget, "miniat", "MiniAT Target");
}