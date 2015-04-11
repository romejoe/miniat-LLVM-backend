
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
defm LOAD:LoadInstr 	<0x10>;
defm STORE:StoreInstr	<0x11>;


//defm RLOAD:RLoadInstr;
//defm RSTORE:RStoreInstr;



//control Instructions
//defm BRAE:MiniATBranchInstr 	<0x16	,"BRAE" 	, seteq>;
//defm BRANE:MiniATBranchInstr 	<0x17	,"BRANE" 	, setne>;
//defm BRAL:MiniATBranchInstr 	<0x18	,"BRAL" 	, setlt>;
//defm BRALE:MiniATBranchInstr 	<0x19	,"BRALE" 	, setle>;
//defm BRAG:MiniATBranchInstr 	<0x1A	,"BRAG" 	, setgt>;
//defm BRAGE:MiniATBranchInstr 	<0x1B	,"BRAGE" 	, setge>;


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
def MOVR:InstR		<0x20, "MOV $RA, $RC", [(set GPR:$RA, GPR:$RC)], II_CLZ>;
def MOVI:InstI		<0x21, "MOV $RA, $imm", [(set GPR:$RA, immSExt32:$imm)], II_CLZ>;
///* MOVI */	def :Pat<(set GPR:$RA, i32imm:$imm), (set GPR:$RA, (add r0, i32imm:$imm))>;
///* NOP */	def :Pat<(), (set r0, (add r0, r0))>;



def NOP:MiniATInst<0,(outs),(ins), "nop", [], IIAlu>;

///* Flush */	def :Pat<(), (br r255)>;

//Control
//Unconditional Branches

def BRARI:MiniATInst<
	0x22
	, (outs)
	, (ins GPR:$RC, i32imm:$imm)
	, "BRA [$RC + $imm]"
	, []//,[(br bb:(add GPR:$RC, i32imm:$imm))]
	, IIBranch
>;


//def :Pat<
//	(br (add GPR:$RC, i32imm:$imm))
//	, (brcond (seteq (i32 (seteq r0, r0)), (add GPR:$RC, i32imm:$imm)) )
//	>;
//
//def :Pat<
//	(br GPR:$RC)
//	, (brcond (seteq (i32 (seteq r0, r0)), GPR:$RC) )
//	>;
//
//def :Pat<
//	(br i32imm:$imm)
//	, (brcond (seteq (i32 (seteq r0, r0)), i32imm:$imm) )
//	>;



/* Pseudo Ret */
//def PRet:MiniATInst<0xFE, (outs ), (ins ), "PRET", [(ret)]>;


//def PCall:Pattern<(ret),
//	[(STORE GPR:r255, GPR:r254, i32:4), (LOAD]>;
//SDNPSideEffect
def pret: SDNode<"MiniATISD::PRet", SDTNone, [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def PRet:MiniATInst<0x24, (outs), (ins), "PRET", [(pret)], IIBranch>{
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

//def :Pat<(pret),(BRARI r0, (SUBRI r254, 1))>;