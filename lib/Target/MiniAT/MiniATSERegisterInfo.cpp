//===-- MiniATSERegisterInfo.cpp - MINIAT Register Information ------== -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MINIAT implementation of the TargetRegisterInfo
// class.
//
//===----------------------------------------------------------------------===//

#include "MiniATSERegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-reg-info"

MiniATSERegisterInfo::MiniATSERegisterInfo(const MiniATSubtarget &ST)
  : MiniATRegisterInfo(ST) {}

const TargetRegisterClass *
MiniATSERegisterInfo::intRegClass(unsigned Size) const {
  return &MiniAT::GPRRegClass;
}

