//===-- MiniATInstPrinter.cpp - Convert MiniAT MCInst to assembly syntax ------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This class prints an MiniAT MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "MiniATInstPrinter.h"

#include "MiniATInstrInfo.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/raw_ostream.h"
using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#define PRINT_ALIAS_INSTR
#include "MiniATGenAsmWriter.inc"

/*MiniATInstPrinter::MiniATInstPrinter(MCAsmInfo const &mai, MCInstrInfo const &mii, MCRegisterInfo const &mri,)
        : MCInstPrinter(mai,mii,mri) {
}*/

void MiniATInstPrinter::printRegName(raw_ostream &OS, unsigned RegNo) const {
//- getRegisterName(RegNo) defined in MiniATGenAsmWriter.inc which came from 
//   MiniAT.td indicate.
  OS << StringRef(getRegisterName(RegNo)).lower();
}


void MiniATInstPrinter::printInst(const MCInst *MI, raw_ostream &OS, StringRef Annot) {
  // Try to print any aliases first.
  if (!printAliasInstr(MI, OS))
    printInstruction(MI, OS);

  printAnnotation(OS, Annot);
}

//@printExpr {
static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
//@printExpr body {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (const MCBinaryExpr *BE = dyn_cast<MCBinaryExpr>(Expr)) {
    SRE = dyn_cast<MCSymbolRefExpr>(BE->getLHS());
    const MCConstantExpr *CE = dyn_cast<MCConstantExpr>(BE->getRHS());
    assert(SRE && CE && "Binary expression must be sym+const.");
    Offset = CE->getValue();
  }
  else if (!(SRE = dyn_cast<MCSymbolRefExpr>(Expr)))
    assert(false && "Unexpected MCExpr type.");

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
    default:                                 llvm_unreachable("Invalid kind!");
    case MCSymbolRefExpr::VK_None:           break;
  }

  OS << SRE->getSymbol();

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }

  /*if ((Kind == MCSymbolRefExpr::VK_MiniAT_GPOFF_HI) ||
          (Kind == MCSymbolRefExpr::VK_MiniAT_GPOFF_LO))
    OS << ")))";
  else if (Kind != MCSymbolRefExpr::VK_None)
    OS << ')';
    */
}

void MiniATInstPrinter::printOperand(const MCInst *MI, unsigned OpNo, raw_ostream &O) {
  const MCOperand &Op = MI->getOperand(OpNo);
  if (Op.isReg()) {
    printRegName(O, Op.getReg());
    return;
  }

  if (Op.isImm()) {
    O << Op.getImm();
    return;
  }

  assert(Op.isExpr() && "unknown operand kind in printOperand");
  printExpr(Op.getExpr(), O);
}

void MiniATInstPrinter::printUnsignedImm(const MCInst *MI, int opNum, raw_ostream &O) {
  const MCOperand &MO = MI->getOperand(opNum);
  if (MO.isImm())
    O << (unsigned short int)MO.getImm();
  else
    printOperand(MI, opNum, O);
}

void MiniATInstPrinter::printMemOperand(const MCInst *MI, int opNum, raw_ostream &O) {
  // Load/Store memory operands -- imm($reg)
  // If PIC target the target is loaded as the
  // pattern ld $t9,%call16($gp)
  /*
  printOperand(MI, opNum+1, O);
  O << "(";
  printOperand(MI, opNum, O);
  O << ")";
   */
}