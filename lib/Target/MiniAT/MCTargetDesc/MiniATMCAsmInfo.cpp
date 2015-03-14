//===-- MiniATMCAsmInfo.cpp - XCore asm properties -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MiniATMCAsmInfo.h"
#include "llvm/ADT/StringRef.h"
using namespace llvm;

void MiniATMCAsmInfo::anchor() { }

MiniATMCAsmInfo::MiniATMCAsmInfo(StringRef TT) {
  SupportsDebugInformation = false;
  //Data16bitsDirective = "\t.short\t";
  //Data32bitsDirective = "\t.long\t";
  //Data64bitsDirective = nullptr;
  //ZeroDirective = "\t.space\t";
  CommentString = "#";

  //AscizDirective = ".asciiz";

  //HiddenVisibilityAttr = MCSA_Invalid;
  //HiddenDeclarationVisibilityAttr = MCSA_Invalid;
  //ProtectedVisibilityAttr = MCSA_Invalid;

  // Debug
  //HasLEB128 = true;
  //ExceptionsType = ExceptionHandling::DwarfCFI;
  //DwarfRegNumForCFI = true;
}

