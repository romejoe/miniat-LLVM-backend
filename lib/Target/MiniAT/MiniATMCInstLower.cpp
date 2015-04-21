//===-- MiniATMCInstLower.cpp - Convert MiniAT MachineInstr to MCInst ---------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains code to lower MiniAT MachineInstrs to their corresponding
// MCInst records.
//
//===----------------------------------------------------------------------===//

#include "MiniATMCInstLower.h"

#include "MiniATAsmPrinter.h"
#include "MiniATInstrInfo.h"
#include "MCTargetDesc/MiniATBaseInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"

using namespace llvm;

MiniATMCInstLower::MiniATMCInstLower(MiniATAsmPrinter &asmprinter)
        : AsmPrinter(asmprinter) {}

void MiniATMCInstLower::Initialize(MCContext* C) {
    Ctx = C;
}

static void CreateMCInst(MCInst& Inst, unsigned Opc, const MCOperand& Opnd0,
        const MCOperand& Opnd1,
        const MCOperand& Opnd2 = MCOperand()) {
    Inst.setOpcode(Opc);
    Inst.addOperand(Opnd0);
    Inst.addOperand(Opnd1);
    if (Opnd2.isValid())
        Inst.addOperand(Opnd2);
}

//@LowerOperand {
MCOperand MiniATMCInstLower::LowerOperand(const MachineOperand& MO,
        unsigned offset) const {
    MachineOperandType MOTy = MO.getType();

    switch (MOTy) {
        default: llvm_unreachable("unknown operand type");
        case MachineOperand::MO_Register:
            // Ignore all implicit register operands.
            if (MO.isImplicit()) break;
            return MCOperand::CreateReg(MO.getReg());
        case MachineOperand::MO_Immediate:
            return MCOperand::CreateImm(MO.getImm() + offset);
        case MachineOperand::MO_RegisterMask:
            break;
        case MachineOperand::MO_MachineBasicBlock:
            return LowerSymbolOperand(MO, MOTy, offset);
    }

    return MCOperand();
}

void MiniATMCInstLower::Lower(const MachineInstr *MI, MCInst &OutMI) const {
    OutMI.setOpcode(MI->getOpcode());

    for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
        const MachineOperand &MO = MI->getOperand(i);
        MCOperand MCOp = LowerOperand(MO);

        if (MCOp.isValid())
            OutMI.addOperand(MCOp);
    }
}

MCOperand MiniATMCInstLower::LowerSymbolOperand(const MachineOperand &MO,
        MachineOperandType MOTy,
        unsigned Offset) const {
    MCSymbolRefExpr::VariantKind Kind;
    const MCSymbol *Symbol;

    /*switch(MO.getTargetFlags()) {
        default:                   llvm_unreachable("Invalid target flag!");
        case Cpu0II::MO_NO_FLAG:   Kind = MCSymbolRefExpr::VK_None; break;

// Cpu0_GPREL is for llc -march=cpu0 -relocation-model=static -cpu0-islinux-
//  format=false (global var in .sdata).
        case Cpu0II::MO_GPREL:     Kind = MCSymbolRefExpr::VK_Cpu0_GPREL; break;

#if CH >= CH9_1 //1
        case Cpu0II::MO_GOT_CALL:  Kind = MCSymbolRefExpr::VK_Cpu0_GOT_CALL; break;
#endif
        case Cpu0II::MO_GOT16:     Kind = MCSymbolRefExpr::VK_Cpu0_GOT16; break;
        case Cpu0II::MO_GOT:       Kind = MCSymbolRefExpr::VK_Cpu0_GOT; break;
// ABS_HI and ABS_LO is for llc -march=cpu0 -relocation-model=static (global
//  var in .data).
        case Cpu0II::MO_ABS_HI:    Kind = MCSymbolRefExpr::VK_Cpu0_ABS_HI; break;
        case Cpu0II::MO_ABS_LO:    Kind = MCSymbolRefExpr::VK_Cpu0_ABS_LO; break;
#if CH >= CH12_1
        case Cpu0II::MO_TLSGD:     Kind = MCSymbolRefExpr::VK_Cpu0_TLSGD; break;
        case Cpu0II::MO_TLSLDM:    Kind = MCSymbolRefExpr::VK_Cpu0_TLSLDM; break;
        case Cpu0II::MO_DTP_HI:    Kind = MCSymbolRefExpr::VK_Cpu0_DTP_HI; break;
        case Cpu0II::MO_DTP_LO:    Kind = MCSymbolRefExpr::VK_Cpu0_DTP_LO; break;
        case Cpu0II::MO_GOTTPREL:  Kind = MCSymbolRefExpr::VK_Cpu0_GOTTPREL; break;
        case Cpu0II::MO_TP_HI:     Kind = MCSymbolRefExpr::VK_Cpu0_TP_HI; break;
        case Cpu0II::MO_TP_LO:     Kind = MCSymbolRefExpr::VK_Cpu0_TP_LO; break;
#endif
        case Cpu0II::MO_GOT_HI16:  Kind = MCSymbolRefExpr::VK_Cpu0_GOT_HI16; break;
        case Cpu0II::MO_GOT_LO16:  Kind = MCSymbolRefExpr::VK_Cpu0_GOT_LO16; break;
    }*/

    switch (MOTy) {
        case MachineOperand::MO_GlobalAddress:
            Symbol = AsmPrinter.getSymbol(MO.getGlobal());
            break;


        case MachineOperand::MO_MachineBasicBlock:
            Symbol = MO.getMBB()->getSymbol();
            break;

        case MachineOperand::MO_BlockAddress:
            Symbol = AsmPrinter.GetBlockAddressSymbol(MO.getBlockAddress());
            Offset += MO.getOffset();
            break;

        case MachineOperand::MO_ExternalSymbol:
            Symbol = AsmPrinter.GetExternalSymbolSymbol(MO.getSymbolName());
            Offset += MO.getOffset();
            break;
        case MachineOperand::MO_JumpTableIndex:
            Symbol = AsmPrinter.GetJTISymbol(MO.getIndex());
            break;

        default:
            llvm_unreachable("<unknown operand type>");
    }

    const MCSymbolRefExpr *MCSym = MCSymbolRefExpr::Create(Symbol, Kind, *Ctx);

    if (!Offset)
        return MCOperand::CreateExpr(MCSym);

    // Assume offset is never negative.
    assert(Offset > 0);

    const MCConstantExpr *OffsetExpr =  MCConstantExpr::Create(Offset, *Ctx);
    const MCBinaryExpr *AddExpr = MCBinaryExpr::CreateAdd(MCSym, OffsetExpr, *Ctx);
    return MCOperand::CreateExpr(AddExpr);
}