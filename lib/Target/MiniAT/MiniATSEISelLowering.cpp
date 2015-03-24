//===-- MipsSEISelLowering.cpp - MipsSE DAG Lowering Interface --*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of MipsTargetLowering specialized for mips32/64.
//
//===----------------------------------------------------------------------===//
#include "MiniATMachineFunction.h"
#include "MiniATSEISelLowering.h"

#include "MiniATRegisterInfo.h"
#include "MiniATTargetMachine.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetInstrInfo.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-isel"

//static cl::opt<bool>
//EnableMiniATTailCalls("enable-miniat-tail-calls", cl::Hidden,
//                    cl::desc("MINIAT: Enable tail calls."), cl::init(false));

MiniATSETargetLowering::MiniATSETargetLowering(MiniATTargetMachine &TM,
                                           const MiniATSubtarget &STI)
    : MiniATTargetLowering(TM, STI) {
  // Set up the register classes
  addRegisterClass(MVT::i32, &MiniAT::CPURegsRegClass);

  computeRegisterProperties();
}

const MiniATTargetLowering *
llvm::createMiniATSETargetLowering(MiniATTargetMachine &TM,
                                 const MiniATSubtarget &STI) {
  return new MiniATSETargetLowering(TM, STI);
}

