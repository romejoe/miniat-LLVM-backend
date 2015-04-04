
include "MiniATInstrFormats.td"

//arithmetic Instructions
defm ADD:ArithLogicInst 	<0x00	,"ADD"	,add>;
defm SUB:ArithLogicInst 	<0x01	,"SUB"	,sub>;
defm MULT:ArithLogicInst 	<0x02	,"MULT"	,mul>;
defm DIV:ArithLogicInst 	<0x03	,"DIV"	,sdiv>;
defm MOD:ArithLogicInst 	<0x04	,"MOD"	,srem>;

//logic Instructions
defm AND:ArithLogicInst 	<0x09	,"AND"	,and>;
defm OR:ArithLogicInst  	<0x0A	,"OR"	,or>;
defm EXOR:ArithLogicInst 	<0x0B	,"EXOR"	,xor>;
defm SHL:ArithLogicInst 	<0x0C	,"SHL"	,rotl>;//check
defm SHR:ArithLogicInst 	<0x0D	,"SHR"	,rotr>;//check

//data Instructions
defm LOAD:LoadInstr 	<0x10>;
defm STORE:StoreInstr	<0x11>;

/*
defm RLOAD:RLoadInstr;
defm RSTORE:RStoreInstr;
*/


//control Instructions
/*
defm BRAE:MiniATBranchInstr 	<0x16	,"BRAE" 	,seteq>;
defm BRANE:MiniATBranchInstr 	<0x17	,"BRANE" 	,setne>;
defm BRAL:MiniATBranchInstr 	<0x18	,"BRAL" 	,setlt>;
defm BRALE:MiniATBranchInstr 	<0x19	,"BRALE" 	,setle>;
defm BRAG:MiniATBranchInstr 	<0x1A	,"BRAG" 	,setgt>;
defm BRAGE:MiniATBranchInstr 	<0x1B	,"BRAGE" 	,setge>;

*/

//Pseudo Instructions
//Arithmetic
/*def NEG:MiniATInstr<
	(outs MiniATReg:$RA)
	, (ins MiniATReg:$RC)
	, "NEG $RA, $RC"
	, []>;
*/
//Logic
/*def INV:MiniATInstr<
	(outs MiniATReg:$RA)
	, (ins MiniATReg:$RC)
	, "INV $RA, $RC"
	, []>;
*/
//Data

///* MOVR */	def :Pat<(set GPR:$RA, GPR:$RC), (set GPR:$RA, (add GPR:$RC, 0))>;
///* MOVI */	def :Pat<(set GPR:$RA, i32imm:$imm), (set GPR:$RA, (add r0, i32imm:$imm))>;
///* NOP */	def :Pat<(), (set r0, (add r0, r0))>;
///* Flush */	def :Pat<(), (br r255)>;

//Control
//Unconditional Branches
/*def :Pat<
	(br (add GPR:$RC, i32imm:$imm))
	, (brcond (seteq (i32 (seteq r0, r0)), (add GPR:$RC, i32imm:$imm)) )
	>;

def :Pat<
	(br GPR:$RC)
	, (brcond (seteq (i32 (seteq r0, r0)), GPR:$RC) )
	>;

def :Pat<
	(br i32imm:$imm)
	, (brcond (seteq (i32 (seteq r0, r0)), i32imm:$imm) )
	>;

*/

//def :Pat<(