//===-- MiniATAsmPrinter.h - MiniAT LLVM Assembly Printer ----------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MiniAT Assembly printer class.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATASMPRINTER_H
#define MINIATASMPRINTER_H

#include "MiniATMachineFunction.h"
#include "MiniATMCInstLower.h"
#include "MiniATSubtarget.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/Support/Compiler.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class MCStreamer;
class MachineInstr;
class MachineBasicBlock;
class Module;
class raw_ostream;

class LLVM_LIBRARY_VISIBILITY MiniATAsmPrinter : public AsmPrinter {

  void EmitInstrWithMacroNoAT(const MachineInstr *MI);

private:
#if CH >= CH9_1
  // tblgen'erated function.
  bool emitPseudoExpansionLowering(MCStreamer &OutStreamer,
                                   const MachineInstr *MI);
#endif

  // lowerOperand - Convert a MachineOperand into the equivalent MCOperand.
  bool lowerOperand(const MachineOperand &MO, MCOperand &MCOp);

public:

  const MiniATSubtarget *Subtarget;
  const MiniATFunctionInfo *MiniATFI;
  MiniATMCInstLower MCInstLowering;

  explicit MiniATAsmPrinter(TargetMachine &TM,  MCStreamer &Streamer)
    : AsmPrinter(TM, Streamer), MCInstLowering(*this) {
    Subtarget = &TM.getSubtarget<MiniATSubtarget>();
  }

  virtual const char *getPassName() const {
    return "MiniAT Assembly Printer";
  }

  virtual bool runOnMachineFunction(MachineFunction &MF);

//- EmitInstruction() must exists or will have run time error.
  void EmitInstruction(const MachineInstr *MI);
  void printSavedRegsBitmask(raw_ostream &O);
  void printHex32(unsigned int Value, raw_ostream &O);
  void emitFrameDirective();
  const char *getCurrentABIString() const;
  void EmitFunctionEntryLabel() override;
  void EmitFunctionBodyStart() override;
  void EmitFunctionBodyEnd() override;
#if CH >= CH11_2
  bool PrintAsmOperand(const MachineInstr *MI, unsigned OpNo,
                       unsigned AsmVariant, const char *ExtraCode,
                       raw_ostream &O);
  bool PrintAsmMemoryOperand(const MachineInstr *MI, unsigned OpNum,
                             unsigned AsmVariant, const char *ExtraCode,
                             raw_ostream &O);
  void printOperand(const MachineInstr *MI, int opNum, raw_ostream &O);
#endif
  void EmitStartOfAsmFile(Module &M);
  virtual MachineLocation getDebugValueLocation(const MachineInstr *MI) const;
  void PrintDebugValueComment(const MachineInstr *MI, raw_ostream &OS);
};
}



#endif

