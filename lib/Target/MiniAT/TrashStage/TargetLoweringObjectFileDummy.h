//==-- TargetLoweringObjectFileMiniAT.h - Object Info -*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements classes used to handle lowerings specific to common
// object file formats.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CODEGEN_TARGETLOWERINGOBJECTFILEMINIAT_H
#define LLVM_CODEGEN_TARGETLOWERINGOBJECTFILEMINIAT_H

#include "llvm/ADT/StringRef.h"
#include "llvm/MC/SectionKind.h"
#include "llvm/Target/TargetLoweringObjectFile.h"

namespace llvm {
    class MachineModuleInfo;

    class Mangler;

    class MCAsmInfo;

    class MCExpr;

    class MCSection;

    class MCSectionMachO;

    class MCSymbol;

    class MCContext;

    class GlobalValue;

    class TargetMachine;


    class TargetLoweringObjectFileDummy : public TargetLoweringObjectFile {
        bool UseInitArray;

    public:
        ~TargetLoweringObjectFileDummy();

        void emitPersonalityValue(MCStreamer &Streamer, const TargetMachine &TM,
                const MCSymbol *Sym) const override;

        /// Given a constant with the SectionKind, return a section that it should be
        /// placed in.
        const MCSection *getSectionForConstant(SectionKind Kind,
                const Constant *C) const override;

        const MCSection *getExplicitSectionGlobal(const GlobalValue *GV,
                SectionKind Kind, Mangler &Mang,
                const TargetMachine &TM) const override;

        const MCSection *SelectSectionForGlobal(const GlobalValue *GV,
                SectionKind Kind, Mangler &Mang,
                const TargetMachine &TM) const override;

        /// Return an MCExpr to use for a reference to the specified type info global
        /// variable from exception handling information.
        const MCExpr *
                getTTypeGlobalReference(const GlobalValue *GV, unsigned Encoding,
                Mangler &Mang, const TargetMachine &TM,
                MachineModuleInfo *MMI,
                MCStreamer &Streamer) const override;

        // The symbol that gets passed to .cfi_personality.
        MCSymbol *getCFIPersonalitySymbol(const GlobalValue *GV, Mangler &Mang,
                const TargetMachine &TM,
                MachineModuleInfo *MMI) const override;

        const MCSection *getStaticCtorSection(unsigned Priority,
                const MCSymbol *KeySym) const override;

        const MCSection *getStaticDtorSection(unsigned Priority,
                const MCSymbol *KeySym) const override;
    };


}
#endif
