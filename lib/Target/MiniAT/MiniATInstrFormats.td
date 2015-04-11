
class MiniATInst<bits<6> opcode, dag outs, dag ins, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
 	: Instruction{
	let Namespace = "MiniAT";
	field bits<64> Inst;

	let isCommutable = isComm;	// e.g. add rb rc =  add rc rb

	bits<32> imm = 0;
	
	bits<1> PredicateBit = 0;
	bits<1> BranchPredictionBit = 0;
	bits<5> Opcode = opcode{1-5};
	bits<1> UnusedBit = 0;
	
	bits<8> rA = 0;
	bits<8> rB = 0;
	bits<8> rC = 0;


	let Inst{31-0} = imm;
	let Inst{39-32} = rC;
	let Inst{47-40} = rB;
	let Inst{55-48} = rA;
	let Inst{56} = UnusedBit;
	let Inst{61-57} = Opcode;
	let Inst{62} = BranchPredictionBit;
	let Inst{63} = PredicateBit;


	dag OutOperandList = outs;
	dag InOperandList = ins;
	let AsmString   = asmstr;
	let Pattern = pattern;
	let Itinerary = itin;

}


def immSExt32 : PatLeaf<(imm),[{ return isInt<32>(N->getSExtValue()); }]>;

class InstRRI<bits<6> opcode, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
			: MiniATInst<opcode
			, (outs GPR:$RA)
			, (ins GPR:$RB, GPR:$RC, i32imm:$imm)
			, asmstr, pattern, itin, isComm>;

class InstRI<bits<6> opcode, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
			: MiniATInst<opcode
			, (outs GPR:$RA)
			, (ins GPR:$RB, i32imm:$imm)
			, asmstr, pattern, itin, isComm>;

class InstRR<bits<6> opcode, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
			: MiniATInst<opcode
			, (outs GPR:$RA)
			, (ins GPR:$RB, GPR:$RC)
			, asmstr, pattern, itin, isComm>;

class InstR<bits<6> opcode, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
			: MiniATInst<opcode
			, (outs GPR:$RA)
			, (ins GPR:$RC)
			, asmstr, pattern, itin, isComm>;

class InstI<bits<6> opcode, string asmstr, list<dag> pattern, InstrItinClass itin, bit isComm = 0>
			: MiniATInst<opcode
			, (outs GPR:$RA)
			, (ins i32imm:$imm)
			, asmstr, pattern, itin, isComm>;

multiclass ArithLogicInst<bits<6> op, string OpCode, SDNode OpNode, bit isComm = 0> {
	def RRI:InstRRI<
		op
		, !strconcat(OpCode, " $RA, $RB, ($imm + $RC)")
		, [(set GPR:$RA, (OpNode GPR:$RB, (add GPR:$RC, immSExt32:$imm)))]
		, IIAlu, isComm
	>;

	def RI: InstRI<
		op
		, !strconcat(OpCode, "$RA, $RB, ($imm + r0)")
		, [(set GPR:$RA, (OpNode GPR:$RB, immSExt32:$imm))]
		, IIAlu, isComm
	>;

	def RR: InstRR<
		op
		, !strconcat(OpCode, "$RA, $RB, (0 + $RC)")
		, [(set GPR:$RA, (OpNode GPR:$RB, GPR:$RC))]
		, IIAlu, isComm
	>;

}

multiclass LoadInstr<bits<6> opcode> {
	def RI: InstRI<
		opcode
		, "LOAD $RA, [$imm + $RB]"
		, [(set GPR:$RA, (load (add GPR:$RB, immSExt32:$imm)))]
		, IILoad
	>;

	def I: InstI<
		opcode
		, "LOAD $RA, [$imm + r0]"
		, [(set GPR:$RA, (load immSExt32:$imm))]
		, IILoad
	>;
	
	def R: InstR<
		opcode
		, "LOAD $RA, [0 + $RC]"
		, [(set GPR:$RA, (load GPR:$RC))]
		, IILoad
	>;
}

multiclass StoreInstr<bits<6> opcode> {
	def RI: MiniATInst<
		opcode
		, (outs )
		, (ins GPR:$RA, GPR:$RC, i32imm:$imm)
		, "STORE [$imm + $RC], $RA"
		, [(store GPR:$RA, (add i32:$RC, immSExt32:$imm))]
		, IIStore
	>;

	def I: MiniATInst<
		opcode
		, (outs )
		, (ins GPR:$RA, i32imm:$imm)
		, "STORE [$imm + r0], $RA"
		, [(store GPR:$RA, immSExt32:$imm)]
		, IIStore
	>;
	def R: MiniATInst<
		opcode
		, (outs )
		, (ins GPR:$RA, GPR:$RC)
		, "STORE [0 + $RC], $RA"
		, [(store GPR:$RA, GPR:$RC)]
		, IIStore
	>;

}
///*
//multiclass RLoadInstr {
//	def ri: MiniATInstr<
//		(outs GPR:$RA)
//		, (ins GPR:$RC, i32imm:$imm)
//		, "RLOAD $RA, [$imm + $RC]"
//		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
//	def i: MiniATInstr<
//		(outs GPR:$RA)
//		, (ins i32imm:$imm)
//		, "RLOAD $RA, [$imm + r0]"
//		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
//	def r: MiniATInstr<
//		(outs GPR:$RA)
//		, (ins GPR:$RC)
//		, "RLOAD $RA, [0 + $RC]"
//		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode
//
//}
//
//multiclass RStoreInstr {
//	def ri: MiniATInstr<
//		(outs GPR:$RC, i32imm:$imm)
//		, (ins GPR:$RA)
//		, "RSTORE [$imm + $RC], $RA"
//		, [(set i32:$RA, (OpNode i32:$RB, (add i32:$imm, i32:$RC)))]>;//OpNode
//	def i: MiniATInstr<
//		(outs GPR:$RA)
//		, (ins i32imm:$imm)
//		, "RSTORE [$imm + r0], $RA"
//		, [(set i32:$RA, (OpNode i32:$RB, i32:$imm))]>;//OpNode
//	def r: MiniATInstr<
//		(outs GPR:$RA)
//		, (ins GPR:$RC)
//		, "RSTORE [0 + $RC], $RA"
//		, [(set i32:$RA, (OpNode i32:$RB, i32:$RC))]>;//OpNode
//
//}
//*/



multiclass MiniATBranchInstr<bits<6> opcode, string asmstr, PatFrag cond>{

	let isBranch=1 in
	def RI: MiniATInst<
		opcode
		, (outs )
		, (ins GPR:$RA, GPR:$RB, GPR:$RC, i32imm:$imm)
		, !strconcat(asmstr, " [$RC + $imm], $RA, $RB")
		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), (add GPR:$RC, immSExt32:$imm))]
		, IIBranch
	>;
//	/*def :Pat<
//		(brcond (i32 (cond GPR:$RA, GPR:$RB)), immSExt32:$imm),
//		(brcond (i32 (cond GPR:$RA, GPR:$RB)), (add r0, immSExt32:$imm))
//	>;
//
//	def :Pat<
//		(brcond (i32 (cond GPR:$RA, GPR:$RB)), immSExt32:$imm),
//		(brcond (i32 (cond GPR:$RA, GPR:$RB)), (add GPR:$RC, 0))
//	>;*/
///*
//	def I: MiniATInst<
//		opcode
//		, (outs )
//		, (ins GPR:$RA, GPR:$RB, i32imm:$imm)
//		, !strconcat(asmstr, " [r0 + $imm], $RA, $RB")
//		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), immSExt32:$imm)]
//		, IIBranch
//	>;
//	def R: MiniATInst<
//		opcode
//		, (outs )
//		, (ins GPR:$RA, GPR:$RB, GPR:$RC)
//		, !strconcat(asmstr, " [$RC + 0], $RA, $RB")
//		, [(brcond (i32 (cond GPR:$RA, GPR:$RB)), GPR:$RC)]
//		, IIBranch
//	>;*/
}


///*
//class MiniATPurePseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
//      MiniATInst<outs, ins, asmstr, pattern, IIPseudo, Pseudo> {
//  let isCodeGenOnly = 1;
//  let isPseudo = 1;
//}*/