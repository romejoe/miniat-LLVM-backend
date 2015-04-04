//===-- MiniATTargetMachine.cpp - Define TargetMachine for MiniAT -------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implements the info about MiniAT target spec.
//
//===----------------------------------------------------------------------===//

#include "MiniATTargetMachine.h"
#include "MiniAT.h"
#include "MiniATSubtarget.h"
#include "MiniATTargetObjectFile.h"
#include "llvm/PassManager.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "miniat"

extern "C" void LLVMInitializeMiniATTarget() {
    RegisterTargetMachine<MiniATStandardTargetMachine> X(TheMiniATTarget);
}


MiniATTargetMachine::
MiniATTargetMachine(const Target &T, StringRef TT,
                    StringRef CPU, StringRef FS, const TargetOptions &Options,
                    Reloc::Model RM, CodeModel::Model CM,
                    CodeGenOpt::Level OL)
        : LLVMTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL),
          TLOF(make_unique<MiniATTargetObjectFile>()),
          Subtarget(nullptr), DefaultSubtarget(TT, CPU, FS, RM, this) {
    Subtarget = &DefaultSubtarget;
    initAsmInfo();
}

MiniATTargetMachine::~MiniATTargetMachine() { }

void MiniATStandardTargetMachine::anchor() { }

MiniATStandardTargetMachine::
MiniATStandardTargetMachine(
        Target const &T, StringRef const &TT,
        StringRef const &CPU, StringRef const &FS,
        TargetOptions const &Options, Reloc::Model const &RM,
        CodeModel::Model const &CM, CodeGenOpt::Level const &OL)
        :
        MiniATTargetMachine(
                T, TT, CPU, FS,
                Options, RM, CM, OL) {

}

namespace {
//@MiniATPassConfig {
/// MiniAT Code Generator Pass Configuration Options.
    class MiniATPassConfig : public TargetPassConfig {
    public:
        MiniATPassConfig(MiniATTargetMachine *TM, PassManagerBase &PM)
                : TargetPassConfig(TM, PM) { }

        MiniATTargetMachine &getMiniATTargetMachine() const {
            return getTM<MiniATTargetMachine>();
        }

        const MiniATSubtarget &getMiniATSubtarget() const {
            return *getMiniATTargetMachine().getSubtargetImpl();
        }

        bool addInstSelector() override;

    };
} // namespace

TargetPassConfig *MiniATTargetMachine::createPassConfig(PassManagerBase &PM) {
    return new MiniATPassConfig(this, PM);
}


// Install an instruction selector pass using
// the ISelDag to gen MiniAT code.
bool MiniATPassConfig::addInstSelector() {
    addPass(createMiniATISelDag(getMiniATTargetMachine()));
    return false;
}

