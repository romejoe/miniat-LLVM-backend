//===-- MiniATSubtarget.cpp - MiniAT Subtarget Information --------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the MiniAT specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "MiniATMachineFunction.h"
#include "MiniAT.h"
#include "MiniATRegisterInfo.h"
#include "MiniATSubtarget.h"

#include "MiniATTargetMachine.h"
#include "llvm/IR/Attributes.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR

#include "MiniATGenSubtargetInfo.inc"

static cl::opt<bool> EnableMiniATS32Calls(
        "miniat-s32-calls"
        , cl::Hidden
        , cl::desc("MINIAT S32 call: use stack only to pass arguments.")
        , cl::init(true)
    );

extern bool FixGlobalBaseReg;

/// Select the MiniAT CPU for the given triple and cpu name.
/// FIXME: Merge with the copy in MiniATMCTargetDesc.cpp
static StringRef selectMiniATCPU(Triple TT, StringRef CPU) {
    if (CPU.empty() || CPU == "generic") {
        if (TT.getArch() == Triple::miniat)
            CPU = "miniat";
    }
    return CPU;
}

void MiniATSubtarget::anchor() {
}

static std::string computeDataLayout(const MiniATSubtarget &ST) {
    std::string Ret = "";

    // There are both little and big endian mips.
    //if (ST.isLittle())
    Ret += "e";
    //else
    //  Ret += "E";

    //Ret += "-m:m";

    Ret += "-p:32:32";

    // 8 and 16 bit integers only need no have natural alignment, but try to
    // align them to 32 bits.
    Ret += "-i8:8:32-i16:16:32";//-i64:64";

    // 32 bit registers are always available and the stack is at least 32 bit
    // aligned.
    Ret += "-n32-S32";

    return Ret;
}

//MiniATSubtarget::MiniATSubtarget {
MiniATSubtarget::MiniATSubtarget(const std::string &TT, const std::string &CPU,
        const std::string &FS,
        Reloc::Model _RM,
        MiniATTargetMachine *_TM) :
//MiniATSubtarget::MiniATSubtarget }
        MiniATGenSubtargetInfo(TT, CPU, FS),
        MiniATABI(StandardABI), RM(_RM), TM(_TM), TargetTriple(TT),
        DL(computeDataLayout(initializeSubtargetDependencies(CPU, FS, TM))),
        TSInfo(DL), InstrInfo(MiniATInstrInfo::create(*this)),
        FrameLowering(MiniATFrameLowering::create(*this)),
        TLInfo(MiniATTargetLowering::create(*TM, *this)) {

    //if (EnableMiniATS32Calls)
    //  MiniATABI = S32;
    //else
    //  MiniATABI = O32;
    // Assert exactly one ABI was chosen.
    assert(MiniATABI != UnknownABI);

}

MiniATSubtarget &
MiniATSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS,
        const TargetMachine *TM) {
    std::string CPUName = selectMiniATCPU(TargetTriple, CPU);

    if (CPUName == "help")
        CPUName = "miniat";

    if (CPUName == "miniat")
        MiniATArchVersion = MiniATStandard;
    //else if (CPUName == "miniat32II")
        //MiniATArchVersion = MiniAT32II;

//    if (isMiniAT32I()) {
//        HasCmp = true;
//        HasSlt = false;
//    }
//    else if (isMiniAT32II()) {
//        HasCmp = true;
//        HasSlt = true;
//    }
//    else {
//        errs() << "-mcpu must be empty(default:miniat32II), miniat32I or miniat32II" << "\n";
//    }

    // Parse features string.
    ParseSubtargetFeatures(CPUName, FS);
    // Initialize scheduling itinerary for the specified CPU.
    InstrItins = getInstrItineraryForCPU(CPUName);

    return *this;
}

