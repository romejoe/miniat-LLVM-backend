//===-- MiniATISelLowering.h - MiniAT DAG Lowering Interface --------*- C++ -*-===//
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

#ifndef MiniATISELLOWERING_H
#define MiniATISELLOWERING_H

#include "MiniAT.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/IR/Function.h"
#include "llvm/Target/TargetLowering.h"
#include <deque>

namespace llvm {
    namespace MiniATISD {
        enum NodeType {
            // Start the numbering from where ISD NodeType finishes.
                    FIRST_NUMBER = ISD::BUILTIN_OP_END//,

            // Jump and link (call)
            //JmpLink,

            // Tail call
            //TailCall,

            // Get the Higher 16 bits from a 32-bit immediate
            // No relation with MiniAT Hi register
            //Hi,
            // Get the Lower 16 bits from a 32-bit immediate
            // No relation with MiniAT Lo register
            //Lo,

            // Handle gp_rel (small data/bss sections) relocation.
            //GPRel,

            // Thread Pointer
            //ThreadPointer,

            // Return
            //Ret,

            // DivRem(u)
            //DivRem,
            //DivRemU,

            //Wrapper,
            //DynAlloc,

            //Sync
        };
    }

    //===--------------------------------------------------------------------===//
    // TargetLowering Implementation
    //===--------------------------------------------------------------------===//
    class MiniATFunctionInfo;

    class MiniATSubtarget;

    //@class MiniATTargetLowering
    class MiniATTargetLowering : public TargetLowering {

        /// MiniATCC - This class provides methods used to analyze formal and call
        /// arguments and inquire about calling convention information.
        class MiniATCC {

            void analyzeFormalArguments(const SmallVectorImpl<ISD::InputArg> &Ins,
                                        bool IsSoftFloat,
                                        Function::const_arg_iterator FuncArg);
            /// regSize - Size (in number of bits) of integer registers.
            unsigned regSize() const { return 32; }
            /// numIntArgRegs - Number of integer registers available for calls.
            unsigned numIntArgRegs() const;

            /// Return pointer to array of integer argument registers.
            const MCPhysReg *intArgRegs() const;

            void handleByValArg(unsigned ValNo, MVT ValVT, MVT LocVT,
                                CCValAssign::LocInfo LocInfo,
                                ISD::ArgFlagsTy ArgFlags);

            /// useRegsForByval - Returns true if the calling convention allows the
            /// use of registers to pass byval arguments.
            bool useRegsForByval() const { return CallConv != CallingConv::Fast; }

            /// Return the function that analyzes fixed argument list functions.
            llvm::CCAssignFn *fixedArgFn() const;

            void allocateRegs(ByValArgInfo &ByVal, unsigned ByValSize,
                              unsigned Align);
        };
    public:
        explicit MiniATTargetLowering(MiniATTargetMachine &TM,
                                      const MiniATSubtarget &STI);

        static const MiniATTargetLowering *create(MiniATTargetMachine &TM,
                                                  const MiniATSubtarget &STI);

        /// getTargetNodeName - This method returns the name of a target specific
        //  DAG node.
        const char *getTargetNodeName(unsigned Opcode) const override;

    protected:

        /// ByValArgInfo - Byval argument information.
        struct ByValArgInfo {
            unsigned FirstIdx; // Index of the first register used.
            unsigned NumRegs;  // Number of registers used for this argument.
            unsigned Address;  // Offset of the stack area used to pass this argument.

            ByValArgInfo() : FirstIdx(0), NumRegs(0), Address(0) {
            }
        };

    protected:
        // Subtarget Info
        const MiniATSubtarget &Subtarget;

    private:

#if 0
    // Create a TargetConstantPool node.
    SDValue getTargetNode(ConstantPoolSDNode *N, EVT Ty, SelectionDAG &DAG,
                          unsigned Flag) const;
#endif

        // Lower Operand specifics
        SDValue lowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

        SDValue lowerJumpTable(SDValue Op, SelectionDAG &DAG) const;

        //- must be exist even without function all
        SDValue
                LowerFormalArguments(SDValue Chain,
                                     CallingConv::ID CallConv, bool IsVarArg,
                                     const SmallVectorImpl<ISD::InputArg> &Ins,
                                     SDLoc dl, SelectionDAG &DAG,
                                     SmallVectorImpl<SDValue> &InVals) const override;

        SDValue LowerReturn(SDValue Chain,
                            CallingConv::ID CallConv, bool IsVarArg,
                            const SmallVectorImpl<ISD::OutputArg> &Outs,
                            const SmallVectorImpl<SDValue> &OutVals,
                            SDLoc dl, SelectionDAG &DAG) const override;

    };

    const MiniATTargetLowering *
            createMiniATStandardTargetLowering(MiniATTargetMachine &TM, const MiniATSubtarget &STI);
}

#endif // MiniATISELLOWERING_H

