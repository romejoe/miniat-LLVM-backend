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
#include <bitset>
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
        case MiniATISD::PCall:               return "MiniATISD::PCall";
        case MiniATISD::PRet:               return "MiniATISD::PRet";
        case MiniATISD::NOP:               return "MiniATISD::NOP";
        case MiniATISD::PCRelativeWrapper : return "MiniATISD::PCRelativeWrapper";
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

    addRegisterClass(MVT::i32, &MiniAT::SPRRegClass);
    addRegisterClass(MVT::i32, &MiniAT::GPRRegClass);

    computeRegisterProperties();
    setIntDivIsCheap(false);

    setBooleanContents(ZeroOrOneBooleanContent);

    setOperationAction(ISD::SETCC,MVT::i32, Expand);
    setOperationAction(ISD::SELECT_CC, MVT::i32,   Expand);


    setOperationAction(ISD::BlockAddress, MVT::i32 , Custom);

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
    //return Chain;
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
    //return Chain;
    //return SDValue();
    MachineFunction &MF = DAG.getMachineFunction();
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


    if (isVarArg) {
        // Argument registers
        static const MCPhysReg ArgRegs[] = {
                MiniAT::r2
                , MiniAT::r3
                , MiniAT::r4
                , MiniAT::r5
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

    return DAG.getNode(MiniATISD::PCall, dl, MVT::Other, Chain);
    //return Chain;
}


//===----------------------------------------------------------------------===//
//               Return Value Calling Convention Implementation
//===----------------------------------------------------------------------===//

bool
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
}


SDValue MiniATTargetLowering::LowerReturn(
        SDValue Chain
        , CallingConv::ID CallConv
        , bool IsVarArg
        , const SmallVectorImpl<ISD::OutputArg> &Outs
        , const SmallVectorImpl<SDValue> &OutVals
        , SDLoc DL
        , SelectionDAG &DAG
) const {

   /* MiniATFunctionInfo *XFI =
            DAG.getMachineFunction().getInfo<MiniATFunctionInfo>();

    MachineFrameInfo *MFI = DAG.getMachineFunction().getFrameInfo();

    // CCValAssign - represent the assignment of
    // the return value to a location
    SmallVector<CCValAssign, 16> RVLocs;
    MachineFunction &MF = DAG.getMachineFunction();


    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), RVLocs,
                   *DAG.getContext());

    MiniATCC MiniATCCInfo(CallConv, CCInfo);

    // Analyze return values.
    MiniATCCInfo.analyzeReturn(Outs, Subtarget.abiUsesSoftFloat(),
                             MF.getFunction()->getReturnType());

    // Analyze return values.
    if (!IsVarArg) {
        CCInfo.AllocateStack(XFI->getReturnStackOffset(), 4);
    }

    CCInfo.AnalyzeReturn(Outs, RetCC_MiniAT);

    SDValue Flag;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    // Return on MiniAT is always a "retsp 0"
    //RetOps.push_back(DAG.getConstant(0, MVT::i32));

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
*/
    // CCValAssign - represent the assignment of
    // the return value to a location
    SmallVector<CCValAssign, 16> RVLocs;
    MachineFunction &MF = DAG.getMachineFunction();

    // CCState - Info about the registers and stack slot.
    CCState CCInfo(CallConv, IsVarArg, MF, RVLocs,
                   *DAG.getContext());
    MiniATCC MiniATCCInfo(CallConv, CCInfo);

    // Analyze return values.
    MiniATCCInfo.analyzeReturn(Outs, Subtarget.abiUsesSoftFloat(),
                             MF.getFunction()->getReturnType());

    SDValue Flag;
    SmallVector<SDValue, 4> RetOps(1, Chain);

    // Copy the result values into the output registers.
    for (unsigned i = 0; i != RVLocs.size(); ++i) {
        SDValue Val = OutVals[i];
        CCValAssign &VA = RVLocs[i];
        assert(VA.isRegLoc() && "Can only return in registers!");

        if (RVLocs[i].getValVT() != RVLocs[i].getLocVT())
            Val = DAG.getNode(ISD::BITCAST, DL, RVLocs[i].getLocVT(), Val);

        Chain = DAG.getCopyToReg(Chain, DL, VA.getLocReg(), Val, Flag);

        // Guarantee that all emitted copies are stuck together with flags.
        Flag = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(VA.getLocReg(), VA.getLocVT()));
    }

//@Ordinary struct type: 2 {
    // The cpu0 ABIs for returning structs by value requires that we copy
    // the sret argument into $v0 for the return. We saved the argument into
    // a virtual register in the entry block, so now we copy the value out
    // and into $v0.
    /*if (MF.getFunction()->hasStructRetAttr()) {
        MiniATFunctionInfo *MiniATFI = MF.getInfo<MiniATFunctionInfo>();
        unsigned Reg = MiniATFI->getSRetReturnReg();


        if (!Reg)
            llvm_unreachable("sret virtual register not created in the entry block");
        SDValue Val = DAG.getCopyFromReg(Chain, DL, Reg, getPointerTy());
        unsigned R2 = MiniAT::r2;

        Chain = DAG.getCopyToReg(Chain, DL, R2, Val, Flag);
        Flag = Chain.getValue(1);
        RetOps.push_back(DAG.getRegister(R2, getPointerTy()));
    }*/
//@Ordinary struct type: 2 }

    RetOps[0] = Chain;  // Update chain.

    // Add the flag if we have it.
    if (Flag.getNode())
        RetOps.push_back(Flag);
    //return DAG.getNode(MiniAT::ADDRRI, DL, MVT::Other, RetOps);
    //return DAG.getNode(MiniAT::NOP, DL, MVT::Other, RetOps);
    //return DAG.getNode(ISD::RETURNADDR, DL, MVT::Other, RetOps);
    return DAG.getNode(MiniATISD::PRet, DL, MVT::Other, RetOps);
    //return DAG.getNode(MiniATISD::TRet, DL, MVT::Other, RetOps);
}

SDValue MiniATTargetLowering::LowerOperation(
        SDValue Op
        , SelectionDAG &DAG
) const {
    SmallVector<SDValue, 4> RetOps(1);

    switch(Op->getNodeId()){
        default:
            break;
        case ISD::RETURNADDR:
            /*int i = 0;
            int e = Op->getNumOperands();
            while(i < e){
                //save operands
                SDValue operation = Op.getOperand(i);
                ++i;
                switch(operation->getNodeId()){
                    default: llvm_unreachable("Operation not implemented yet");
                    case ISD::CopyToReg:
                        SDValue param = Op.getOperand(i);
                        ++i;
                        SDLoc dl(param);
                        switch(param->getNodeId()){
                            default: llvm_unreachable("Operation not implemented yet");
                            case ISD::Register:
                                RetOps.push_back(DAG.getMachineNode(MiniAT::MOVR, dl, MVT::Other, param));
                        }

                        break;
                }
            }*/
            break;
        case ISD::BlockAddress: return LowerBlockAddress(Op, DAG);

    }
    return Op;
    //return SDValue();
    //return DAG.getNode(ISD::BasicBlock,SDLoc(Op), MVT::Other, RetOps);
  //  return TargetLowering::LowerOperation(Op, DAG);
}

SDValue MiniATTargetLowering::
LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const
{
    SDLoc DL(Op);

    const BlockAddress *BA = cast<BlockAddressSDNode>(Op)->getBlockAddress();
    SDValue Result = DAG.getTargetBlockAddress(BA, getPointerTy());

    return DAG.getNode(MiniATISD::PCRelativeWrapper, DL, getPointerTy(), Result);
}


template<typename Ty>
void MiniATTargetLowering::MiniATCC::
analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
        const SDNode *CallNode, const Type *RetTy) const {
    CCAssignFn *Fn;

    Fn = RetCC_MiniAT;

    for (unsigned I = 0, E = RetVals.size(); I < E; ++I) {
        MVT VT = RetVals[I].VT;
        ISD::ArgFlagsTy Flags = RetVals[I].Flags;
        MVT RegVT = this->getRegVT(VT, RetTy, CallNode, IsSoftFloat);

        if (Fn(I, VT, RegVT, CCValAssign::Full, Flags, this->CCInfo)) {
#ifndef NDEBUG
            dbgs() << "Call result #" << I << " has unhandled type "
            << EVT(VT).getEVTString() << '\n';
#endif
            llvm_unreachable(nullptr);
        }
    }
}

void MiniATTargetLowering::MiniATCC::
analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins, bool IsSoftFloat,
        const SDNode *CallNode, const Type *RetTy) const {
    analyzeReturn(Ins, IsSoftFloat, CallNode, RetTy);
}

void MiniATTargetLowering::MiniATCC::
analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs, bool IsSoftFloat,
        const Type *RetTy) const {
    analyzeReturn(Outs, IsSoftFloat, nullptr, RetTy);
}
unsigned MiniATTargetLowering::MiniATCC::reservedArgArea() const {
    return (CallConv != CallingConv::Fast) ? 8 : 0;
}

MVT MiniATTargetLowering::MiniATCC::getRegVT(MVT VT, const Type *OrigTy,
        const SDNode *CallNode,
        bool IsSoftFloat) const {
    if (IsSoftFloat)
        return VT;

    return VT;
}

MiniATTargetLowering::MiniATCC::MiniATCC(
        CallingConv::ID CC, CCState &Info,
        MiniATCC::SpecialCallingConvType SpecialCallingConv_)
        : CCInfo(Info), CallConv(CC) {
    // Pre-allocate reserved argument area.
    CCInfo.AllocateStack(reservedArgArea(), 1);
}

SDValue MiniATTargetLowering::LowerCall(
        TargetLowering::CallLoweringInfo &info
        , SmallVectorImpl<SDValue> &InVals
) const {
    SelectionDAG &DAG                     = info.DAG;
    SDLoc &dl                             = info.DL;
    SmallVectorImpl<ISD::OutputArg> &Outs = info.Outs;
    SmallVectorImpl<SDValue> &OutVals     = info.OutVals;
    SmallVectorImpl<ISD::InputArg> &Ins   = info.Ins;
    SDValue Chain                         = info.Chain;
    SDValue Callee                        = info.Callee;
    bool &isTailCall                      = info.IsTailCall;
    CallingConv::ID CallConv              = info.CallConv;
    bool isVarArg                         = info.IsVarArg;

    // XCore target does not yet support tail call optimization.
    isTailCall = false;

    // For now, only CallingConv::C implemented
    switch (CallConv)
    {
        default:
            llvm_unreachable("Unsupported calling convention");
        case CallingConv::Fast:
        case CallingConv::C:
            return LowerCCCArguments(Chain,CallConv,isVarArg,Ins,dl,DAG,InVals);
            //return LowerCCCArguments(Chain, Callee, CallConv, isVarArg, isTailCall,
                                  //Outs, OutVals, Ins, dl, DAG, InVals);
    }

}

