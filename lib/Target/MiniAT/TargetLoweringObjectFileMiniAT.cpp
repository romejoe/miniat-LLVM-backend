//===-- TargetLoweringObjectFileMiniAT.cpp - Object File Info --===//
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
#include "TargetLoweringObjectFileMiniAT.h"
using namespace llvm;
using namespace dwarf;
//===----------------------------------------------------------------------===//
//                                  MiniAT
//===----------------------------------------------------------------------===//

MCSymbol *TargetLoweringObjectFileMiniAT::getCFIPersonalitySymbol(
    const GlobalValue *GV, Mangler &Mang, const TargetMachine &TM,
    MachineModuleInfo *MMI) const {
  report_fatal_error("We do not support this encoding yet!");
}

void TargetLoweringObjectFileMiniAT::emitPersonalityValue(MCStreamer &Streamer,
                                                       const TargetMachine &TM,
                                                       const MCSymbol *Sym) const {
  /*SmallString<64> NameData("DW.ref.");
  NameData += Sym->getName();
  MCSymbol *Label = getContext().GetOrCreateSymbol(NameData);
  Streamer.EmitSymbolAttribute(Label, MCSA_Hidden);
  Streamer.EmitSymbolAttribute(Label, MCSA_Weak);
  StringRef Prefix = ".data.";
  NameData.insert(NameData.begin(), Prefix.begin(), Prefix.end());
  unsigned Flags = ELF::SHF_ALLOC | ELF::SHF_WRITE | ELF::SHF_GROUP;
  const MCSection *Sec = getContext().getELFSection(NameData,
                                                    ELF::SHT_PROGBITS,
                                                    Flags,
                                                    SectionKind::getDataRel(),
                                                    0, Label->getName());
  unsigned Size = TM.getDataLayout()->getPointerSize();
  Streamer.SwitchSection(Sec);
  Streamer.EmitValueToAlignment(TM.getDataLayout()->getPointerABIAlignment());
  Streamer.EmitSymbolAttribute(Label, MCSA_ELF_TypeObject);
  const MCExpr *E = MCConstantExpr::Create(Size, getContext());
  Streamer.EmitELFSize(Label, E);
  Streamer.EmitLabel(Label);

  Streamer.EmitSymbolValue(Sym, Size);*/
}

const MCExpr *TargetLoweringObjectFileMiniAT::getTTypeGlobalReference(
    const GlobalValue *GV, unsigned Encoding, Mangler &Mang,
    const TargetMachine &TM, MachineModuleInfo *MMI,
    MCStreamer &Streamer) const {
/*
  if (Encoding & dwarf::DW_EH_PE_indirect) {
    MachineModuleInfoELF &ELFMMI = MMI->getObjFileInfo<MachineModuleInfoELF>();

    MCSymbol *SSym = getSymbolWithGlobalValueBase(GV, ".DW.stub", Mang, TM);

    // Add information about the stub reference to ELFMMI so that the stub
    // gets emitted by the asmprinter.
    MachineModuleInfoImpl::StubValueTy &StubSym = ELFMMI.getGVStubEntry(SSym);
    if (!StubSym.getPointer()) {
      MCSymbol *Sym = TM.getSymbol(GV, Mang);
      StubSym = MachineModuleInfoImpl::StubValueTy(Sym, !GV->hasLocalLinkage());
    }

    return TargetLoweringObjectFile::
      getTTypeReference(MCSymbolRefExpr::Create(SSym, getContext()),
                        Encoding & ~dwarf::DW_EH_PE_indirect, Streamer);
  }

  return TargetLoweringObjectFile::
    getTTypeGlobalReference(GV, Encoding, Mang, TM, MMI, Streamer);
    */
    return NULL;
}

const MCSection *TargetLoweringObjectFileMiniAT::getExplicitSectionGlobal(
    const GlobalValue *GV, SectionKind Kind, Mangler &Mang,
    const TargetMachine &TM) const {
  /*StringRef SectionName = GV->getSection();

  // Infer section flags from the section name if we can.
  Kind = getELFKindForNamedSection(SectionName, Kind);

  StringRef Group = "";
  unsigned Flags = getELFSectionFlags(Kind);
  if (const Comdat *C = getELFComdat(GV)) {
    Group = C->getName();
    Flags |= ELF::SHF_GROUP;
  }
  return getContext().getELFSection(SectionName,
                                    getELFSectionType(SectionName, Kind), Flags,
                                    Kind, EntrySize=0, Group);
                                    */
  return NULL;
}


const MCSection *TargetLoweringObjectFileMiniAT::
SelectSectionForGlobal(const GlobalValue *GV, SectionKind Kind,
                       Mangler &Mang, const TargetMachine &TM) const {
  // If we have -ffunction-section or -fdata-section then we should emit the
  // global value to a uniqued section specifically for it.
  /*bool EmitUniquedSection;
  if (Kind.isText())
    EmitUniquedSection = TM.getFunctionSections();
  else
    EmitUniquedSection = TM.getDataSections();

  // If this global is linkonce/weak and the target handles this by emitting it
  // into a 'uniqued' section name, create and return the section now.
  if ((GV->isWeakForLinker() || EmitUniquedSection || GV->hasComdat()) &&
      !Kind.isCommon()) {
    StringRef Prefix = getSectionPrefixForGlobal(Kind);

    SmallString<128> Name(Prefix);
    TM.getNameWithPrefix(Name, GV, Mang, true);

    StringRef Group = "";
    unsigned Flags = getELFSectionFlags(Kind);
    if (GV->isWeakForLinker() || GV->hasComdat()) {
      if (const Comdat *C = getELFComdat(GV))
        Group = C->getName();
      else
        Group = Name.substr(Prefix.size());
      Flags |= ELF::SHF_GROUP;
    }

    return getContext().getELFSection(Name.str(),
                                      getELFSectionType(Name.str(), Kind),
                                      Flags, Kind, 0, Group);
  }

  if (Kind.isText()) return TextSection;

  if (Kind.isMergeable1ByteCString() ||
      Kind.isMergeable2ByteCString() ||
      Kind.isMergeable4ByteCString()) {

    // We also need alignment here.
    // FIXME: this is getting the alignment of the character, not the
    // alignment of the global!
    unsigned Align =
      TM.getDataLayout()->getPreferredAlignment(cast<GlobalVariable>(GV));

    const char *SizeSpec = ".rodata.str1.";
    if (Kind.isMergeable2ByteCString())
      SizeSpec = ".rodata.str2.";
    else if (Kind.isMergeable4ByteCString())
      SizeSpec = ".rodata.str4.";
    else
      assert(Kind.isMergeable1ByteCString() && "unknown string width");


    std::string Name = SizeSpec + utostr(Align);
    return getContext().getELFSection(Name, ELF::SHT_PROGBITS,
                                      ELF::SHF_ALLOC |
                                      ELF::SHF_MERGE |
                                      ELF::SHF_STRINGS,
                                      Kind);
            
  }

  if (Kind.isMergeableConst()) {
    if (Kind.isMergeableConst4() && MergeableConst4Section)
      return MergeableConst4Section;
    if (Kind.isMergeableConst8() && MergeableConst8Section)
      return MergeableConst8Section;
    if (Kind.isMergeableConst16() && MergeableConst16Section)
      return MergeableConst16Section;
    return ReadOnlySection;  // .const
  }

  if (Kind.isReadOnly())             return ReadOnlySection;

  if (Kind.isThreadData())           return TLSDataSection;
  if (Kind.isThreadBSS())            return TLSBSSSection;

  // Note: we claim that common symbols are put in BSSSection, but they are
  // really emitted with the magic .comm directive, which creates a symbol table
  // entry but not a section.
  if (Kind.isBSS() || Kind.isCommon()) return BSSSection;

  if (Kind.isDataNoRel())            return DataSection;
  if (Kind.isDataRelLocal())         return DataRelLocalSection;
  if (Kind.isDataRel())              return DataRelSection;
  if (Kind.isReadOnlyWithRelLocal()) return DataRelROLocalSection;

  assert(Kind.isReadOnlyWithRel() && "Unknown section kind");
  return DataRelROSection;
                            */
return NULL;
}

/// getSectionForConstant - Given a mergeable constant with the
/// specified size and relocation information, return a section that it
/// should be placed in.
const MCSection *
TargetLoweringObjectFileMiniAT::getSectionForConstant(SectionKind Kind,
                                                   const Constant *C) const {
/*  if (Kind.isMergeableConst4() && MergeableConst4Section)
    return MergeableConst4Section;
  if (Kind.isMergeableConst8() && MergeableConst8Section)
    return MergeableConst8Section;
  if (Kind.isMergeableConst16() && MergeableConst16Section)
    return MergeableConst16Section;
  if (Kind.isReadOnly())
    return ReadOnlySection;

  if (Kind.isReadOnlyWithRelLocal()) return DataRelROLocalSection;
  assert(Kind.isReadOnlyWithRel() && "Unknown section kind");
  return DataRelROSection;
                            */
return NULL;
}

const MCSection *TargetLoweringObjectFileMiniAT::getStaticCtorSection(
    unsigned Priority, const MCSymbol *KeySym) const {
 /* // The default scheme is .ctor / .dtor, so we have to invert the priority
  // numbering.
  if (Priority == 65535)
    return StaticCtorSection;

  if (UseInitArray) {
    std::string Name = std::string(".init_array.") + utostr(Priority);
    return getContext().getELFSection(Name, ELF::SHT_INIT_ARRAY,
                                      ELF::SHF_ALLOC | ELF::SHF_WRITE,
                                      SectionKind::getDataRel());
  } else {
    std::string Name = std::string(".ctors.") + utostr(65535 - Priority);
    return getContext().getELFSection(Name, ELF::SHT_PROGBITS,
                                      ELF::SHF_ALLOC |ELF::SHF_WRITE,
                                      SectionKind::getDataRel());
  }
                            */
return NULL;
}

const MCSection *TargetLoweringObjectFileMiniAT::getStaticDtorSection(
    unsigned Priority, const MCSymbol *KeySym) const {
  /*// The default scheme is .ctor / .dtor, so we have to invert the priority
  // numbering.
  if (Priority == 65535)
    return StaticDtorSection;

  if (UseInitArray) {
    std::string Name = std::string(".fini_array.") + utostr(Priority);
    return getContext().getELFSection(Name, ELF::SHT_FINI_ARRAY,
                                      ELF::SHF_ALLOC | ELF::SHF_WRITE,
                                      SectionKind::getDataRel());
  } else {
    std::string Name = std::string(".dtors.") + utostr(65535 - Priority);
    return getContext().getELFSection(Name, ELF::SHT_PROGBITS,
                                      ELF::SHF_ALLOC |ELF::SHF_WRITE,
                                      SectionKind::getDataRel());
  }
                            */
return NULL;
}
