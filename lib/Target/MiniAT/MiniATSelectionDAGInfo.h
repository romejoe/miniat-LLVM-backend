//===-- MiniATSelectionDAGInfo.h - MiniAT SelectionDAG Info ---------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the MiniAT subclass for TargetSelectionDAGInfo.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSELECTIONDAGINFO_H
#define MINIATSELECTIONDAGINFO_H

#include "llvm/Target/TargetSelectionDAGInfo.h"

namespace llvm {

class MiniATTargetMachine;

class MiniATSelectionDAGInfo : public TargetSelectionDAGInfo {
public:
  explicit MiniATSelectionDAGInfo(const DataLayout &DL);
  ~MiniATSelectionDAGInfo();
};

}

#endif

