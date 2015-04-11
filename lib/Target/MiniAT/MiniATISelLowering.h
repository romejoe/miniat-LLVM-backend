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
                    FIRST_NUMBER = ISD::BUILTIN_OP_END,

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
            PRet,
            NOP



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

        SDValue
                LowerCCCArguments(
                SDValue Chain
                , CallingConv::ID CallConv
                , bool isVarArg
                , const SmallVectorImpl<ISD::InputArg> &Ins
                , SDLoc dl
                , SelectionDAG &DAG
                , SmallVectorImpl<SDValue> &InVals
        ) const;

    public:
        explicit MiniATTargetLowering(
                MiniATTargetMachine &TM
                , const MiniATSubtarget &STI
        );

        static const MiniATTargetLowering *create(
                MiniATTargetMachine &TM
                , const MiniATSubtarget &STI
        );

        /// getTargetNodeName - This method returns the name of a target specific
        //  DAG node.
        const char *getTargetNodeName(unsigned Opcode) const override;


/// ByValArgInfo - Byval argument information.
        struct ByValArgInfo {
            unsigned FirstIdx; // Index of the first register used.
            unsigned NumRegs;  // Number of registers used for this argument.
            unsigned Address;  // Offset of the stack area used to pass this argument.

            ByValArgInfo() : FirstIdx(0), NumRegs(0), Address(0) {
            }
        };


        class MiniATCC {
        public:
            enum SpecialCallingConvType {
                NoSpecialCallingConv
            };

            MiniATCC(CallingConv::ID CallConv, CCState &Info,
                    SpecialCallingConvType SpecialCallingConv = NoSpecialCallingConv);

            void analyzeCallResult(const SmallVectorImpl<ISD::InputArg> &Ins,
                    bool IsSoftFloat, const SDNode *CallNode,
                    const Type *RetTy) const;

            void analyzeReturn(const SmallVectorImpl<ISD::OutputArg> &Outs,
                    bool IsSoftFloat, const Type *RetTy) const;

            const CCState &getCCInfo() const { return CCInfo; }

            /// hasByValArg - Returns true if function has byval arguments.
            bool hasByValArg() const { return !ByValArgs.empty(); }

            /// reservedArgArea - The size of the area the caller reserves for
            /// register arguments. This is 16-byte if ABI is O32.
            unsigned reservedArgArea() const;

            typedef SmallVectorImpl<ByValArgInfo>::const_iterator byval_iterator;
            byval_iterator byval_begin() const { return ByValArgs.begin(); }
            byval_iterator byval_end() const { return ByValArgs.end(); }

        private:

            /// Return the type of the register which is used to pass an argument or
            /// return a value. This function returns f64 if the argument is an i64
            /// value which has been generated as a result of softening an f128 value.
            /// Otherwise, it just returns VT.
            MVT getRegVT(MVT VT, const Type *OrigTy, const SDNode *CallNode,
                    bool IsSoftFloat) const;

            template<typename Ty>
            void analyzeReturn(const SmallVectorImpl<Ty> &RetVals, bool IsSoftFloat,
                    const SDNode *CallNode, const Type *RetTy) const;

            CCState &CCInfo;
            CallingConv::ID CallConv;
            SpecialCallingConvType SpecialCallingConv;
            SmallVector<ByValArgInfo, 2> ByValArgs;
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

        bool CanLowerReturn(
                CallingConv::ID CallConv
                , MachineFunction &MF
                , bool IsVarArg
                , SmallVectorImpl<ISD::OutputArg> const &Outs
                , LLVMContext &Context
        ) const;


        // Lower Operand specifics
        /*SDValue lowerGlobalAddress(
                SDValue Op
                , SelectionDAG &DAG
        ) const;

        SDValue lowerJumpTable(
                SDValue Op
                , SelectionDAG &DAG
        ) const;*/

        //- must be exist even without function all
        SDValue
                LowerFormalArguments(
                SDValue Chain
                , CallingConv::ID CallConv
                , bool IsVarArg
                , const SmallVectorImpl<ISD::InputArg> &Ins
                , SDLoc dl
                , SelectionDAG &DAG
                , SmallVectorImpl<SDValue> &InVals
        ) const override;


        SDValue LowerReturn(
                SDValue Chain
                , CallingConv::ID CallConv
                , bool IsVarArg
                , const SmallVectorImpl<ISD::OutputArg> &Outs
                , const SmallVectorImpl<SDValue> &OutVals
                , SDLoc dl
                , SelectionDAG &DAG
        ) const override;


    public:
        virtual SDValue LowerOperation(
                SDValue Op
                , SelectionDAG &DAG
        ) const override;
    };

    const MiniATTargetLowering *
            createMiniATStandardTargetLowering(
            MiniATTargetMachine &TM
            , const MiniATSubtarget &STI
    );
}

#endif // MiniATISELLOWERING_H

