
#ifndef MINIATTARGETMACHINE_H
#define MINIATTARGETMACHINE_H

#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {

  class Module;

  class miniatTargetMachine :: public LLVMTargetMachine : DataLayout(/*TODO*/),FrameInfo(/*TODO*/){

    const DataLayout DataLayout;
    //no need for subtarget at the moment
    miniatInstrInfo InstrInfo;
    TargetFrameInfo FrameInfo;

  protected:

  public:

  };

} // End llvm namespace

#endif
