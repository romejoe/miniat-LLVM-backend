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
#include "llvm/ADT/Statistic.h"
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
  return createMiniATStandardTargetLowering(TM, STI);
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

    MachineFunction &MF = DAG.getMachineFunction();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    MiniATFunctionInfo *MiniATFI = MF.getInfo<MiniATFunctionInfo>();

    MiniATFI->setVarArgsFrameIndex(0);

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(),
                   ArgLocs, *DAG.getContext());
    MiniATCC MiniATCCInfo(CallConv, Subtarget.isABI_O32(),
                      CCInfo);
    MiniATFI->setFormalArgInfo(CCInfo.getNextStackOffset(),
                             MiniATCCInfo.hasByValArg());

    Function::const_arg_iterator FuncArg =
            DAG.getMachineFunction().getFunction()->arg_begin();
    bool UseSoftFloat = Subtarget.abiUsesSoftFloat();

    MiniATCCInfo.analyzeFormalArguments(Ins, UseSoftFloat, FuncArg);

    // Used with vargs to acumulate store chains.
    std::vector<SDValue> OutChains;

    unsigned CurArgIdx = 0;
    MiniATCC::byval_iterator ByValArg = MiniATCCInfo.byval_begin();

    //@2 {
    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        //@2 }
        CCValAssign &VA = ArgLocs[i];
        std::advance(FuncArg, Ins[i].OrigArgIndex - CurArgIdx);
        CurArgIdx = Ins[i].OrigArgIndex;
        EVT ValVT = VA.getValVT();
        ISD::ArgFlagsTy Flags = Ins[i].Flags;
        bool IsRegLoc = VA.isRegLoc();

        //@byval pass {
        if (Flags.isByVal()) {
            assert(Flags.getByValSize() &&
                   "ByVal args of size 0 should have been ignored by front-end.");
            assert(ByValArg != MiniATCCInfo.byval_end());
            copyByValRegs(Chain, DL, OutChains, DAG, Flags, InVals, &*FuncArg,
                          MiniATCCInfo, *ByValArg);
            ++ByValArg;
            continue;
        }
        //@byval pass }
        // Arguments stored on registers
        if (Subtarget.isABI_O32() && IsRegLoc) {
            MVT RegVT = VA.getLocVT();
            unsigned ArgReg = VA.getLocReg();
            const TargetRegisterClass *RC = getRegClassFor(RegVT);

            // Transform the arguments stored on
            // physical registers into virtual ones
            unsigned Reg = addLiveIn(DAG.getMachineFunction(), ArgReg, RC);
            SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Reg, RegVT);

            // If this is an 8 or 16-bit value, it has been passed promoted
            // to 32 bits.  Insert an assert[sz]ext to capture this, then
            // truncate to the right size.
            if (VA.getLocInfo() != CCValAssign::Full) {
                unsigned Opcode = 0;
                if (VA.getLocInfo() == CCValAssign::SExt)
                    Opcode = ISD::AssertSext;
                else if (VA.getLocInfo() == CCValAssign::ZExt)
                    Opcode = ISD::AssertZext;
                if (Opcode)
                    ArgValue = DAG.getNode(Opcode, DL, RegVT, ArgValue,
                                           DAG.getValueType(ValVT));
                ArgValue = DAG.getNode(ISD::TRUNCATE, DL, ValVT, ArgValue);
            }

            // Handle floating point arguments passed in integer registers.
            if ((RegVT == MVT::i32 && ValVT == MVT::f32) ||
                (RegVT == MVT::i64 && ValVT == MVT::f64))
                ArgValue = DAG.getNode(ISD::BITCAST, DL, ValVT, ArgValue);
            InVals.push_back(ArgValue);
        } else { // VA.isRegLoc()

            // sanity check
            assert(VA.isMemLoc());

            // The stack pointer offset is relative to the caller stack frame.
            int FI = MFI->CreateFixedObject(ValVT.getSizeInBits()/8,
                                            VA.getLocMemOffset(), true);

            // Create load nodes to retrieve arguments from the stack
            SDValue FIN = DAG.getFrameIndex(FI, getPointerTy());
            SDValue Load = DAG.getLoad(ValVT, DL, Chain, FIN,
                                       MachinePointerInfo::getFixedStack(FI),
                                       false, false, false, 0);
            InVals.push_back(Load);
            OutChains.push_back(Load.getValue(1));
        }
    }

//@Ordinary struct type: 1 {
    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
        // The MiniAT ABIs for returning structs by value requires that we copy
        // the sret argument into $v0 for the return. Save the argument into
        // a virtual register so that we can access it from the return points.
        if (Ins[i].Flags.isSRet()) {
            unsigned Reg = MiniATFI->getSRetReturnReg();
            if (!Reg) {
                Reg = MF.getRegInfo().createVirtualRegister(
                        getRegClassFor(MVT::i32));
                MiniATFI->setSRetReturnReg(Reg);
            }
            SDValue Copy = DAG.getCopyToReg(DAG.getEntryNode(), DL, Reg, InVals[i]);
            Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, Copy, Chain);
            break;
        }
    }
//@Ordinary struct type: 1 }

    // All stores are grouped in one node to allow the matching between
    // the size of Ins and InVals. This only happens when on varg functions
    if (!OutChains.empty()) {
        OutChains.push_back(Chain);
        Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
    }

    return Chain;
}
    switch (CallConv)
    {
        default:
            llvm_unreachable("Unsupported calling convention");
        case CallingConv::C:
            return LowerCCCArguments(Chain, CallConv, isVarArg,
                                     Ins, dl, DAG, InVals);
    }
}



// @LowerFormalArguments }

//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

SDValue MiniATTargetLowering::LowerReturn(SDValue Chain,
                                CallingConv::ID CallConv, bool IsVarArg,
                                const SmallVectorImpl<ISD::OutputArg> &Outs,
                                const SmallVectorImpl<SDValue> &OutVals,
                                SDLoc DL, SelectionDAG &DAG) const {
    //TODO: implement

    return DAG.getNode(0, DL, MVT::Other,
                     Chain, DAG.getRegister(0, MVT::i32));
}

