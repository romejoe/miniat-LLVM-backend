//===-- MiniATSubtarget.h - Define Subtarget for the MiniAT ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MiniAT specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSUBTARGET_H
#define MINIATSUBTARGET_H

#include "MiniATFrameLowering.h"
#include "MiniATISelLowering.h"
#include "MiniATInstrInfo.h"
#include "MiniATSelectionDAGInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/MC/MCInstrItineraries.h"
#include "llvm/Target/TargetSubtargetInfo.h"
#include <string>

#define GET_SUBTARGETINFO_HEADER
#include "MiniATGenSubtargetInfo.inc"

//namespace
namespace llvm {
    class StringRef;

    class MiniATTargetMachine;

    class MiniATSubtarget : public MiniATGenSubtargetInfo {
        virtual void anchor();

    public:

        //bool HasChapterDummy;
        //bool HasChapterAll;



        // NOTE: O64 will not be supported.
        enum MiniATABIEnum {
            UnknownABI, S32, O32
        };

    protected:
        enum MiniATArchEnum {
            MiniATStandard
        };

        // MiniAT architecture version
        MiniATArchEnum MiniATArchVersion;

        // MiniAT supported ABIs
        MiniATABIEnum MiniATABI;


        InstrItineraryData InstrItins;

        // Relocation Model
        Reloc::Model RM;

        MiniATTargetMachine *TM;

        Triple TargetTriple;

        const DataLayout DL; // Calculates type size & alignment
        const MiniATSelectionDAGInfo TSInfo;
        std::unique_ptr<const MiniATInstrInfo> InstrInfo;
        std::unique_ptr<const MiniATFrameLowering> FrameLowering;
        std::unique_ptr<const MiniATTargetLowering> TLInfo;

    public:
        //bool isABI_S32() const { return MiniATABI == S32; }
        //bool isABI_O32() const { return MiniATABI == O32; }

        //unsigned getTargetABI() const { return MiniATABI; }

        /// This constructor initializes the data members to match that
        /// of the specified triple.
        MiniATSubtarget(const std::string &TT, const std::string &CPU,
                const std::string &FS, Reloc::Model _RM,
                MiniATTargetMachine *_TM);

        //- Vitual function, must have
        /// ParseSubtargetFeatures - Parses features string setting specified
        /// subtarget options.  Definition of function is auto generated by tblgen.
        void ParseSubtargetFeatures(StringRef CPU, StringRef FS);


        //bool hasMiniAT32I() const { return MiniATArchVersion >= MiniAT32I; }
        //bool isMiniAT32I() const { return MiniATArchVersion == MiniAT32I; }
        //bool hasMiniAT32II() const { return MiniATArchVersion >= MiniAT32II; }
        //bool isMiniAT32II() const { return MiniATArchVersion == MiniAT32II; }

        /// Features related to the presence of specific instructions.
        ///bool enableOverflow() const { return EnableOverflow; }
        ///bool disableOverflow() const { return !EnableOverflow; }
        ///bool hasCmp()   const { return HasCmp; }
        ///bool hasSlt()   const { return HasSlt; }

        //bool abiUsesSoftFloat() const;

        const InstrItineraryData *getInstrItineraryData() const {
            return &InstrItins;
        }

        unsigned stackAlignment() const {
            return 8;
        }

        MiniATSubtarget &initializeSubtargetDependencies(StringRef CPU, StringRef FS,
                const TargetMachine *TM);

        const MiniATSelectionDAGInfo *getSelectionDAGInfo() const {
            return &TSInfo;
        }

        const DataLayout *getDataLayout() const {
            return &DL;
        }

        const MiniATInstrInfo *getInstrInfo() const {
            return InstrInfo.get();
        }

        const TargetFrameLowering *getFrameLowering() const {
            return FrameLowering.get();
        }

        const MiniATRegisterInfo *getRegisterInfo() const {
            return &InstrInfo->getRegisterInfo();
        }

        const MiniATTargetLowering *getTargetLowering() const {
            return TLInfo.get();
        }
    };
} // End llvm namespace

#endif

