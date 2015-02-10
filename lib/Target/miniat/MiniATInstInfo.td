class MiniATInstr<dag outs, dag ins, string asmstr, list<dag> pattern>
 : Instruction{
	let Namespace = "MiniAT";

	dag OutOperandList = outs;
	dag InOperandList = ins;
	let AsmString   = asmstr;
	let Pattern = pattern;

}
multiclass BinaryInstr<string OpCode, SDNode OpNode> {
	def rri: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RB, MiniATReg:$RC, i32imm:$imm)
		, !strconcat(OpCode, " $RA, $RB, ($imm + $RC)")
		, [(set i32:$RA, (OpNode i32:$RB, (add i32imm:$imm, i32:$RC)))]>;
	def ri: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RB, i32imm:$imm)
		, !strconcat(OpCode, "$RA, $RB, ($imm + r0)")
		, [(set i32:$RA, (OpNode i32:$RB, i32imm:$imm)))]>;
	def rr: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RB, MiniATReg:$RC)
		, !strconcat(OpCode, "$RA, $RB, (0 + $RC)")
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC)))]>;

}

multiclass LoadInstr<> {
	def ri: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC, i32imm:$imm)
		, "LOAD $RA, [$imm + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins i32imm:$imm)
		, "LOAD $RA, [$imm + r0]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC)
		, "LOAD $RA, [0 + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}

multiclass StoreInstr<> {
	def ri: MiniATInstr<
		(outs MiniATReg:$RC, i32imm:$imm)
		, (ins MiniATReg:$RA)
		, "STORE [$imm + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins i32imm:$imm)
		, "STORE [$imm + r0], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC)
		, "STORE [0 + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}

multiclass RLoadInstr<> {
	def ri: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC, i32imm:$imm)
		, "RLOAD $RA, [$imm + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins i32imm:$imm)
		, "RLOAD $RA, [$imm + r0]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC)
		, "RLOAD $RA, [0 + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}

multiclass RStoreInstr<> {
	def ri: MiniATInstr<
		(outs MiniATReg:$RC, i32imm:$imm)
		, (ins MiniATReg:$RA)
		, "RSTORE [$imm + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins i32imm:$imm)
		, "RSTORE [$imm + r0], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs MiniATReg:$RA)
		, (ins MiniATReg:$RC)
		, "RSTORE [0 + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}


multiclass MiniATBranchInstr<string asmstr, SDNode OpNode>{

	def ri: MiniATInstr<
		(outs )
		, (ins MiniATReg:$RA, MiniATReg:$RB, MiniATReg:$RC, i32imm:$imm)
		, (!strconcat(OpCode, "[$RC + $imm], $RA, $RB"))
		, []>;
	def i: MiniATInstr<
		(outs )
		, (ins MiniATReg:$RA, MiniATReg:$RB, i32imm:$imm)
		, (!strconcat(OpCode, "[r0 + $imm], $RA, $RB"))
		, []>;
	def r: MiniATInstr<
		(outs )
		, (ins MiniATReg:$RA, MiniATReg:$RB, MiniATReg:$RC)
		, (!strconcat(OpCode, "[$RC + 0], $RA, $RB"))
		, []>;
}



//arithmetic Instructions
defm ADD:BinaryInstr<"ADD", add>;
defm SUB:BinaryInstr<"SUB", sub>;
defm MULT:BinaryInstr<"MULT", mul>;
defm DIV:BinaryInstr<"DIV", div>;
defm MOD:BinaryInstr<"MOD", mod>;

//logic Instructions
defm AND:BinaryInstr<"AND", and>;
defm OR:BinaryInstr<"OR", or>;
defm EXOR:BinaryInstr<"EXOR", xor>;
defm SHL:BinaryInstr<"SHL", shl>;//check
defm SHR:BinaryInstr<"SHR", shr>;//check

//data Instructions
defm LOAD:LoadInstr;
defm STORE:StoreInstr;

defm RLOAD:RLoadInstr;
defm RSTORE:RStoreInstr;



//control Instructions
defm BRAE:MiniATBranchInstr<"BRAE">;
defm BRANE:MiniATBranchInstr<"BRANE">;
defm BRAL:MiniATBranchInstr<"BRAL">;
defm BRALE:MiniATBranchInstr<"BRALE">;
defm BRAG:MiniATBranchInstr<"BRAG">;
defm BRAGE:MiniATBranchInstr<"BRAGE">;



//Pseudo Instructions
//Arithmetic
def NEG:MiniATInstr<(outs MiniATReg:$RA), (ins MiniATReg:$RC), "NEG $RA, $RC", []>;

//Logic
def INV:MiniATInstr<(outs MiniATReg:$RA), (ins MiniATReg:$RC), "INV $RA, $RC", []>;

//Data
def MOVR:MiniATInstr<(outs MiniATReg:$RA), (ins MiniATReg:$RC), "MOV $RA, $RC", []>;
def MOVI:MiniATInstr<(outs MiniATReg:$RA), (ins i32imm:$imm) "MOV $RA, $imm", []>;

//Control
multiclass MiniATBraInstr<string asmstr, SDNode OpNode>{

	def ri: MiniATInstr<
		(outs )
		, (ins MiniATReg:$RC, i32imm:$imm)
		, (!strconcat(OpCode, "[$RC + $imm]"))
		, []>;
	def i: MiniATInstr<
		(outs )
		, (ins i32imm:$imm)
		, (!strconcat(OpCode, "[r0 + $imm]"))
		, []>;
	def r: MiniATInstr<
		(outs )
		, (ins MiniATReg:$RC)
		, (!strconcat(OpCode, "[$RC + 0]"))
		, []>;
}
def BRA:MiniATBraInstr<"BRA">; 
def NOP:MiniATInstr<(outs ), (ins ), "NOP", []>;
def FLUSH:MiniATInstr<(outs ), (ins ), "FLUSH", []>;