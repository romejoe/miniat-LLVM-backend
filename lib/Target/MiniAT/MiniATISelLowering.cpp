//===-- MiniATISelLowering.cpp - MiniAT DAG Lowering Implementation -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that MiniAT uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//
#include "MiniATISelLowering.h"

#include "MiniATMachineFunction.h"
#include "MiniATTargetMachine.h"
#include "MiniATTargetObjectFile.h"
#include "MiniATSubtarget.h"
//#include "llvm/ADT/Statistic.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/ValueTypes.h"
#include "llvm/IR/CallingConv.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-lower"

#if 0
SDValue MiniATTargetLowering::getTargetNode(ConstantPoolSDNode *N, EVT Ty,
                                          SelectionDAG &DAG,
                                          unsigned Flag) const {
  return DAG.getTargetConstantPool(N->getConstVal(), Ty, N->getAlignment(),
                                   N->getOffset(), Flag);
}
#endif

const char *MiniATTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  //case MiniATISD::JmpLink:           return "MiniATISD::JmpLink";
  //case MiniATISD::TailCall:          return "MiniATISD::TailCall";
  //case MiniATISD::Hi:                return "MiniATISD::Hi";
  //case MiniATISD::Lo:                return "MiniATISD::Lo";
  //case MiniATISD::GPRel:             return "MiniATISD::GPRel";
  //case MiniATISD::Ret:               return "MiniATISD::Ret";
  //case MiniATISD::DivRem:            return "MiniATISD::DivRem";
  //case MiniATISD::DivRemU:           return "MiniATISD::DivRemU";
  //case MiniATISD::Wrapper:           return "MiniATISD::Wrapper";
  default:                         return NULL;
  }
}

//@MiniATTargetLowering {
MiniATTargetLowering::MiniATTargetLowering(MiniATTargetMachine &TM,
                                       const MiniATSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {

}

const MiniATTargetLowering *MiniATTargetLowering::create(MiniATTargetMachine &TM,
                                                     const MiniATSubtarget &STI) {
  return llvm::createMiniATSETargetLowering(TM, STI);
}

//===----------------------------------------------------------------------===//
//  Lower helper functions
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
//  Misc Lower Operation implementation
//===----------------------------------------------------------------------===//

#include "MiniATGenCallingConv.inc"

//===----------------------------------------------------------------------===//
//             Formal Arguments Calling Convention Implementation
//===----------------------------------------------------------------------===//

//@LowerFormalArguments {
/// LowerFormalArguments - transform physical registers into virtual registers
/// and generate load operations for arguments places on the stack.
SDValue
MiniATTargetLowering::LowerFormalArguments(SDValue Chain,
                                         CallingConv::ID CallConv,
                                         bool IsVarArg,
                                      const SmallVectorImpl<ISD::InputArg> &Ins,
                                         SDLoc DL, SelectionDAG &DAG,
                                         SmallVectorImpl<SDValue> &InVals)
                                          const {

  return Chain;
}
// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue
MiniATTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                SDLoc DL, SelectionDAG &DAG) const {
  return DAG.getNode(MiniATISD::Ret, DL, MVT::Other,
                     Chain, DAG.getRegister(MiniAT::LR, MVT::i32));
}

