//===-- MiniATStandardISelDAGToDAG.cpp - A Dag to Dag Inst Selector for MiniATStandard ----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of MiniATDAGToDAGISel specialized for mips32/64.
//
//===----------------------------------------------------------------------===//

#include "MiniATStandardISelDAGToDAG.h"

#include "MCTargetDesc/MiniATBaseInfo.h"
#include "MiniAT.h"
#include "MiniATMachineFunction.h"
#include "MiniATRegisterInfo.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
using namespace llvm;

#define DEBUG_TYPE "miniat-isel"

bool MiniATStandardDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
    Subtarget = &TM.getSubtarget<MiniATSubtarget>();
    return MiniATDAGToDAGISel::runOnMachineFunction(MF);
}

void MiniATStandardDAGToDAGISel::processFunctionAfterISel(MachineFunction &MF) {
}

//@selectNode
std::pair<bool, SDNode*> MiniATStandardDAGToDAGISel::selectNode(SDNode *Node) {
    unsigned Opcode = Node->getOpcode();
    SDLoc DL(Node);

    ///
    // Instruction Selection not handled by the auto-generated
    // tablegen selection should be handled here.
    ///
    SDNode *Result;

    ///
    // Instruction Selection not handled by the auto-generated
    // tablegen selection should be handled here.
    ///
    EVT NodeTy = Node->getValueType(0);
    unsigned MultOpc;

    switch(Opcode) {
        default: break;

    }

    return std::make_pair(false, nullptr);
}

FunctionPass *llvm::createMiniATStandardISelDag(MiniATTargetMachine &TM) {
    return new MiniATStandardDAGToDAGISel(TM);
}