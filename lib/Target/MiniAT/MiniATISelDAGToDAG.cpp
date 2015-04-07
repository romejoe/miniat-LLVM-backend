//===-- MiniATISelDAGToDAG.cpp - A Dag to Dag Inst Selector for MiniAT --------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an instruction selector for the MINIAT target.
//
//===----------------------------------------------------------------------===//

#include "MiniATISelDAGToDAG.h"
#include "MiniAT.h"

#include "MiniATMachineFunction.h"
#include "MiniATRegisterInfo.h"
#include "MiniATStandardISelDAGToDAG.h"
#include "MiniATSubtarget.h"
#include "MiniATTargetMachine.h"
#include "MCTargetDesc/MiniATBaseInfo.h"
#include "llvm/IR/CFG.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Intrinsics.h"
#include "llvm/IR/Type.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/CodeGen/SelectionDAGNodes.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "miniat-isel"

//===----------------------------------------------------------------------===//
// Instruction Selector Implementation
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// MiniATDAGToDAGISel - MINIAT specific code to select MINIAT machine
// instructions for SelectionDAG operations.
//===----------------------------------------------------------------------===//

bool MiniATDAGToDAGISel::runOnMachineFunction(MachineFunction &MF) {
    bool Ret = SelectionDAGISel::runOnMachineFunction(MF);

    return Ret;
}

//@SelectAddr {
/// ComplexPattern used on MiniATInstrInfo
/// Used on MiniAT Load/Store instructions
bool MiniATDAGToDAGISel::
SelectAddr(SDNode *Parent, SDValue Addr, SDValue &Base, SDValue &Offset) {
    EVT ValTy = Addr.getValueType();

    // If Parent is an unaligned f32 load or store, select a (base + index)
    // floating point load/store instruction (luxc1 or suxc1).
    const LSBaseSDNode* LS = 0;

    if (Parent && (LS = dyn_cast<LSBaseSDNode>(Parent))) {
        EVT VT = LS->getMemoryVT();

        if (VT.getSizeInBits() / 8 > LS->getAlignment()) {
            assert("Unaligned loads/stores not supported for this type.");
            if (VT == MVT::f32)
                return false;
        }
    }

    // if Address is FI, get the TargetFrameIndex.
    if (FrameIndexSDNode *FIN = dyn_cast<FrameIndexSDNode>(Addr)) {
        Base   = CurDAG->getTargetFrameIndex(FIN->getIndex(), ValTy);
        Offset = CurDAG->getTargetConstant(0, ValTy);
        return true;
    }

    Base   = Addr;
    Offset = CurDAG->getTargetConstant(0, ValTy);
    return true;
}

/// Select instructions not customized! Used for
/// expanded, promoted and normal instructions
SDNode* MiniATDAGToDAGISel::Select(SDNode *Node) {
    unsigned Opcode = Node->getOpcode();

    // Dump information about the Node being selected
    DEBUG(errs() << "Selecting: "; Node->dump(CurDAG); errs() << "\n");

    // If we have a custom node, we already have selected!
    if (Node->isMachineOpcode()) {
        DEBUG(errs() << "== "; Node->dump(CurDAG); errs() << "\n");    Node->setNodeId(-1);
        return nullptr;
    }

    // See if subclasses can handle this node.
    std::pair<bool, SDNode*> Ret = selectNode(Node);

    if (Ret.first)
        return Ret.second;

    // Select the default instruction
    SDNode *ResNode = SelectCode(Node);

    DEBUG(errs() << "=> ");
    if (ResNode == NULL || ResNode == Node)
        DEBUG(Node->dump(CurDAG));
    else
        DEBUG(ResNode->dump(CurDAG));
    DEBUG(errs() << "\n");
    return ResNode;
}

/// createMiniATISelDag - This pass converts a legalized DAG into a
/// MINIAT-specific DAG, ready for instruction scheduling.
FunctionPass *llvm::createMiniATISelDag(MiniATTargetMachine &TM) {
    return llvm::createMiniATStandardISelDag(TM);
}