//===-- MiniATFrameToArgsOffsetElim.cpp ----------------------------*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Replace Pseudo FRAME_TO_ARGS_OFFSET with the appropriate real offset.
//
//===----------------------------------------------------------------------===//

#include "MiniAT.h"
#include "MiniATInstrInfo.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

namespace {
  struct MiniATFTAOElim : public MachineFunctionPass {
    static char ID;
    MiniATFTAOElim() : MachineFunctionPass(ID) {}

    bool runOnMachineFunction(MachineFunction &Fn) override;

    const char *getPassName() const override {
      return "MiniAT FRAME_TO_ARGS_OFFSET Elimination";
    }
  };
  char MiniATFTAOElim::ID = 0;
}

/// createMiniATFrameToArgsOffsetEliminationPass - returns an instance of the
/// Frame to args offset elimination pass
FunctionPass *llvm::createMiniATFrameToArgsOffsetEliminationPass() {
  return new MiniATFTAOElim();
}

bool MiniATFTAOElim::runOnMachineFunction(MachineFunction &MF) {
  const MiniATInstrInfo &TII =
          *static_cast<const MiniATInstrInfo*>(MF.getTarget().getInstrInfo());
  unsigned StackSize = MF.getFrameInfo()->getStackSize();
  for (MachineFunction::iterator MFI = MF.begin(), E = MF.end(); MFI != E;
       ++MFI) {
    MachineBasicBlock &MBB = *MFI;
    for (MachineBasicBlock::iterator MBBI = MBB.begin(), EE = MBB.end();
         MBBI != EE; ++MBBI) {
      if (MBBI->getOpcode() == MiniAT::FRAME_TO_ARGS_OFFSET) {
        MachineInstr *OldInst = MBBI;
        unsigned Reg = OldInst->getOperand(0).getReg();
        MBBI = TII.loadImmediate(MBB, MBBI, Reg, StackSize);
        OldInst->eraseFromParent();
      }
    }
  }
  return true;
}
