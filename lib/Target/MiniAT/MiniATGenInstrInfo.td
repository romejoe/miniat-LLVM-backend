
include "MiniATGenInstrFormats.td"

//arithmetic Instructions
defm ADD:BinaryInstr<"ADD", add>;
defm SUB:BinaryInstr<"SUB", sub>;
defm MULT:BinaryInstr<"MULT", mul>;
defm DIV:BinaryInstr<"DIV", sdiv>;
defm MOD:BinaryInstr<"MOD", srem>;

//logic Instructions
defm AND:BinaryInstr<"AND", and>;
defm OR:BinaryInstr<"OR", or>;
defm EXOR:BinaryInstr<"EXOR", xor>;
defm SHL:BinaryInstr<"SHL", rotl>;//check
defm SHR:BinaryInstr<"SHR", rotr>;//check

//data Instructions
defm LOAD:LoadInstr;
defm STORE:StoreInstr;

/*
defm RLOAD:RLoadInstr;
defm RSTORE:RStoreInstr;
*/


//control Instructions
defm BRAE:MiniATBranchInstr<"BRAE", seteq>;
defm BRANE:MiniATBranchInstr<"BRANE", setne>;
defm BRAL:MiniATBranchInstr<"BRAL", setlt>;
defm BRALE:MiniATBranchInstr<"BRALE", setle>;
defm BRAG:MiniATBranchInstr<"BRAG", setgt>;
defm BRAGE:MiniATBranchInstr<"BRAGE", setge>;



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
def MOVR:MiniATInstr<(outs GPR:$RA), (ins GPR:$RC), "MOV $RA, $RC", []>;
def MOVI:MiniATInstr<(outs GPR:$RA), (ins i32imm:$imm), "MOV $RA, $imm", []>;

//Control

defm BRA:MiniATBraInstr; 
def NOP:MiniATInstr<(outs ), (ins ), "NOP", []>;
def FLUSH:MiniATInstr<(outs ), (ins ), "FLUSH", []>;