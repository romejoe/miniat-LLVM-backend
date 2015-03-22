//===-- MiniATMCTargetDesc.cpp - MiniAT Target Descriptions -----------------===//
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
#include "MiniATTargetStreamer.h"
#include "llvm/MC/MCCodeGenInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_REGINFO_MC_DESC
#include "MiniATGenRegisterInfo.inc"

#define GET_INSTRINFO_MC_DESC
#include "MiniATGenInstrInfo.inc"

//#define GET_SUBTARGETINFO_MC_DESC
//#include "MiniATGenSubtargetInfo.inc"


/*
static MCInstrInfo *createMiniATMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitMiniATMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createMiniATMCRegisterInfo(StringRef TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitMiniATMCRegisterInfo(X, MiniAT::LR);
  return X;
}

static MCSubtargetInfo *createMiniATMCSubtargetInfo(StringRef TT, StringRef CPU,
                                                   StringRef FS) {
  MCSubtargetInfo *X = new MCSubtargetInfo();
  InitMiniATMCSubtargetInfo(X, TT, CPU, FS);
  return X;
}

static MCAsmInfo *createMiniATMCAsmInfo(const MCRegisterInfo &MRI,
                                       StringRef TT) {
  MCAsmInfo *MAI = new MiniATMCAsmInfo(TT);

  // Initial state of the frame pointer is SP.
  MCCFIInstruction Inst = MCCFIInstruction::createDefCfa(nullptr, MiniAT::SP, 0);
  MAI->addInitialFrameState(Inst);

  return MAI;
}

static MCCodeGenInfo *createMiniATMCCodeGenInfo(StringRef TT, Reloc::Model RM,
                                               CodeModel::Model CM,
                                               CodeGenOpt::Level OL) {
  MCCodeGenInfo *X = new MCCodeGenInfo();

  X->InitMCCodeGenInfo(RM, CM, OL);
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

MiniATTargetStreamer::MiniATTargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}
MiniATTargetStreamer::~MiniATTargetStreamer() {}

namespace {

class MiniATTargetAsmStreamer : public MiniATTargetStreamer {
  formatted_raw_ostream &OS;
public:
  MiniATTargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
  virtual void emitCCTopData(StringRef Name) override;
  virtual void emitCCTopFunction(StringRef Name) override;
  virtual void emitCCBottomData(StringRef Name) override;
  virtual void emitCCBottomFunction(StringRef Name) override;
};

MiniATTargetAsmStreamer::MiniATTargetAsmStreamer(MCStreamer &S,
                                               formatted_raw_ostream &OS)
    : MiniATTargetStreamer(S), OS(OS) {}

void MiniATTargetAsmStreamer::emitCCTopData(StringRef Name) {
  OS << "\t.cc_top " << Name << ".data," << Name << '\n';
}

void MiniATTargetAsmStreamer::emitCCTopFunction(StringRef Name) {
  OS << "\t.cc_top " << Name << ".function," << Name << '\n';
}

void MiniATTargetAsmStreamer::emitCCBottomData(StringRef Name) {
  OS << "\t.cc_bottom " << Name << ".data\n";
}

void MiniATTargetAsmStreamer::emitCCBottomFunction(StringRef Name) {
  OS << "\t.cc_bottom " << Name << ".function\n";
}
}

static MCStreamer *
createMiniATMCAsmStreamer(MCContext &Ctx, formatted_raw_ostream &OS,
                         bool isVerboseAsm, bool useDwarfDirectory,
                         MCInstPrinter *InstPrint, MCCodeEmitter *CE,
                         MCAsmBackend *TAB, bool ShowInst) {
  MCStreamer *S = llvm::createAsmStreamer(
      Ctx, OS, isVerboseAsm, useDwarfDirectory, InstPrint, CE, TAB, ShowInst);
  new MiniATTargetAsmStreamer(*S, OS);
  return S;
}
*/
// Force static initialization.
extern "C" void LLVMInitializeMiniATTargetMC() {
  // Register the MC asm info.
  /*RegisterMCAsmInfoFn X(TheMiniATTarget, createMiniATMCAsmInfo);

  // Register the MC codegen info.
  TargetRegistry::RegisterMCCodeGenInfo(TheMiniATTarget,
                                        createMiniATMCCodeGenInfo);

  // Register the MC instruction info.
  TargetRegistry::RegisterMCInstrInfo(TheMiniATTarget, createMiniATMCInstrInfo);

  // Register the MC register info.
  TargetRegistry::RegisterMCRegInfo(TheMiniATTarget, createMiniATMCRegisterInfo);

  // Register the MC subtarget info.
  TargetRegistry::RegisterMCSubtargetInfo(TheMiniATTarget,
                                          createMiniATMCSubtargetInfo);

  // Register the MCInstPrinter
  TargetRegistry::RegisterMCInstPrinter(TheMiniATTarget,
                                        createMiniATMCInstPrinter);

  TargetRegistry::RegisterAsmStreamer(TheMiniATTarget, createMiniATMCAsmStreamer);
  */
}
