//===-- MiniATStandardRegisterInfo.cpp - MINIAT Register Information ------== -------===//
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

#include "MiniATStandardRegisterInfo.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-reg-info"

MiniATStandardRegisterInfo::MiniATStandardRegisterInfo(const MiniATSubtarget &ST)
  : MiniATRegisterInfo(ST) {}

const TargetRegisterClass *
MiniATStandardRegisterInfo::intRegClass(unsigned Size) const {
  return &MiniAT::GPRRegClass;
}

