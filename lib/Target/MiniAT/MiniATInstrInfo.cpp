//===-- MiniATInstrInfo.cpp - MiniAT Instruction Information ----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the MiniAT implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#include "MiniATInstrInfo.h"
#include "MiniAT.h"
#include "MiniATMachineFunctionInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

using namespace llvm;

#define GET_INSTRINFO_CTOR_DTOR
#include "MiniATGenInstrInfo.inc"

namespace llvm {
namespace MiniAT {

  // MiniAT Condition Codes
  enum CondCode {
    COND_TRUE,
    COND_FALSE,
    COND_INVALID
  };
}
}

// Pin the vtable to this file.
void MiniATInstrInfo::anchor() {}

MiniATInstrInfo::MiniATInstrInfo()
  : MiniATGenInstrInfo(MiniAT::ADJCALLSTACKDOWN, MiniAT::ADJCALLSTACKUP),
    RI() {
}

static bool isZeroImm(const MachineOperand &op) {
  return op.isImm() && op.getImm() == 0;
}

/// isLoadFromStackSlot - If the specified machine instruction is a direct
/// load from a stack slot, return the virtual or physical register number of
/// the destination along with the FrameIndex of the loaded stack slot.  If
/// not, return 0.  This predicate must return 0 if the instruction has
/// any side effects other than loading from the stack slot.
unsigned
MiniATInstrInfo::isLoadFromStackSlot(const MachineInstr *MI, int &FrameIndex) const{
  /*int Opcode = MI->getOpcode();
  if (Opcode == MiniAT::LDWFI) 
  {
    if ((MI->getOperand(1).isFI()) && // is a stack slot
        (MI->getOperand(2).isImm()) &&  // the imm is zero
        (isZeroImm(MI->getOperand(2)))) 
    {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }*/
  return 0;
}
  
  /// isStoreToStackSlot - If the specified machine instruction is a direct
  /// store to a stack slot, return the virtual or physical register number of
  /// the source reg along with the FrameIndex of the loaded stack slot.  If
  /// not, return 0.  This predicate must return 0 if the instruction has
  /// any side effects other than storing to the stack slot.
unsigned
MiniATInstrInfo::isStoreToStackSlot(const MachineInstr *MI,
                                   int &FrameIndex) const {
  /*int Opcode = MI->getOpcode();
  if (Opcode == MiniAT::STWFI)
  {
    if ((MI->getOperand(1).isFI()) && // is a stack slot
        (MI->getOperand(2).isImm()) &&  // the imm is zero
        (isZeroImm(MI->getOperand(2))))
    {
      FrameIndex = MI->getOperand(1).getIndex();
      return MI->getOperand(0).getReg();
    }
  }*/
  return 0;
}

//===----------------------------------------------------------------------===//
// Branch Analysis
//===----------------------------------------------------------------------===//

static inline bool IsBRU(unsigned BrOpc) {
  return BrOpc == MiniAT::BRFU_u6
      || BrOpc == MiniAT::BRFU_lu6
      || BrOpc == MiniAT::BRBU_u6
      || BrOpc == MiniAT::BRBU_lu6;
}

static inline bool IsBRT(unsigned BrOpc) {
  return BrOpc == MiniAT::BRFT_ru6
      || BrOpc == MiniAT::BRFT_lru6
      || BrOpc == MiniAT::BRBT_ru6
      || BrOpc == MiniAT::BRBT_lru6;
}

static inline bool IsBRF(unsigned BrOpc) {
  return BrOpc == MiniAT::BRFF_ru6
      || BrOpc == MiniAT::BRFF_lru6
      || BrOpc == MiniAT::BRBF_ru6
      || BrOpc == MiniAT::BRBF_lru6;
}

static inline bool IsCondBranch(unsigned BrOpc) {
  return IsBRF(BrOpc) || IsBRT(BrOpc);
}

static inline bool IsBR_JT(unsigned BrOpc) {
  return BrOpc == MiniAT::BR_JT
      || BrOpc == MiniAT::BR_JT32;
}

/// GetCondFromBranchOpc - Return the MiniAT CC that matches 
/// the correspondent Branch instruction opcode.
static MiniAT::CondCode GetCondFromBranchOpc(unsigned BrOpc) 
{
  if (IsBRT(BrOpc)) {
    return MiniAT::COND_TRUE;
  } else if (IsBRF(BrOpc)) {
    return MiniAT::COND_FALSE;
  } else {
    return MiniAT::COND_INVALID;
  }
}

/// GetCondBranchFromCond - Return the Branch instruction
/// opcode that matches the cc.
static inline unsigned GetCondBranchFromCond(MiniAT::CondCode CC) 
{
  switch (CC) {
  default: llvm_unreachable("Illegal condition code!");
  case MiniAT::COND_TRUE   : return MiniAT::BRFT_lru6;
  case MiniAT::COND_FALSE  : return MiniAT::BRFF_lru6;
  }
}

/// GetOppositeBranchCondition - Return the inverse of the specified 
/// condition, e.g. turning COND_E to COND_NE.
static inline MiniAT::CondCode GetOppositeBranchCondition(MiniAT::CondCode CC)
{
  switch (CC) {
  default: llvm_unreachable("Illegal condition code!");
  case MiniAT::COND_TRUE   : return MiniAT::COND_FALSE;
  case MiniAT::COND_FALSE  : return MiniAT::COND_TRUE;
  }
}

/// AnalyzeBranch - Analyze the branching code at the end of MBB, returning
/// true if it cannot be understood (e.g. it's a switch dispatch or isn't
/// implemented for a target).  Upon success, this returns false and returns
/// with the following information in various cases:
///
/// 1. If this block ends with no branches (it just falls through to its succ)
///    just return false, leaving TBB/FBB null.
/// 2. If this block ends with only an unconditional branch, it sets TBB to be
///    the destination block.
/// 3. If this block ends with an conditional branch and it falls through to
///    an successor block, it sets TBB to be the branch destination block and a
///    list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
/// 4. If this block ends with an conditional branch and an unconditional
///    block, it returns the 'true' destination in TBB, the 'false' destination
///    in FBB, and a list of operands that evaluate the condition. These
///    operands can be passed to other TargetInstrInfo methods to create new
///    branches.
///
/// Note that RemoveBranch and InsertBranch must be implemented to support
/// cases where this method returns success.
///
bool
MiniATInstrInfo::AnalyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                              MachineBasicBlock *&FBB,
                              SmallVectorImpl<MachineOperand> &Cond,
                              bool AllowModify) const {
  // If the block has no terminators, it just falls into the block after it.
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin())
    return false;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return false;
    --I;
  }
  if (!isUnpredicatedTerminator(I))
    return false;

  // Get the last instruction in the block.
  MachineInstr *LastInst = I;
  
  // If there is only one terminator instruction, process it.
  if (I == MBB.begin() || !isUnpredicatedTerminator(--I)) {
    if (IsBRU(LastInst->getOpcode())) {
      TBB = LastInst->getOperand(0).getMBB();
      return false;
    }
    
    MiniAT::CondCode BranchCode = GetCondFromBranchOpc(LastInst->getOpcode());
    if (BranchCode == MiniAT::COND_INVALID)
      return true;  // Can't handle indirect branch.
    
    // Conditional branch
    // Block ends with fall-through condbranch.

    TBB = LastInst->getOperand(1).getMBB();
    Cond.push_back(MachineOperand::CreateImm(BranchCode));
    Cond.push_back(LastInst->getOperand(0));
    return false;
  }
  
  // Get the instruction before it if it's a terminator.
  MachineInstr *SecondLastInst = I;

  // If there are three terminators, we don't know what sort of block this is.
  if (SecondLastInst && I != MBB.begin() &&
      isUnpredicatedTerminator(--I))
    return true;
  
  unsigned SecondLastOpc    = SecondLastInst->getOpcode();
  MiniAT::CondCode BranchCode = GetCondFromBranchOpc(SecondLastOpc);
  
  // If the block ends with conditional branch followed by unconditional,
  // handle it.
  if (BranchCode != MiniAT::COND_INVALID
    && IsBRU(LastInst->getOpcode())) {

    TBB = SecondLastInst->getOperand(1).getMBB();
    Cond.push_back(MachineOperand::CreateImm(BranchCode));
    Cond.push_back(SecondLastInst->getOperand(0));

    FBB = LastInst->getOperand(0).getMBB();
    return false;
  }
  
  // If the block ends with two unconditional branches, handle it.  The second
  // one is not executed, so remove it.
  if (IsBRU(SecondLastInst->getOpcode()) && 
      IsBRU(LastInst->getOpcode())) {
    TBB = SecondLastInst->getOperand(0).getMBB();
    I = LastInst;
    if (AllowModify)
      I->eraseFromParent();
    return false;
  }

  // Likewise if it ends with a branch table followed by an unconditional branch.
  if (IsBR_JT(SecondLastInst->getOpcode()) && IsBRU(LastInst->getOpcode())) {
    I = LastInst;
    if (AllowModify)
      I->eraseFromParent();
    return true;
  }

  // Otherwise, can't handle this.
  return true;
}

unsigned
MiniATInstrInfo::InsertBranch(MachineBasicBlock &MBB,MachineBasicBlock *TBB,
                             MachineBasicBlock *FBB,
                             const SmallVectorImpl<MachineOperand> &Cond,
                             DebugLoc DL)const{
  // Shouldn't be a fall through.
  assert(TBB && "InsertBranch must not be told to insert a fallthrough");
  assert((Cond.size() == 2 || Cond.size() == 0) &&
         "Unexpected number of components!");
  
  if (!FBB) { // One way branch.
    if (Cond.empty()) {
      // Unconditional branch
      BuildMI(&MBB, DL, get(MiniAT::BRFU_lu6)).addMBB(TBB);
    } else {
      // Conditional branch.
      unsigned Opc = GetCondBranchFromCond((MiniAT::CondCode)Cond[0].getImm());
      BuildMI(&MBB, DL, get(Opc)).addReg(Cond[1].getReg())
                             .addMBB(TBB);
    }
    return 1;
  }
  
  // Two-way Conditional branch.
  assert(Cond.size() == 2 && "Unexpected number of components!");
  unsigned Opc = GetCondBranchFromCond((MiniAT::CondCode)Cond[0].getImm());
  BuildMI(&MBB, DL, get(Opc)).addReg(Cond[1].getReg())
                         .addMBB(TBB);
  BuildMI(&MBB, DL, get(MiniAT::BRFU_lu6)).addMBB(FBB);
  return 2;
}

unsigned
MiniATInstrInfo::RemoveBranch(MachineBasicBlock &MBB) const {
  MachineBasicBlock::iterator I = MBB.end();
  if (I == MBB.begin()) return 0;
  --I;
  while (I->isDebugValue()) {
    if (I == MBB.begin())
      return 0;
    --I;
  }
  if (!IsBRU(I->getOpcode()) && !IsCondBranch(I->getOpcode()))
    return 0;
  
  // Remove the branch.
  I->eraseFromParent();
  
  I = MBB.end();

  if (I == MBB.begin()) return 1;
  --I;
  if (!IsCondBranch(I->getOpcode()))
    return 1;
  
  // Remove the branch.
  I->eraseFromParent();
  return 2;
}

void MiniATInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator I, DebugLoc DL,
                                 unsigned DestReg, unsigned SrcReg,
                                 bool KillSrc) const {
  bool GRDest = MiniAT::GRRegsRegClass.contains(DestReg);
  bool GRSrc  = MiniAT::GRRegsRegClass.contains(SrcReg);

  if (GRDest && GRSrc) {
    BuildMI(MBB, I, DL, get(MiniAT::ADD_2rus), DestReg)
      .addReg(SrcReg, getKillRegState(KillSrc))
      .addImm(0);
    return;
  }
  
  if (GRDest && SrcReg == MiniAT::SP) {
    BuildMI(MBB, I, DL, get(MiniAT::LDAWSP_ru6), DestReg).addImm(0);
    return;
  }

  if (DestReg == MiniAT::SP && GRSrc) {
    BuildMI(MBB, I, DL, get(MiniAT::SETSP_1r))
      .addReg(SrcReg, getKillRegState(KillSrc));
    return;
  }
  llvm_unreachable("Impossible reg-to-reg copy");
}

void MiniATInstrInfo::storeRegToStackSlot(MachineBasicBlock &MBB,
                                         MachineBasicBlock::iterator I,
                                         unsigned SrcReg, bool isKill,
                                         int FrameIndex,
                                         const TargetRegisterClass *RC,
                                         const TargetRegisterInfo *TRI) const
{
  DebugLoc DL;
  if (I != MBB.end() && !I->isDebugValue())
    DL = I->getDebugLoc();
  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = *MF->getFrameInfo();
  MachineMemOperand *MMO =
    MF->getMachineMemOperand(MachinePointerInfo::getFixedStack(FrameIndex),
                             MachineMemOperand::MOStore,
                             MFI.getObjectSize(FrameIndex),
                             MFI.getObjectAlignment(FrameIndex));
  BuildMI(MBB, I, DL, get(MiniAT::STWFI))
    .addReg(SrcReg, getKillRegState(isKill))
    .addFrameIndex(FrameIndex)
    .addImm(0)
    .addMemOperand(MMO);
}

void MiniATInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                          MachineBasicBlock::iterator I,
                                          unsigned DestReg, int FrameIndex,
                                          const TargetRegisterClass *RC,
                                          const TargetRegisterInfo *TRI) const
{
  DebugLoc DL;
  if (I != MBB.end() && !I->isDebugValue())
    DL = I->getDebugLoc();
  MachineFunction *MF = MBB.getParent();
  const MachineFrameInfo &MFI = *MF->getFrameInfo();
  MachineMemOperand *MMO =
    MF->getMachineMemOperand(MachinePointerInfo::getFixedStack(FrameIndex),
                             MachineMemOperand::MOLoad,
                             MFI.getObjectSize(FrameIndex),
                             MFI.getObjectAlignment(FrameIndex));
  BuildMI(MBB, I, DL, get(MiniAT::LDWFI), DestReg)
    .addFrameIndex(FrameIndex)
    .addImm(0)
    .addMemOperand(MMO);
}

/// ReverseBranchCondition - Return the inverse opcode of the 
/// specified Branch instruction.
bool MiniATInstrInfo::
ReverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const {
  assert((Cond.size() == 2) && 
          "Invalid MiniAT branch condition!");
  Cond[0].setImm(GetOppositeBranchCondition((MiniAT::CondCode)Cond[0].getImm()));
  return false;
}

static inline bool isImmU6(unsigned val) {
  return val < (1 << 6);
}

static inline bool isImmU16(unsigned val) {
  return val < (1 << 16);
}

static bool isImmMskBitp(unsigned val) {
  if (!isMask_32(val)) {
    return false;
  }
  int N = Log2_32(val) + 1;
  return (N >= 1 && N <= 8) || N == 16 || N == 24 || N == 32;
}

MachineBasicBlock::iterator MiniATInstrInfo::loadImmediate(
                                              MachineBasicBlock &MBB,
                                              MachineBasicBlock::iterator MI,
                                              unsigned Reg, uint64_t Value) const {
  DebugLoc dl;
  if (MI != MBB.end() && !MI->isDebugValue())
    dl = MI->getDebugLoc();
  if (isImmMskBitp(Value)) {
    int N = Log2_32(Value) + 1;
    return BuildMI(MBB, MI, dl, get(MiniAT::MKMSK_rus), Reg).addImm(N);
  }
  if (isImmU16(Value)) {
    int Opcode = isImmU6(Value) ? MiniAT::LDC_ru6 : MiniAT::LDC_lru6;
    return BuildMI(MBB, MI, dl, get(Opcode), Reg).addImm(Value);
  }
  MachineConstantPool *ConstantPool = MBB.getParent()->getConstantPool();
  const Constant *C = ConstantInt::get(
        Type::getInt32Ty(MBB.getParent()->getFunction()->getContext()), Value);
  unsigned Idx = ConstantPool->getConstantPoolIndex(C, 4);
  return BuildMI(MBB, MI, dl, get(MiniAT::LDWCP_lru6), Reg)
            .addConstantPoolIndex(Idx);
}
