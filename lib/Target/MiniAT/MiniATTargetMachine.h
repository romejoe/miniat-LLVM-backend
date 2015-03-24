//===-- MiniATTargetMachine.h - Define TargetMachine for MiniAT -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MiniAT specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATTARGETMACHINE_H
#define MINIATTARGETMACHINE_H

#include <llvm/Support/TargetRegistry.h>
#include "MiniATSubtarget.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/SelectionDAGISel.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
    class formatted_raw_ostream;

    class MiniATRegisterInfo;

    class MiniATTargetMachine : public TargetMachine {
        std::unique_ptr<TargetLoweringObjectFile> TLOF;
        MiniATSubtarget *Subtarget;
        MiniATSubtarget DefaultSubtarget;

        mutable StringMap<std::unique_ptr<MiniATSubtarget>> SubtargetMap;
    public:
        MiniATTargetMachine(const Target &T, StringRef TT, StringRef CPU,
                StringRef FS, const TargetOptions &Options,
                Reloc::Model RM, CodeModel::Model CM,
                CodeGenOpt::Level OL);

        ~MiniATTargetMachine();

        const MiniATSubtarget *getSubtargetImpl() const override {
            if (Subtarget)
                return Subtarget;
            return &DefaultSubtarget;
        }

        /// \brief Reset the subtarget for the MiniAT target.
        void resetSubtarget(MachineFunction *MF);

        // Pass Pipeline Configuration
        virtual TargetPassConfig *createPassConfig(PassManagerBase &PM);

        TargetLoweringObjectFile *getObjFileLowering() const {
            return TLOF.get();
        }
    };

/// MiniATStandardTargetMachine
///
    class MiniATStandardTargetMachine : public MiniATTargetMachine {
    public:
        MiniATStandardTargetMachine(
                Target const &T
                , StringRef const &TT
                , StringRef const &CPU
                , StringRef const &FS
                , TargetOptions const &Options
                , Reloc::Model const &RM
                , CodeModel::Model const &CM
                , CodeGenOpt::Level const &OL)
        : MiniATTargetMachine(
            T
            , TT
            , CPU
            , FS
            , Options
            , RM
            , CM
            , OL
        ){}


    private:
        virtual void anchor();

    //public:
        //MiniATStandadTargetMachine(const Target &T, StringRef TT,
        //        StringRef CPU, StringRef FS, const TargetOptions &Options,
        //        Reloc::Model RM, CodeModel::Model CM,
        //        CodeGenOpt::Level OL)::
    };

} // End llvm namespace

#endif