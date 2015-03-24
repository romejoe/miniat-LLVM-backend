//===-- MiniATMCTargetDesc.h - MiniAT Target Descriptions ---------*- C++ -*-===//
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

#ifndef MINIATMCTARGETDESC_H
#define MINIATMCTARGETDESC_H

namespace llvm {
class Target;

extern Target TheMiniATTarget;

} // End llvm namespace

// Defines symbolic names for MiniAT registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "MiniATGenRegisterInfo.inc"

// Defines symbolic names for the MiniAT instructions.
//
#define GET_INSTRINFO_ENUM
#include "MiniATGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "MiniATGenSubtargetInfo.inc"

#endif
