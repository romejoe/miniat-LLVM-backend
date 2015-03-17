
class MiniATInst<
		bits<5> opcode
		, dag outs
		, dag ins
		, string asmstr
		, list<dag> pattern
	>
 	: Instruction{
	let Namespace = "MiniAT";
	field bits<64> Inst;


	bits<32> imm = 0;
	
	bits<1> PredicateBit = 0;
	bits<1> BranchPredictionBit = 0;
	bits<5> Opcode = 0;
	bits<1> UnusedBit = 0;
	
	bits<8> rA = 0;
	bits<8> rB = 0;
	bits<8> rC = 0;


	Inst{31-0} = imm;
	Inst{39-32} = rC;
	Inst{47-40} = rB;
	Inst{55-48} = rA;
	Inst{56} = UnusedBit;
	Inst{61-57} = Opcode;
	Inst{62} = BranchPredictionBit;
	Inst{63} = PredicateBit;


	dag OutOperandList = outs;
	dag InOperandList = ins;
	let AsmString   = asmstr;
	let Pattern = pattern;
}

class InstRRI<
		bits<5> opcode
		, dag outs
		, dag ins
		, string asmstr
		, list<dag> pattern
	>: MiniATInst<opcode, outs, ins, asmstr, pattern>{

}

multiclass ArithLogicInst<bits<5> op, string OpCode, SDNode OpNode> {
	def rri: MiniATInstr<
		op
		, (outs GPR:$RA)
		, (ins GPR:$RB, GPR:$RC, i32imm:$imm)
		, !strconcat(OpCode, " $RA, $RB, ($imm + $RC)")
		, [(set i32:$RA, (OpNode i32:$RB, (add i32imm:$imm, i32:$RC)))]>{

		}
	def ri: MiniATInstr<
		op
		, (outs GPR:$RA)
		, (ins GPR:$RB, i32imm:$imm)
		, !strconcat(OpCode, "$RA, $RB, ($imm + r0)")
		, [(set i32:$RA, (OpNode i32:$RB, i32imm:$imm))]>;
	def rr: MiniATInstr<
		op
		, (outs GPR:$RA)
		, (ins GPR:$RB, GPR:$RC)
		, !strconcat(OpCode, "$RA, $RB, (0 + $RC)")
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;

}

multiclass LoadInstr {
	def ri: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC, i32imm:$imm)
		, "LOAD $RA, [$imm + $RC]"
		, [(set GPR:$RA, (load (add i32imm:$imm, GPR:$RC)))]>;
	def i: MiniATInstr<
		(outs GPR:$RA)
		, (ins i32imm:$imm)
		, "LOAD $RA, [$imm + r0]"
		, [(set GPR:$RA, (load (i32imm:$imm)))]>; //node???
	def r: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC)
		, "LOAD $RA, [0 + $RC]"
		, [(set GPR:$RA, (load GPR:$RC))]>;

}

multiclass StoreInstr {
	def ri: MiniATInstr<
		(outs GPR:$RC, i32imm:$imm)
		, (ins GPR:$RA)
		, "STORE [$imm + $RC], $RA"
		, [(set i32:$RA, (store i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs GPR:$RA)
		, (ins i32imm:$imm)
		, "STORE [$imm + r0], $RA"
		, [(set i32:$RA, (store i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC)
		, "STORE [0 + $RC], $RA"
		, [(set i32:$RA, (store i32:$RB, i32:$RC))]>;//OpNode

}
/*
multiclass RLoadInstr {
	def ri: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC, i32imm:$imm)
		, "RLOAD $RA, [$imm + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs GPR:$RA)
		, (ins i32imm:$imm)
		, "RLOAD $RA, [$imm + r0]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC)
		, "RLOAD $RA, [0 + $RC]"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}

multiclass RStoreInstr {
	def ri: MiniATInstr<
		(outs GPR:$RC, i32imm:$imm)
		, (ins GPR:$RA)
		, "RSTORE [$imm + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
	def i: MiniATInstr<
		(outs GPR:$RA)
		, (ins i32imm:$imm)
		, "RSTORE [$imm + r0], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
	def r: MiniATInstr<
		(outs GPR:$RA)
		, (ins GPR:$RC)
		, "RSTORE [0 + $RC], $RA"
		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode

}
*/

multiclass MiniATBranchInstr<string asmstr, PatFrag cond>{

	def ri: MiniATInstr<
		(outs )
		, (ins GPR:$RA, GPR:$RB, GPR:$RC, i32imm:$imm)
		, !strconcat(asmstr, " [$RC + $imm], $RA, $RB")
		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), (add GPR:$RC, i32imm:$imm))]>;
	def i: MiniATInstr<
		(outs )
		, (ins GPR:$RA, GPR:$RB, i32imm:$imm)
		, !strconcat(asmstr, " [r0 + $imm], $RA, $RB")
		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), bb:$imm)]>;
	def r: MiniATInstr<
		(outs )
		, (ins GPR:$RA, GPR:$RB, GPR:$RC)
		, !strconcat(asmstr, " [$RC + 0], $RA, $RB")
		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), GPR:$RC)]>;
}

multiclass MiniATBraInstr{

	def ri: MiniATInstr<
		(outs )
		, (ins GPR:$RC, i32imm:$imm)
		, "BRA [$RC + $imm]"
		, [(br (add GPR:$RC, i32imm:$imm))]>;
	def i: MiniATInstr<
		(outs )
		, (ins i32imm:$imm)
		, "BRA [r0 + $imm]"
		, [(br i32imm:$imm)]>;
	def r: MiniATInstr<
		(outs )
		, (ins GPR:$RC)
		, "BRA [$RC + 0]"
		, [(br GPR:$RC)]>;
}