//===-- MiniATTargetStreamer.h - MiniAT Target Streamer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATTARGETSTREAMER_H
#define MINIATTARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {
class MiniATTargetStreamer : public MCTargetStreamer {
public:
  MiniATTargetStreamer(MCStreamer &S);
  virtual ~MiniATTargetStreamer();
  virtual void emitCCTopData(StringRef Name) = 0;
  virtual void emitCCTopFunction(StringRef Name) = 0;
  virtual void emitCCBottomData(StringRef Name) = 0;
  virtual void emitCCBottomFunction(StringRef Name) = 0;
};
}

#endif
