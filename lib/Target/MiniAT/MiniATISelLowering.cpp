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
//#include "MiniATTargetObjectFile.h"
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
        //      case MiniATISD::TRet:               return "MiniATISD::TRet";
        //case MiniATISD::DivRem:            return "MiniATISD::DivRem";
        //case MiniATISD::DivRemU:           return "MiniATISD::DivRemU";
        //case MiniATISD::Wrapper:           return "MiniATISD::Wrapper";
        case MiniATISD::PRet:               return "MiniATISD::PRet";
        case MiniATISD::NOP:               return "MiniATISD::NOP";
        default:
            return NULL;
    };
}

//@MiniATTargetLowering {
MiniATTargetLowering::MiniATTargetLowering(
        MiniATTargetMachine &TM
        , const MiniATSubtarget &STI
)
        : TargetLowering(TM), Subtarget(STI) {

}

const MiniATTargetLowering *MiniATTargetLowering::create(
        MiniATTargetMachine &TM
        , const MiniATSubtarget &STI
) {
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
MiniATTargetLowering::LowerFormalArguments(
        SDValue Chain
        , CallingConv::ID CallConv
        , bool IsVarArg
        , const SmallVectorImpl<ISD::InputArg> &Ins
        , SDLoc DL
        , SelectionDAG &DAG
        , SmallVectorImpl<SDValue> &InVals
)
const {
    switch (CallConv) {
        default:
            llvm_unreachable("Unsupported calling convention");
        case CallingConv::C:
        //case CallingConv::Fast:
            return LowerCCCArguments(Chain, CallConv, IsVarArg,
                                     Ins, DL, DAG, InVals
            );
    }
}
// @LowerFormalArguments }

namespace {
    struct ArgDataPair { SDValue SDV; ISD::ArgFlagsTy Flags; };
}

SDValue
MiniATTargetLowering::LowerCCCArguments(
        SDValue Chain
        , CallingConv::ID CallConv
        , bool isVarArg
        , const SmallVectorImpl<ISD::InputArg> &Ins
        , SDLoc dl
        , SelectionDAG &DAG
        , SmallVectorImpl<SDValue> &InVals
) const {
    return Chain;
    /*MachineFunction &MF = DAG.getMachineFunction();
    MachineFrameInfo *MFI = MF.getFrameInfo();
    MachineRegisterInfo &RegInfo = MF.getRegInfo();
    MiniATFunctionInfo *XFI = MF.getInfo<MiniATFunctionInfo>();

    // Assign locations to all of the incoming arguments.
    SmallVector<CCValAssign, 16> ArgLocs;
    CCState CCInfo(CallConv, isVarArg, DAG.getMachineFunction(), ArgLocs,
                   *DAG.getContext());

    CCInfo.AnalyzeFormalArguments(Ins, CC_MiniAT);

    unsigned StackSlotSize = MiniATFrameLowering::stackSlotSize();

    unsigned LRSaveSize = StackSlotSize;

    if (!isVarArg) {
        XFI->setReturnStackOffset(CCInfo.getNextStackOffset() + LRSaveSize);
    }

    // All getCopyFromReg ops must precede any getMemcpys to prevent the
    // scheduler clobbering a register before it has been copied.
    // The stages are:
    // 1. CopyFromReg (and load) arg & vararg registers.
    // 2. Chain CopyFromReg nodes into a TokenFactor.
    // 3. Memcpy 'byVal' args & push final InVals.
    // 4. Chain mem ops nodes into a TokenFactor.
    SmallVector<SDValue, 4> CFRegNode;
    SmallVector<ArgDataPair, 4> ArgData;
    SmallVector<SDValue, 4> MemOps;

    // 1a. CopyFromReg (and load) arg registers.
    for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {

        CCValAssign &VA = ArgLocs[i];
        SDValue ArgIn;

        if (VA.isRegLoc()) {
            // Arguments passed in registers
            EVT RegVT = VA.getLocVT();
            switch (RegVT.getSimpleVT().SimpleTy) {
                default: {
#ifndef NDEBUG
                    errs() << "LowerFormalArguments Unhandled argument type: "
                    << RegVT.getSimpleVT().SimpleTy << "\n";
#endif
                    llvm_unreachable(nullptr);
                }
                case MVT::i32:
                    unsigned VReg = RegInfo.createVirtualRegister(&MiniAT::GPRRegClass);
                    RegInfo.addLiveIn(VA.getLocReg(), VReg);
                    ArgIn = DAG.getCopyFromReg(Chain, dl, VReg, RegVT);
                    CFRegNode.push_back(ArgIn.getValue(ArgIn->getNumValues() - 1));
            }
        }
        else {
            // sanity check
            assert(VA.isMemLoc());
            // Load the argument to a virtual register
            unsigned ObjSize = VA.getLocVT().getSizeInBits() / 8;
            if (ObjSize > StackSlotSize) {
                errs() << "LowerFormalArguments Unhandled argument type: "
                << EVT(VA.getLocVT()).getEVTString()
                << "\n";
            }
            // Create the frame index object for this incoming parameter...
            int FI = MFI->CreateFixedObject(ObjSize,
                                            LRSaveSize + VA.getLocMemOffset(),
                                            true
            );

            // Create the SelectionDAG nodes corresponding to a load
            //from this parameter
            SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
            ArgIn = DAG.getLoad(VA.getLocVT(), dl, Chain, FIN,
                                MachinePointerInfo::getFixedStack(FI),
                                false, false, false, 0
            );
        }
        const ArgDataPair ADP = {ArgIn, Ins[i].Flags};
        ArgData.push_back(ADP);
    }

    // 1b. CopyFromReg vararg registers.

    /*
    if (isVarArg) {
        // Argument registers
        static const MCPhysReg ArgRegs[] = {
                MiniAT::R0, MiniAT::R1, MiniAT::R2, MiniAT::R3
        };
        MiniATFunctionInfo *XFI = MF.getInfo<MiniATFunctionInfo>();
        unsigned FirstVAReg = CCInfo.getFirstUnallocated(ArgRegs,
                                                         array_lengthof(ArgRegs));
        if (FirstVAReg < array_lengthof(ArgRegs)) {
            int offset = 0;
            // Save remaining registers, storing higher register numbers at a higher
            // address
            for (int i = array_lengthof(ArgRegs) - 1; i >= (int) FirstVAReg; --i) {
                // Create a stack slot
                int FI = MFI->CreateFixedObject(4, offset, true);
                if (i == (int) FirstVAReg) {
                    XFI->setVarArgsFrameIndex(FI);
                }
                offset -= StackSlotSize;
                SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
                // Move argument from phys reg -> virt reg
                unsigned VReg = RegInfo.createVirtualRegister(&MiniAT::GPRRegClass);
                RegInfo.addLiveIn(ArgRegs[i], VReg);
                SDValue Val = DAG.getCopyFromReg(Chain, dl, VReg, MVT::i32);
                CFRegNode.push_back(Val.getValue(Val->getNumValues() - 1));
                // Move argument from virt reg -> stack
                SDValue Store = DAG.getStore(Val.getValue(1), dl, Val, FIN,
                                             MachinePointerInfo(), false, false, 0
                );
                MemOps.push_back(Store);
            }
        }
        else {
            // This will point to the next argument passed via stack.
            XFI->setVarArgsFrameIndex(
                    MFI->CreateFixedObject(4, LRSaveSize + CCInfo.getNextStackOffset(),
                                           true
                    ));
        }
    }

    // 2. chain CopyFromReg nodes into a TokenFactor.
    if (!CFRegNode.empty()) {
        Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, CFRegNode);
    }

    // 3. Memcpy 'byVal' args & push final InVals.
    // Aggregates passed "byVal" need to be copied by the callee.
    // The callee will use a pointer to this copy, rather than the original
    // pointer.
    for (SmallVectorImpl<ArgDataPair>::const_iterator ArgDI = ArgData.begin(),
                 ArgDE = ArgData.end();
         ArgDI != ArgDE; ++ArgDI) {
        if (ArgDI->Flags.isByVal() && ArgDI->Flags.getByValSize()) {
            unsigned Size = ArgDI->Flags.getByValSize();
            unsigned Align = std::max(StackSlotSize, ArgDI->Flags.getByValAlign());
            // Create a new object on the stack and copy the pointee into it.
            int FI = MFI->CreateStackObject(Size, Align, false);
            SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
            InVals.push_back(FIN);
            MemOps.push_back(DAG.getMemcpy(Chain, dl, FIN, ArgDI->SDV,
                                           DAG.getConstant(Size, MVT::i32),
                                           Align, false, false,
                                           MachinePointerInfo(),
                                           MachinePointerInfo()));
        }
        else {
            InVals.push_back(ArgDI->SDV);
        }
    }

    // 4, chain mem ops nodes into a TokenFactor.
    if (!MemOps.empty()) {
        MemOps.push_back(Chain);
        Chain = DAG.getNode(ISD::TokenFactor, dl, MVT::Other, MemOps);
    }

    return Chain;*/
}


//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

/*bool
MiniATTargetLowering::CanLowerReturn(
        CallingConv::ID CallConv
        , MachineFunction &MF
        , bool IsVarArg
        , const SmallVectorImpl<ISD::OutputArg> &Outs
        , LLVMContext &Context
) const {
    SmallVector<CCValAssign, 16> RVLocs;
    CCState CCInfo(CallConv, IsVarArg, MF,
                   RVLocs, Context
    );
    return CCInfo.CheckReturn(Outs, RetCC_MiniAT);
}*/


SDValue MiniATTargetLowering::LowerReturn(
        SDValue Chain
        , CallingConv::ID CallConv
        , bool IsVarArg
        , const SmallVectorImpl<ISD::OutputArg> &Outs
        , const SmallVectorImpl<SDValue> &OutVals
        , SDLoc DL
        , SelectionDAG &DAG
) const {

    MiniATFunctionInfo *XFI =
            DAG.getMachineFunction().getInfo<MiniATFunctionInfo>();
    MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();

    // CCValAssign - represent the assignment of
    // the return value to a location
    SmallVector<CCValAssign, 16> RVLocs;

    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                   *DAG.getContext());

    // Analyze return values.
    if (!IsVarArg) {
        CCInfo.AllocateStack(XFI->getReturnStackOffset(), 4);
    }

    CCInfo.AnalyzeReturn(Outs, RetCC_MiniAT);

    SDValue Flag;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    // Return on MiniAT is always a "retsp 0"
    RetOps.push_back(DAG.getConstant(0, MVT::i32));

    SmallVector<SDValue, 4> MemOpChains;
    // Handle return values that must be copied to memory.
    for (unsigned i = 0, e = RVLocs.size(); i != e; ++i) {
        CCValAssign &VA = RVLocs[i];
        if (VA.isRegLoc()) {
            continue;
        }
        assert(VA.isMemLoc());
        if (IsVarArg) {
            report_fatal_error("Can't return value from vararg function in memory");
        }

        int Offset = VA.getLocMemOffset();
        unsigned ObjSize = VA.getLocVT().getSizeInBits() / 8;
        // Create the frame index object for the memory location.
        int FI = MFI->CreateFixedObject(ObjSize, Offset, false);

        // Create a SelectionDAG node corresponding to a store
        // to this memory location.
        SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
        MemOpChains.push_back(DAG.getStore(Chain, DL, OutVals[i], FIN,
                                           MachinePointerInfo::getFixedStack(FI), false, false,
                                           0
        ));
    }

    // Transform all store nodes into one single node because
    // all stores are independent of each other.
    if (!MemOpChains.empty()) {
        Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);
    }

    // Now handle return values copied to registers.
    for (unsigned i = 0, e = RVLocs.size(); i != e; ++i) {
        CCValAssign &VA = RVLocs[i];
        if (!VA.isRegLoc()) {
            continue;
        }
        // Copy the result values into the output registers.
        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), OutVals[i], Flag);

        // guarantee that all emitted copies are
        // stuck together, avoiding something bad
        Flag = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

    RetOps[0] = Chain;  // Update chain.

    // Add the flag if we have it.
    if (Flag.getNode()) {
        RetOps.push_back(Flag);
    }


    //return DAG.getNode(MiniAT::ADDRRI, DL, MVT::Other, RetOps);
    return DAG.getNode(MiniATISD::PRet, DL, MVT::Other, RetOps);
    //return DAG.getNode(MiniATISD::TRet, DL, MVT::Other, RetOps);
}