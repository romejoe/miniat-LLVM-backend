//===-- MiniATAsmPrinter.cpp - MiniAT LLVM Assembly Printer -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format MINIAT assembly language.
//
//===----------------------------------------------------------------------===//

#include "MiniATAsmPrinter.h"


#include "InstPrinter/MiniATInstPrinter.h"
#include "MCTargetDesc/MiniATBaseInfo.h"
#include "MiniAT.h"
#include "MiniATInstrInfo.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/Twine.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineFunctionPass.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstr.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Mangler.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetLoweringObjectFile.h"
#include "llvm/Target/TargetOptions.h"

using namespace llvm;

#define DEBUG_TYPE "miniat-asm-printer"


bool MiniATAsmPrinter::runOnMachineFunction(MachineFunction &MF) {
    MiniATFI = MF.getInfo<MiniATFunctionInfo>();
    AsmPrinter::runOnMachineFunction(MF);
    return true;
}

#if CH >= CH9_1
bool MiniATAsmPrinter::lowerOperand(const MachineOperand &MO, MCOperand &MCOp) {
    MCOp = MCInstLowering.LowerOperand(MO);
    return MCOp.isValid();
}

#include "MiniATGenMCPseudoLowering.inc"
#endif

//@EmitInstruction {
//- EmitInstruction() must exists or will have run time error.
void MiniATAsmPrinter::EmitInstruction(const MachineInstr *MI) {
//@EmitInstruction body {
    /*if (MI->isDebugValue()) {
        SmallString<128> Str;
        raw_svector_ostream OS(Str);

        PrintDebugValueComment(MI, OS);
        return;
    }*/
/*
#if CH >= CH9_3 //2
#ifdef ENABLE_GPRESTORE
  unsigned Opc = MI->getOpcode();
  SmallVector<MCInst, 4> MCInsts;

  switch (Opc) {
  case MiniAT::CPRESTORE: {
    const MachineOperand &MO = MI->getOperand(0);
    assert(MO.isImm() && "CPRESTORE's operand must be an immediate.");
    int64_t Offset = MO.getImm();

    if (OutStreamer.hasRawTextSupport()) {
      if (!isInt<16>(Offset)) {
        EmitInstrWithMacroNoAT(MI);
        return;
      }
    } else {
      MCInstLowering.LowerCPRESTORE(Offset, MCInsts);

      for (SmallVector<MCInst, 4>::iterator I = MCInsts.begin();
           I != MCInsts.end(); ++I)
        OutStreamer.EmitInstruction(*I, getSubtargetInfo());

      return;
    }

    break;
  }
  default:
    break;
  }
#endif
#endif //#if CH >= CH9_3 //2
*/
    MachineBasicBlock::const_instr_iterator I = MI;
    MachineBasicBlock::const_instr_iterator E = MI->getParent()->instr_end();

    MCInst TmpInst0;
    do {
//#if CH >= CH9_1
        // Do any auto-generated pseudo lowerings.
//        if (emitPseudoExpansionLowering(OutStreamer, &*I))
//            continue;
//#endif //#if CH >= CH9_1
        MCInstLowering.Lower(I, TmpInst0);
        OutStreamer.EmitInstruction(TmpInst0, getSubtargetInfo());
    } while ((++I != E) && I->isInsideBundle()); // Delay slot check
}
//@EmitInstruction }

//===----------------------------------------------------------------------===//
//
//  MiniAT Asm Directives
//
//  -- Frame directive "frame Stackpointer, Stacksize, RARegister"
//  Describe the stack frame.
//
//  -- Mask directives "(f)mask  bitmask, offset"
//  Tells the assembler which registers are saved and where.
//  bitmask - contain a little endian bitset indicating which registers are
//            saved on function prologue (e.g. with a 0x80000000 mask, the
//            assembler knows the register 31 (RA) is saved at prologue.
//  offset  - the position before stack pointer subtraction indicating where
//            the first saved register on prologue is located. (e.g. with a
//
//  Consider the following function prologue:
//
//    .frame  $fp,48,$ra
//    .mask   0xc0000000,-8
//       addiu $sp, $sp, -48
//       st $ra, 40($sp)
//       st $fp, 36($sp)
//
//    With a 0xc0000000 mask, the assembler knows the register 31 (RA) and
//    30 (FP) are saved at prologue. As the save order on prologue is from
//    left to right, RA is saved first. A -8 offset means that after the
//    stack pointer subtration, the first register in the mask (RA) will be
//    saved at address 48-8=40.
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Mask directives
//===----------------------------------------------------------------------===//
//	.frame	$sp,8,$lr
//->	.mask 	0x00000000,0
//	.set	noreorder
//	.set	nomacro

// Create a bitmask with all callee saved registers for CPU.
// For CPU registers consider LR, GP and FP for saving if necessary.
void MiniATAsmPrinter::printSavedRegsBitmask(raw_ostream &O) {
    // CPU and FPU Saved Registers Bitmasks
    unsigned CPUBitmask = 0;
    int CPUTopSavedRegOff;

    // Set the CPU and FPU Bitmasks
    const MachineFrameInfo *MFI = MF->getFrameInfo();
    const std::vector<CalleeSavedInfo> &CSI = MFI->getCalleeSavedInfo();
    // size of stack area to which FP callee-saved regs are saved.
    unsigned CPURegSize = MiniAT::GPRRegClass.getSize();
    unsigned i = 0, e = CSI.size();

    // Set CPU Bitmask.
    for (; i != e; ++i) {
        unsigned Reg = CSI[i].getReg();
        unsigned RegNum = getMiniATRegisterNumbering(Reg);
        CPUBitmask |= (1 << RegNum);
    }

    CPUTopSavedRegOff = CPUBitmask ? -CPURegSize : 0;

    // Print CPUBitmask
    O << "\t.mask \t"; printHex32(CPUBitmask, O);
    O << ',' << CPUTopSavedRegOff << '\n';
}

// Print a 32 bit hex number with all numbers.
void MiniATAsmPrinter::printHex32(unsigned Value, raw_ostream &O) {
    O << "0x";
    for (int i = 7; i >= 0; i--)
        O.write_hex((Value & (0xF << (i*4))) >> (i*4));
}

//===----------------------------------------------------------------------===//
// Frame and Set directives
//===----------------------------------------------------------------------===//
//->	.frame	$sp,8,$lr
//	.mask 	0x00000000,0
//	.set	noreorder
//	.set	nomacro
/// Frame Directive
void MiniATAsmPrinter::emitFrameDirective() {
    const TargetRegisterInfo &RI = *TM.getSubtargetImpl()->getRegisterInfo();

    unsigned stackReg  = RI.getFrameRegister(*MF);
    unsigned returnReg = RI.getRARegister();
    unsigned stackSize = MF->getFrameInfo()->getStackSize();

    if (OutStreamer.hasRawTextSupport())
        OutStreamer.EmitRawText("\t.frame\t$" +
                StringRef(MiniATInstPrinter::getRegisterName(stackReg)).lower() +
                "," + Twine(stackSize) + ",$" +
                StringRef(MiniATInstPrinter::getRegisterName(returnReg)).lower());
}

/// Emit Set directives.
const char *MiniATAsmPrinter::getCurrentABIString() const {
    return "std";
    /*switch (Subtarget->getTargetABI()) {
        case MiniATSubtarget::S32:  return "abiS32";
        case MiniATSubtarget::O32:  return "abi32";
        default: llvm_unreachable("Unknown MiniAT ABI");;
    }*/
}

//		.type	main,@function
//->		.ent	main                    # @main
//	main:
void MiniATAsmPrinter::EmitFunctionEntryLabel() {
    if (OutStreamer.hasRawTextSupport()) {
        OutStreamer.EmitRawText("!" + Twine(CurrentFnSym->getName()));
        //OutStreamer.EmitRawText(CurrentFnSym->getName());
        //OutStreamer.EmitRawText("\n"); //TODO:platform independent newline
        //OutStreamer.EmitLabel(CurrentFnSym);
    }
//        OutStreamer.EmitRawText("\t.ent\t" + Twine(CurrentFnSym->getName()));


}



/// EmitFunctionBodyStart - Targets can override this to emit stuff before
/// the first basic block in the function.
void MiniATAsmPrinter::EmitFunctionBodyStart() {
    MCInstLowering.Initialize(&MF->getContext());

}

/// EmitFunctionBodyEnd - Targets can override this to emit stuff after
/// the last basic block in the function.
void MiniATAsmPrinter::EmitFunctionBodyEnd() {
    // There are instruction for this macros, but they must
    // always be at the function end, and we can't emit and
    // break with BB logic.
    
    //TODO: emit exit frame
}

//preamble
void MiniATAsmPrinter::EmitStartOfAsmFile(Module &M) {
    //TODO: output variables?
    //TODO: output constants?
    //TODO: output symbols?
    for (const auto &G : M.globals()){
        if(G.isConstant()){
            emitGlobalConstant(G);
        }
        else {
            emitGlobalVariable(G);
        }
    }

/*    Module::GlobalListType &globals = M.getGlobalList();

    for(auto it = globals.begin(); it != globals.end(); it++){
        GlobalVariable *var = M.getGlobalVariable(it->getName(), true);
        std::string output = "";
        if(var->isConstant()){
            output += ".constant ";
        }
        else{
            output += ".variable ";
        }
        output += var->getName();

        if(var->isConstant()){
            output += "#todo";
        }

        if(var->getInitializer()){
            Constant *init = var->getInitializer();
            init = NULL;
        }


        OutStreamer.EmitRawText(Twine(output));
    }
    */

    //if (OutStreamer.hasRawTextSupport())
    //    OutStreamer.EmitRawText("\t.mode NO_SIMPLE_VARS");



    if (OutStreamer.hasRawTextSupport()){
        OutStreamer.EmitRawText(StringRef("\t##################################"));
        OutStreamer.EmitRawText(StringRef("\t# ...so ends the preamble"));
        OutStreamer.EmitRawText(StringRef("\t# ...thus beginith the meat"));
        OutStreamer.EmitRawText(StringRef("\t##################################"));
        OutStreamer.EmitRawText(StringRef("\n"));

        for (const auto &G : M.globals()){
            if(!G.isConstant()){
                emitGlobalVariableValue(G);
            }
        }

        OutStreamer.EmitRawText(StringRef("MOVI r253, !StackStart \t #Initialize stack\n\n"));
    }

}

void MiniATAsmPrinter::EmitEndOfAsmFile(Module &M) {
    //TODO: output variables?
    //TODO: output constants?
    //TODO: output symbols?

    //if (OutStreamer.hasRawTextSupport())
    //    OutStreamer.EmitRawText("\t.mode NO_SIMPLE_VARS");



    if (OutStreamer.hasRawTextSupport()){
        OutStreamer.EmitRawText(StringRef("\t##################################"));
        OutStreamer.EmitRawText(StringRef("\t# ... so beginith the stack"));
        OutStreamer.EmitRawText(StringRef("\t##################################"));
        OutStreamer.EmitRawText(StringRef("\t!StackStart"));

    }

}

/*
#if CH >= CH11_2
// Print out an operand for an inline asm expression.
bool MiniATAsmPrinter::PrintAsmOperand(const MachineInstr *MI, unsigned OpNum,
        unsigned AsmVariant,const char *ExtraCode,
        raw_ostream &O) {
    // Does this asm operand have a single letter operand modifier?
    if (ExtraCode && ExtraCode[0]) {
        if (ExtraCode[1] != 0) return true; // Unknown modifier.

        const MachineOperand &MO = MI->getOperand(OpNum);
        switch (ExtraCode[0]) {
            default:
                // See if this is a generic print operand
                return AsmPrinter::PrintAsmOperand(MI,OpNum,AsmVariant,ExtraCode,O);
            case 'X': // hex const int
                if ((MO.getType()) != MachineOperand::MO_Immediate)
                    return true;
                O << "0x" << StringRef(utohexstr(MO.getImm())).lower();
                return false;
            case 'x': // hex const int (low 16 bits)
                if ((MO.getType()) != MachineOperand::MO_Immediate)
                    return true;
                O << "0x" << StringRef(utohexstr(MO.getImm() & 0xffff)).lower();
                return false;
            case 'd': // decimal const int
                if ((MO.getType()) != MachineOperand::MO_Immediate)
                    return true;
                O << MO.getImm();
                return false;
            case 'm': // decimal const int minus 1
                if ((MO.getType()) != MachineOperand::MO_Immediate)
                    return true;
                O << MO.getImm() - 1;
                return false;
            case 'z': {
                // $0 if zero, regular printing otherwise
                if (MO.getType() != MachineOperand::MO_Immediate)
                    return true;
                int64_t Val = MO.getImm();
                if (Val)
                    O << Val;
                else
                    O << "$0";
                return false;
            }
        }
    }

    printOperand(MI, OpNum, O);
    return false;
}

bool MiniATAsmPrinter::PrintAsmMemoryOperand(const MachineInstr *MI,
        unsigned OpNum, unsigned AsmVariant,
        const char *ExtraCode,
        raw_ostream &O) {
    int Offset = 0;
    // Currently we are expecting either no ExtraCode or 'D'
    if (ExtraCode) {
        return true; // Unknown modifier.
    }

    const MachineOperand &MO = MI->getOperand(OpNum);
    assert(MO.isReg() && "unexpected inline asm memory operand");
    O << Offset << "($" << MiniATInstPrinter::getRegisterName(MO.getReg()) << ")";

    return false;
}

void MiniATAsmPrinter::printOperand(const MachineInstr *MI, int opNum,
        raw_ostream &O) {
    const MachineOperand &MO = MI->getOperand(opNum);
    bool closeP = false;

    if (MO.getTargetFlags())
        closeP = true;

    switch(MO.getTargetFlags()) {
        case MiniATII::MO_GPREL:    O << "%gp_rel("; break;
        case MiniATII::MO_GOT_CALL: O << "%call16("; break;
        case MiniATII::MO_GOT16:    O << "%got16(";  break;
        case MiniATII::MO_GOT:      O << "%got(";    break;
        case MiniATII::MO_ABS_HI:   O << "%hi(";     break;
        case MiniATII::MO_ABS_LO:   O << "%lo(";     break;
        case MiniATII::MO_GOT_HI16: O << "%got_hi16("; break;
        case MiniATII::MO_GOT_LO16: O << "%got_lo16("; break;
    }

    switch (MO.getType()) {
        case MachineOperand::MO_Register:
            O << '$'
                    << StringRef(MiniATInstPrinter::getRegisterName(MO.getReg())).lower();
            break;

        case MachineOperand::MO_Immediate:
            O << MO.getImm();
            break;

        case MachineOperand::MO_MachineBasicBlock:
            O << *MO.getMBB()->getSymbol();
            return;

        case MachineOperand::MO_GlobalAddress:
            O << *getSymbol(MO.getGlobal());
            break;

        case MachineOperand::MO_BlockAddress: {
            MCSymbol *BA = GetBlockAddressSymbol(MO.getBlockAddress());
            O << BA->getName();
            break;
        }

        case MachineOperand::MO_ExternalSymbol:
            O << *GetExternalSymbolSymbol(MO.getSymbolName());
            break;

        case MachineOperand::MO_JumpTableIndex:
            O << MAI->getPrivateGlobalPrefix() << "JTI" << getFunctionNumber()
                    << '_' << MO.getIndex();
            break;

        case MachineOperand::MO_ConstantPoolIndex:
            O << MAI->getPrivateGlobalPrefix() << "CPI"
                    << getFunctionNumber() << "_" << MO.getIndex();
            if (MO.getOffset())
                O << "+" << MO.getOffset();
            break;

        default:
            llvm_unreachable("<unknown operand type>");
    }

    if (closeP) O << ")";
}
#endif // #if CH >= CH11_2
 */
/*
MachineLocation
MiniATAsmPrinter::getDebugValueLocation(const MachineInstr *MI) const {
    // Handles frame addresses emitted in MiniATInstrInfo::emitFrameIndexDebugValue.
    assert(MI->getNumOperands() == 4 && "Invalid no. of machine operands!");
    assert(MI->getOperand(0).isReg() && MI->getOperand(1).isImm() &&
            "Unexpected MachineOperand types");
    return MachineLocation(MI->getOperand(0).getReg(),
            MI->getOperand(1).getImm());
}

void MiniATAsmPrinter::PrintDebugValueComment(const MachineInstr *MI,
        raw_ostream &OS) {
    // TODO: implement
    OS << "PrintDebugValueComment()";
}
*/

// Force static initialization.
extern "C" void LLVMInitializeMiniATAsmPrinter() {
    RegisterAsmPrinter<MiniATAsmPrinter> X(TheMiniATTarget);
}
/*
void MiniATAsmPrinter::EmitBasicBlockStart(const MachineBasicBlock &MBB) const {
//    AsmPrinter::EmitBasicBlockStart(MBB);
    StringRef ParentName = MBB.getParent()->getName();
    StringRef name = MBB.getSymbol()->getName();
    if(name.startswith("\"")){
        //remove quotes
        name = name.substr(1, name.size() - 2);
    }
    if(name.startswith("!")){
        //remove quotes
        name = name.substr(1, name.size() - 1);
    }
    //OutStreamer.EmitRawText("!" + ParentName + "_" + name+":");
    OutStreamer.EmitRawText("!" + name+":");
    //OutStreamer.EmitLabel(MBB.getSymbol());
}
*/
void MiniATAsmPrinter::EmitFunctionHeader() {
    AsmPrinter::EmitFunctionHeader();
}

void MiniATAsmPrinter::emitGlobalVariable(const GlobalVariable &GV) {

    Twine output = Twine(".variable ") + GV.getName();
    OutStreamer.EmitRawText(output);
}
void MiniATAsmPrinter::emitGlobalVariableValue(const GlobalVariable &GV) {
    if(!GV.hasInitializer()) return;

    std::string value = "";
    auto init = GV.getInitializer();


    const DataLayout *DL = TM.getSubtargetImpl()->getDataLayout();
    uint64_t Size = DL->getTypeAllocSize(GV.getType());
    if (const ConstantInt *CI = dyn_cast<ConstantInt>(init)) {
        switch (Size) {
            case 1:
            case 2:
            case 4:
            case 8:
                if (isVerbose())
                    OutStreamer.GetCommentOS() << format("0x%" PRIx64 "\n",
                                                         CI->getSExtValue());

                value = std::to_string(CI->getSExtValue());
                //OutStreamer.EmitIntValue(CI->getZExtValue(), Size);
                //return;
                break;
            default:
                return;
        }
    }
    Twine output = Twine("\tMOVI ") + GV.getName() + Twine(", " + value);
    OutStreamer.EmitRawText(output);

    //Twine output = Twine("\tMOVI ") + GV.getName() + Twine(", 0\t#todo");

    //OutStreamer.EmitRawText(output);
}

void MiniATAsmPrinter::emitGlobalConstant(const GlobalVariable &CV) {
    if(CV.getNumOperands() != 1) return;
  //  APInt tmp;

//    CV.getIntegerValue(CV.getType(),tmp);
    std::string value = "";// = Twine("");
    //Twine output = Twine("\t#.constant ") + CV.getName() + Twine(" <");// /*+ Twine(std::to_string(tmp.getSExtValue()))*/ + Twine(">");
    //OutStreamer.EmitRawText(output);
    auto init = CV.getInitializer();

    const DataLayout *DL = TM.getSubtargetImpl()->getDataLayout();
    uint64_t Size = DL->getTypeAllocSize(CV.getType());
    if (const ConstantInt *CI = dyn_cast<ConstantInt>(init)) {
        switch (Size) {
            case 1:
            case 2:
            case 4:
            case 8:
                if (isVerbose())
                    OutStreamer.GetCommentOS() << format("0x%" PRIx64 "\n",
                                                            CI->getSExtValue());

                value = std::to_string(CI->getSExtValue());
                //OutStreamer.EmitIntValue(CI->getZExtValue(), Size);
                //return;
                break;
            default:
                return;
        }
    }
    Twine output = Twine("\t.constant ") + CV.getName() + Twine(" ") + Twine(value);// + /*+ Twine(std::to_string(tmp.getSExtValue()))*/ + Twine("");
    OutStreamer.EmitRawText(output);



}

void MiniATAsmPrinter::EmitGlobalVariable(const GlobalVariable *GV) {
   // AsmPrinter::EmitGlobalVariable(GV);
}

void MiniATAsmPrinter::EmitGlobalConstant(const Constant *CV) {
    //AsmPrinter::EmitGlobalConstant(CV);
}
