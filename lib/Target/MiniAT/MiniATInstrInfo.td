
include "MiniATInstrFormats.td"

//arithmetic Instructions
defm ADD:ArithLogicInst 	<0x00	,"ADD"	,add,	1>;
defm SUB:ArithLogicInst 	<0x01	,"SUB"	,sub,	0>;
defm MULT:ArithLogicInst 	<0x02	,"MULT"	,mul,	1>;
defm DIV:ArithLogicInst 	<0x03	,"DIV"	,sdiv,	0>;
defm MOD:ArithLogicInst 	<0x04	,"MOD"	,srem,	0>;

//logic Instructions
defm AND:ArithLogicInst 	<0x09	,"AND"	,and,	1>;
defm OR:ArithLogicInst  	<0x0A	,"OR"	,or,	1>;
defm EXOR:ArithLogicInst 	<0x0B	,"EXOR"	,xor,	1>;
defm SHL:ArithLogicInst 	<0x0C	,"SHL"	,rotl,	0>;//check
defm SHR:ArithLogicInst 	<0x0D	,"SHR"	,rotr,	0>;//check



//data Instructions
defm MLOAD:LoadInstr 	<0x10>;
defm MSTORE:StoreInstr	<0x11>;

//defm LOAD     : LoadM32<0x10,  "LOAD",  load_a>;
//defm STORE     : StoreM32<0x11, "STORE",  store_a>;


//defm RLOAD:RLoadInstr;
//defm RSTORE:RStoreInstr;



//control Instructions
defm BRAE:MiniATBranchInstr  	<0x16,	"BRAE", 	seteq,	SETEQ>;
defm BRANE:MiniATBranchInstr 	<0x17,	"BRANE", 	setne,	SETNE>;
defm BRAL:MiniATBranchInstr  	<0x18,	"BRAL", 	setlt,	SETLT>;
defm BRALE:MiniATBranchInstr 	<0x19,	"BRALE", 	setle,	SETLE>;
defm BRAG:MiniATBranchInstr  	<0x1A,	"BRAG", 	setgt,	SETGT>;
defm BRAGE:MiniATBranchInstr 	<0x1B,	"BRAGE", 	setge,	SETGE>;


//Pseudo Instructions
//Arithmetic
//def NEG:MiniATInstr<
//	(outs MiniATReg:$RA)
//	, (ins MiniATReg:$RC)
//	, "NEG $RA, $RC"
//	, []>;

//Logic
//def INV:MiniATInstr<
	//(outs MiniATReg:$RA)
	//, (ins MiniATReg:$RC)
	//, "INV $RA, $RC"
	//, []>;

//Data

///* MOVR */	def MOVR:Pat<(set GPR:$RA, GPR:$RC), (ADDRR GPR:$RA, r0, GPR:$RC)>;
def MOVR:InstR		<0x20, "MOVR \t $RA, $RC", [(set GPR:$RA, GPR:$RC)], IIAlu>;
def MOVI:InstI		<0x21, "MOVI \t $RA, $imm", [(set GPR:$RA, immSExt32:$imm)], IIAlu>;
///* MOVI */	def :Pat<(set GPR:$RA, i32imm:$imm), (set GPR:$RA, (add r0, i32imm:$imm))>;
///* NOP */	def :Pat<(), (set r0, (add r0, r0))>;


def NOP:MiniATInst<0,(outs),(ins), "nop", [], IIAlu>;

///* Flush */	def :Pat<(), (br r255)>;

//Control
//Unconditional Branches

def BRARImem:MiniATInst<
	0x22
	, (outs)
	//, (ins GPR:$RC, i32imm:$imm)
	, (ins brtarget:$dst)
	//, "BRA \t [$RC + $imm]"
	, "BRA \t [$dst]"
	, [/*(brind (add GPR:$RC, immSExt32:$imm)),*/(br bb:$dst)]
	, IIBranch
>;

def BRARI:MiniATInst<
	0x22
	, (outs)
	, (ins GPR:$RC, i32imm:$imm)
	, "BRA \t [$RC + $imm]"
	, [(brind (add GPR:$RC, immSExt32:$imm))]
	, IIBranch
>;


/* Pseudo Ret */
//def PRet:MiniATInst<0xFE, (outs ), (ins ), "PRET", [(ret)]>;


//def PCall:Pattern<(ret),
//	[(STORE GPR:r255, GPR:r254, i32:4), (LOAD]>;
//SDNPSideEffect

def pcall: SDNode<"MiniATISD::PCall", SDTNone,[SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

//def PCall:MiniATInst<0x25, (outs), (ins brtarget:$dst), "PCALL", [(pcall bb:$dst)], IIBranch>{
def PCall:MiniATInst<0x25, (outs), (ins), "PCALL", [(pcall)], IIBranch>{
	let isPseudo=1;
    	let isCodeGenOnly=1;
    	//let isTerminator=1;
    	let hasCtrlDep=1;
}

def pret: SDNode<"MiniATISD::PRet", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def PRet:MiniATInst<0x26, (outs), (ins), "PRET", [(pret)], IIBranch>{
	let isReturn=1;
	let isPseudo=1;
	let isCodeGenOnly=1;
	let isTerminator=1;
	let hasDelaySlot=1;
	let isBarrier=1;
	let hasCtrlDep=1;
	let hasExtraSrcRegAllocReq = 1;

	//let isCodeGenOnly=1;
    //let hasCtrlDep=1;
}

//def Pseteq: SDNode<"MiniATISD::PSetEQ", SDTIntBinOp, [SDNPOptInGlue]>;
//def Psetne: SDNode<"MiniATISD::PSetNE", SDTIntBinOp, [SDNPOptInGlue]>;
//def Psetlt: SDNode<"MiniATISD::PSetLT", SDTIntBinOp, [SDNPOptInGlue]>;
//def Psetle: SDNode<"MiniATISD::PSetLE", SDTIntBinOp, [SDNPOptInGlue]>;
//def Psetgt: SDNode<"MiniATISD::PSetGT", SDTIntBinOp, [SDNPOptInGlue]>;
//def Psetge: SDNode<"MiniATISD::PSetGE", SDTIntBinOp, [SDNPOptInGlue]>;

let isPseudo=1 in {
def PSetEQ:MiniATInst<0x27, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetEQ", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETEQ))], IIAlu>;
def PSetNE:MiniATInst<0x28, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetNE", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETNE))], IIAlu>;
def PSetLT:MiniATInst<0x29, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetLT", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETLT))], IIAlu>;
def PSetLE:MiniATInst<0x30, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetLE", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETLE))], IIAlu>;
def PSetGT:MiniATInst<0x31, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetGT", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETGT))], IIAlu>;
def PSetGE:MiniATInst<0x32, (outs GPR:$RC), (ins GPR:$RA, GPR:$RB), "PSetGE", [(set GPR:$RC, (selectcc GPR:$RA, GPR:$RB, 1, 0, SETGE))], IIAlu>;
}
//(set GPR:$RC, (Psetlt GPR:$RA, GPR:$RB)),
//def :Pat<(pret),(BRARI r0, (SUBRI r254, 1))>;

//def :Pat<(brcc seteq, GPR:$RA, GPR:$RB, brtarget:$imm), (BRAE $RA, $RB, $imm)>;
def :Pat<(brcond GPR:$cond, bb:$dst), (BRANE $cond, r0, $dst)>;
//def :Pat<(set GPR:$RC, (seteq GPR:$RA, GPR:$RA)), (Pseteq )
