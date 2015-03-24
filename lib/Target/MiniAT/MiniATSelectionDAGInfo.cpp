//===-- MiniATSelectionDAGInfo.cpp - MiniAT SelectionDAG Info -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MiniATSelectionDAGInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATSelectionDAGInfo.h"

#include "MiniATTargetMachine.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-selectiondag-info"

MiniATSelectionDAGInfo::MiniATSelectionDAGInfo(const DataLayout &DL)
    : TargetSelectionDAGInfo(&DL) {}

MiniATSelectionDAGInfo::~MiniATSelectionDAGInfo() {
}

