//===-- MiniATMachineFunctionInfo.cpp - Private data used for MiniAT ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MiniATMachineFunction.h"

#include "MiniATInstrInfo.h"
#include "MiniATSubtarget.h"
#include "llvm/IR/Function.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

using namespace llvm;

bool FixGlobalBaseReg;

// class MiniATCallEntry.
MiniATCallEntry::MiniATCallEntry(const StringRef &N) {
#ifndef NDEBUG
  Name = N;
  Val = nullptr;
#endif
}

MiniATCallEntry::MiniATCallEntry(const GlobalValue *V) {
#ifndef NDEBUG
  Val = V;
#endif
}

bool MiniATCallEntry::isConstant(const MachineFrameInfo *) const {
  return false;
}

bool MiniATCallEntry::isAliased(const MachineFrameInfo *) const {
  return false;
}

bool MiniATCallEntry::mayAlias(const MachineFrameInfo *) const {
  return false;
}

void MiniATCallEntry::printCustom(raw_ostream &O) const {
  O << "MiniATCallEntry: ";
#ifndef NDEBUG
  if (Val)
    O << Val->getName();
  else
    O << Name;
#endif
}

MiniATFunctionInfo::~MiniATFunctionInfo() {
  for (StringMap<const MiniATCallEntry *>::iterator
       I = ExternalCallEntries.begin(), E = ExternalCallEntries.end(); I != E;
       ++I)
    delete I->getValue();

  for (const auto &Entry : GlobalCallEntries)
    delete Entry.second;
}

void MiniATFunctionInfo::anchor() { }

void MiniATFunctionInfo::setReturnStackOffset(unsigned int i) {

}

unsigned int MiniATFunctionInfo::getReturnStackOffset() {
  return 0;
}
