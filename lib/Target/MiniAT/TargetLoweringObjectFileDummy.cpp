//===-- TargetLoweringObjectFileDummy.cpp - Object File Info --===//
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


#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Triple.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Mangler.h"
#include "llvm/IR/Module.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCSectionCOFF.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCSectionMachO.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Dwarf.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "TargetLoweringObjectFileDummy.h"
using namespace llvm;
using namespace dwarf;
//===----------------------------------------------------------------------===//
//                                  MiniAT
//===----------------------------------------------------------------------===//

MCSymbol *TargetLoweringObjectFileDummy::getCFIPersonalitySymbol(
    const GlobalValue *GV, Mangler &Mang, const TargetMachine &TM,
    MachineModuleInfo *MMI) const {
  report_fatal_error("We do not support this encoding yet!");
}

void TargetLoweringObjectFileDummy::emitPersonalityValue(MCStreamer &Streamer,
                                                       const TargetMachine &TM,
                                                       const MCSymbol *Sym) const {

}

const MCExpr *TargetLoweringObjectFileDummy::getTTypeGlobalReference(
    const GlobalValue *GV, unsigned Encoding, Mangler &Mang,
    const TargetMachine &TM, MachineModuleInfo *MMI,
    MCStreamer &Streamer) const {

    return NULL;
}

const MCSection *TargetLoweringObjectFileDummy::getExplicitSectionGlobal(
    const GlobalValue *GV, SectionKind Kind, Mangler &Mang,
    const TargetMachine &TM) const {
  
  return NULL;
}


const MCSection *TargetLoweringObjectFileDummy::
SelectSectionForGlobal(const GlobalValue *GV, SectionKind Kind,
                       Mangler &Mang, const TargetMachine &TM) const {
  
return NULL;
}


const MCSection *
TargetLoweringObjectFileDummy::getSectionForConstant(SectionKind Kind,
                                                   const Constant *C) const {

return NULL;
}

const MCSection *TargetLoweringObjectFileDummy::getStaticCtorSection(
    unsigned Priority, const MCSymbol *KeySym) const {
 
return NULL;
}

const MCSection *TargetLoweringObjectFileDummy::getStaticDtorSection(
    unsigned Priority, const MCSymbol *KeySym) const {
  
return NULL;
}

TargetLoweringObjectFileDummy::~TargetLoweringObjectFileDummy() {

}
