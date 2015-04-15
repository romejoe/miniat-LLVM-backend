//===-- MiniATStandardInstrInfo.cpp - MiniAT32/64 Instruction Information -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniATStandard implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATStandardInstrInfo.h"

#include "MiniATMachineFunction.h"
#include "MiniATTargetMachine.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

static MachineMemOperand* GetMemOperand(MachineBasicBlock &MBB, int FI,
        unsigned Flag) {
    MachineFunction &MF = *MBB.getParent();
    MachineFrameInfo &MFI = *MF.getFrameInfo();
    unsigned Align = MFI.getObjectAlignment(FI);

    return MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(FI), Flag,
                                   MFI.getObjectSize(FI), Align);
}

MiniATStandardInstrInfo::MiniATStandardInstrInfo(const MiniATSubtarget &STI)
    : MiniATInstrInfo(STI),
      RI(STI) {}

const MiniATRegisterInfo &MiniATStandardInstrInfo::getRegisterInfo() const {
  return RI;
}

const MiniATInstrInfo *llvm::createMiniATStandardInstrInfo(const MiniATSubtarget &STI) {
  return new MiniATStandardInstrInfo(STI);
}


void MiniATStandardInstrInfo::
storeRegToStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
        unsigned SrcReg, bool isKill, int FI,
        const TargetRegisterClass *RC, const TargetRegisterInfo *TRI,
        int64_t Offset) const {
    DebugLoc DL;
    if (I != MBB.end()) DL = I->getDebugLoc();
    MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOStore);

    unsigned Opc = 0;

    Opc = MiniAT::MSTORERI;
    assert(Opc && "Register class not handled!");
    BuildMI(MBB, I, DL, get(Opc)).addReg(SrcReg, getKillRegState(isKill))
            .addFrameIndex(FI).addImm(0).addMemOperand(MMO);
}

void MiniATStandardInstrInfo::
loadRegFromStack(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
        unsigned DestReg, int FI, const TargetRegisterClass *RC,
        const TargetRegisterInfo *TRI, int64_t Offset) const {
    DebugLoc DL;
    if (I != MBB.end()) DL = I->getDebugLoc();
    MachineMemOperand *MMO = GetMemOperand(MBB, FI, MachineMemOperand::MOLoad);
    unsigned Opc = 0;

    Opc = MiniAT::MLOADRI;
    assert(Opc && "Register class not handled!");
    BuildMI(MBB, I, DL, get(Opc), DestReg).addFrameIndex(FI).addImm(0)
            .addMemOperand(MMO);
}

// MiniATInstrInfo::expandPostRAPseudo
/// Expand Pseudo instructions into real backend instructions
bool MiniATStandardInstrInfo::expandPostRAPseudo(MachineBasicBlock::iterator MI) const {
    MachineBasicBlock &MBB = *MI->getParent();

    switch(MI->getDesc().getOpcode()) {
        default:
            return false;
        case MiniAT::PCall:
            ExpandPCall(MBB, MI, MiniAT::PCall);
            break;
        case MiniAT::PRet:
            ExpandPRet(MBB, MI, MiniAT::PRet);
            break;
    }

    MBB.erase(MI);
    return true;
}

/// Adjust SP by Amount bytes.
void MiniATStandardInstrInfo::adjustStackPtr(MiniATFunctionInfo *MiniATFI,
        unsigned SP,
        int64_t Amount, MachineBasicBlock &MBB,
        MachineBasicBlock::iterator I) const {
    const MiniATSubtarget &STI = Subtarget;
    DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();
//    unsigned ADDu = MiniAT::ADDu;
//    unsigned ADDiu = MiniAT::ADDiu;

    if (isInt<32>(Amount))// addri sp, sp, amount
        BuildMI(MBB, I, DL, get(MiniAT::ADDRI), MiniAT::r254).addReg(MiniAT::r254).addImm(Amount);
    else { // Expand immediate that doesn't fit in 16-bit.
        //MiniATFI->setEmitNOAT();
        //unsigned Reg = loadImmediate(Amount, MBB, I, DL, nullptr);
        //BuildMI(MBB, I, DL, get(ADDu), SP).addReg(SP).addReg(Reg);

        //no go lets see what happens
    }
}

/// This function generates the sequence of instructions needed to get the
/// result of adding register REG and immediate IMM.
/*unsigned
MiniATStandardInstrInfo::loadImmediate(int64_t Imm, MachineBasicBlock &MBB,
        MachineBasicBlock::iterator II, DebugLoc DL,
        unsigned *NewImm) const {
    MiniATAnalyzeImmediate AnalyzeImm;
    const MiniATSubtarget &STI = Subtarget;
    MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
    unsigned Size = 32;
    unsigned LUi = MiniAT::LUi;
    unsigned ZEROReg = MiniAT::ZERO;
    unsigned ATReg = MiniAT::AT;
    const TargetRegisterClass *RC = &MiniAT::CPURegsRegClass;
    bool LastInstrIsADDiu = NewImm;

    const MiniATAnalyzeImmediate::InstSeq &Seq =
            AnalyzeImm.Analyze(Imm, Size, LastInstrIsADDiu);
    MiniATAnalyzeImmediate::InstSeq::const_iterator Inst = Seq.begin();

    assert(Seq.size() && (!LastInstrIsADDiu || (Seq.size() > 1)));

    // The first instruction can be a LUi, which is different from other
    // instructions (ADDiu, ORI and SLL) in that it does not have a register
    // operand.
    if (Inst->Opc == LUi)
        BuildMI(MBB, II, DL, get(LUi), ATReg).addImm(SignExtend64<16>(Inst->ImmOpnd));
    else
        BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ZEROReg)
                .addImm(SignExtend64<16>(Inst->ImmOpnd));

    // Build the remaining instructions in Seq.
    for (++Inst; Inst != Seq.end() - LastInstrIsADDiu; ++Inst)
        BuildMI(MBB, II, DL, get(Inst->Opc), ATReg).addReg(ATReg)
                .addImm(SignExtend64<16>(Inst->ImmOpnd));

    if (LastInstrIsADDiu)
        *NewImm = Inst->ImmOpnd;

    return ATReg;
}*/

void MiniATStandardInstrInfo::ExpandPRet(
        MachineBasicBlock &MBB
        , MachineBasicBlock::iterator I
        , unsigned Opc
) const {


    //decrement stack pointer
    BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::SUBRI)).addReg(MiniAT::r254).addReg(MiniAT::r254).addImm(4);

    //load return register with correct return address
    //BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::MLOADRI)).addReg(MiniAT::r252).addReg(MiniAT::r254).addImm(0);

    BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::BRARI)).addReg(MiniAT::r254).addImm(0);
    //BuildMI(MBB, I, MiniAT::MOVR)

    //BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::))
    //BuildMI(MBB, I, I->getDebugLoc(), get(Opc)).addReg(MiniAT::r252);
}

void MiniATStandardInstrInfo::ExpandPCall(
        MachineBasicBlock &MBB
        , MachineBasicBlock::iterator I
        , unsigned Opc
) const {
    BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::ADDRI)).addReg(MiniAT::r254).addReg(MiniAT::r254).addImm(4);
    BuildMI(MBB, I, I->getDebugLoc(), get(MiniAT::BRARI)).addReg(MiniAT::r252).addImm(0);

}
