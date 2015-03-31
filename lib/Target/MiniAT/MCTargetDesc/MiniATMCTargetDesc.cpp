//===-- MiniATMCTargetDesc.cpp - MiniAT Target Descriptions -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides MiniAT specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MiniATMCTargetDesc.h"
#include "InstPrinter/MiniATInstPrinter.h"
#include "MiniATMCAsmInfo.h"


#include "llvm/MC/MachineLocation.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_MC_DESC
#include "MiniATGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "MiniATGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "MiniATGenRegisterInfo.inc"


//@1 {
/// Select the MiniAT Architecture Feature for the given triple and cpu name.
/// The function will be called at command 'llvm-objdump -d' for MiniAT elf input.
static StringRef selectMiniATArchFeature(StringRef TT, StringRef CPU) {
    std::string MiniATArchFeature;
    /*if (CPU.empty() || CPU == "generic") {
        Triple TheTriple(TT);
        if (TheTriple.getArch() == Triple::miniat)
            if (CPU.empty() || CPU == "cpu032II")
                MiniATArchFeature = "+cpu032II";
            else if (CPU == "cpu032I")
                MiniATArchFeature = "+cpu032I";
    }*/
    return MiniATArchFeature;
}
//@1 }

static MCInstrInfo *createMiniATMCInstrInfo() {
    MCInstrInfo *X = new MCInstrInfo();
    InitMiniATMCInstrInfo(X); // defined in MiniATGenInstrInfo.inc
    return X;
}

static MCRegisterInfo *createMiniATMCRegisterInfo(StringRef TT) {
    MCRegisterInfo *X = new MCRegisterInfo();
    //r252 == LR
    InitMiniATMCRegisterInfo(X, MiniAT::r252); // defined in MiniATGenRegisterInfo.inc
    return X;
}

static MCSubtargetInfo *createMiniATMCSubtargetInfo(StringRef TT, StringRef CPU,
        StringRef FS) {
    std::string ArchFS = selectMiniATArchFeature(TT,CPU);
    if (!FS.empty()) {
        if (!ArchFS.empty())
            ArchFS = ArchFS + "," + FS.str();
        else
            ArchFS = FS;
    }
    MCSubtargetInfo *X = new MCSubtargetInfo();
    InitMiniATMCSubtargetInfo(X, TT, CPU, ArchFS); // defined in MiniATGenSubtargetInfo.inc
    return X;
}

static MCAsmInfo *createMiniATMCAsmInfo(const MCRegisterInfo &MRI, StringRef TT) {
    MCAsmInfo *MAI = new MiniATMCAsmInfo(TT);

    unsigned SP = MRI.getDwarfRegNum(MiniAT::r254, true);
    MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(0, SP, 0);
    MAI->addInitialFrameState(Inst);

    return MAI;
}

static MCCodeGenInfo *createMiniATMCCodeGenInfo(StringRef TT, Reloc::Model RM,
        CodeModel::Model CM,
        CodeGenOpt::Level OL) {
    //TODO: check
    MCCodeGenInfo *X = new MCCodeGenInfo();
    if (CM == CodeModel::JITDefault)
        RM = Reloc::Static;
    else if (RM == Reloc::Default)
        RM = Reloc::PIC_;
    X->InitMCCodeGenInfo(RM, CM, OL); // defined in lib/MC/MCCodeGenInfo.cpp
    return X;
}

static MCInstPrinter *createMiniATMCInstPrinter(const Target &T,
        unsigned SyntaxVariant,
        const MCAsmInfo &MAI,
        const MCInstrInfo &MII,
        const MCRegisterInfo &MRI,
        const MCSubtargetInfo &STI) {
    return new MiniATInstPrinter(MAI, MII, MRI);
}

#if CH >= CH5_1 //1
static MCStreamer *createMCStreamer(const Target &T, StringRef TT,
        MCContext &Context, MCAsmBackend &MAB,
        raw_ostream &OS, MCCodeEmitter *Emitter,
        const MCSubtargetInfo &STI, bool RelaxAll) {
    return createELFStreamer(Context, MAB, OS, Emitter, RelaxAll);
}

static MCStreamer *
createMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
        bool isVerboseAsm,
        bool useDwarfDirectory, MCInstPrinter *InstPrint,
        MCCodeEmitter *CE, MCAsmBackend *TAB, bool ShowInst) {
    return llvm::createAsmStreamer(Ctx, OS, isVerboseAsm,
            useDwarfDirectory, InstPrint, CE, TAB,
            ShowInst);
}
#endif

//LLVMInitializeMiniATTargetMC
extern "C" void LLVMInitializeMiniATTargetMC() {

    // Register the MC asm info.
    RegisterMCAsmInfoFn X(TheMiniATTarget, createMiniATMCAsmInfo);


    // Register the MC codegen info.
    TargetRegistry::RegisterMCCodeGenInfo(TheMiniATTarget,
            createMiniATMCCodeGenInfo);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(TheMiniATTarget, createMiniATMCInstrInfo);


    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(TheMiniATTarget, createMiniATMCRegisterInfo);

/*
#if CH >= CH5_1 //2
    // Register the MC Code Emitter
    TargetRegistry::RegisterMCCodeEmitter(TheMiniATTarget,
            createMiniATMCCodeEmitterEB);
    TargetRegistry::RegisterMCCodeEmitter(TheMiniATelTarget,
            createMiniATMCCodeEmitterEL);

    // Register the object streamer.
    TargetRegistry::RegisterMCObjectStreamer(TheMiniATTarget, createMCStreamer);
    TargetRegistry::RegisterMCObjectStreamer(TheMiniATelTarget, createMCStreamer);

    // Register the asm streamer.
    TargetRegistry::RegisterAsmStreamer(TheMiniATTarget, createMCAsmStreamer);
    TargetRegistry::RegisterAsmStreamer(TheMiniATelTarget, createMCAsmStreamer);

    // Register the asm backend.
    TargetRegistry::RegisterMCAsmBackend(TheMiniATTarget,
            createMiniATAsmBackendEB32);
    TargetRegistry::RegisterMCAsmBackend(TheMiniATelTarget,
            createMiniATAsmBackendEL32);
#endif
 */
    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(TheMiniATTarget,
            createMiniATMCSubtargetInfo);
    //TargetRegistry::RegisterMCSubtargetInfo(TheMiniATelTarget,
    //        createMiniATMCSubtargetInfo);
    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(TheMiniATTarget,
            createMiniATMCInstPrinter);
}
