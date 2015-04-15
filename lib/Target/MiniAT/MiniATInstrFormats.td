
def simm32      : Operand<i32> {
  let DecoderMethod= "DecodeSimm32";
}
def mem : Operand<i32> {
  let PrintMethod = "printMemOperand";
  let MIOperandInfo = (ops GPR, simm32);
  let EncoderMethod = "getMemEncoding";
}

def brtarget : Operand<OtherVT>;

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
		, !strconcat(OpCode, " \t $RA, $RB, ($RC + $imm)")
		, [(set GPR:$RA, (OpNode GPR:$RB, (add GPR:$RC, immSExt32:$imm)))]
		, IIAlu, isComm
	>;

	def RI: InstRI<
		op
		//, !strconcat(OpCode, " \t $RA, $RB, (r0 + $imm)")
		, !strconcat(OpCode, " \t $RA, $RB, ($imm)")
		, [(set GPR:$RA, (OpNode GPR:$RB, immSExt32:$imm))]
		, IIAlu, isComm
	>;

	def RR: InstRR<
		op
		//, !strconcat(OpCode, " \t $RA, $RB, ($RC + 0)")
		, !strconcat(OpCode, " \t $RA, $RB, ($RC)")
		, [(set GPR:$RA, (OpNode GPR:$RB, GPR:$RC))]
		, IIAlu, isComm
	>;

}



// Cpu0 Address Mode! SDNode frameindex could possibily be a match
// since load and store instructions from stack used it.
def addr : ComplexPattern<iPTR, 2, "SelectAddr", [frameindex], [SDNPWantParent]>;


class AlignedLoad<PatFrag Node> :
  PatFrag<(ops node:$ptr), (Node node:$ptr), [{
  LoadSDNode *LD = cast<LoadSDNode>(N);
  return LD->getMemoryVT().getSizeInBits()/8 <= LD->getAlignment();
}]>;

class AlignedStore<PatFrag Node> :
  PatFrag<(ops node:$val, node:$ptr), (Node node:$val, node:$ptr), [{
  StoreSDNode *SD = cast<StoreSDNode>(N);
  return SD->getMemoryVT().getSizeInBits()/8 <= SD->getAlignment();
}]>;

// Load/Store PatFrags.
def load_a          : AlignedLoad<load>;
def store_a         : AlignedStore<store>;

// Memory Load/Store
let canFoldAsLoad = 1 in
class LoadM<bits<6> op, string instr_asm, PatFrag OpNode, Operand MemOpnd>:
  MiniATInst<op, (outs GPR:$ra), (ins MemOpnd:$addr),
     !strconcat(instr_asm, "\t$ra, [$addr]"),
     [(set GPR:$ra, (OpNode addr:$addr))], IILoad>;

class StoreM<bits<6> op, string instr_asm, PatFrag OpNode, Operand MemOpnd>:
  MiniATInst<op, (outs), (ins GPR:$ra, MemOpnd:$addr),
     !strconcat(instr_asm, "\t[$addr], $ra"),
     [(OpNode GPR:$ra, addr:$addr)], IIStore>;

//multiclass LoadM32<bits<6> op, string instr_asm, PatFrag OpNode,
//                   bit Pseudo = 0> {
//  def #NAME# : LoadM<op, instr_asm, OpNode, mem>;
//}

// 32-bit store.
multiclass StoreM32<bits<6> op, string instr_asm, PatFrag OpNode,
                    bit Pseudo = 0> {
  def #NAME# : StoreM<op, instr_asm, OpNode, mem>;
}




//let canFoldAsLoad = 1 in
multiclass LoadInstr<bits<6> opcode> {
	def RI: InstRI<
		opcode
		, "LOAD \t $RA, [$RB + $imm]"
		//, [(set GPR:$RA, (load (add GPR:$RB, immSExt32:$imm)))]
		, [(set GPR:$RA, (load_a (add GPR:$RB, immSExt32:$imm)))]
		, IILoad
	>;

	def I:LoadM<
		opcode
		, "LOAD"
		, load_a
		, mem
	>;

	//def I: InstI<
	//	opcode
	//	//, "LOAD \t $RA, [r0 + $imm]"
	//	, "LOAD \t $RA, [$imm]"
	//	//, [(set GPR:$RA, (load immSExt32:$imm)), (load_a immSExt32:$imm)]
	//	, [(load_a immSExt32:$imm)]
	//	, IILoad
	//>;

	def R: InstR<
		opcode
		//, "LOAD \t $RA, [$RC + 0]"
		, "LOAD \t $RA, [$RC]"
		, [(set GPR:$RA, (load GPR:$RC))]
		, IILoad
	>;
}

multiclass StoreInstr<bits<6> opcode> {
def RI: InstRI<
		opcode
		, "STORE \t [$RB + $imm], $RA"
		//, [(set GPR:$RA, (load (add GPR:$RB, immSExt32:$imm)))]
		//, [(set GPR:$RA, (load_a (add GPR:$RB, immSExt32:$imm)))]
		,[]
		, IIStore
	>;

	//def RI: InstRI<
	//	opcode
	//	, "STORE \t [$RB + $imm], $RA"
	//	, [(store_a GPR:$RA, (add i32:$RB, immSExt32:$imm))]
	//	, IIStore
	//>;

	def I:StoreM<
    		opcode
    		, "STORE"
    		, store_a
    		, mem
    	>;

	//def I: MiniATInst<
	//	opcode
	//	, (outs )
	//	, (ins GPR:$RA, i32imm:$imm)
	//	//, "STORE \t [$imm + r0], $RA"
	//	, "STORE \t [$imm], $RA"
	//	, [(store GPR:$RA, immSExt32:$imm)]
	//	, IIStore
	//>;
	//def R: MiniATInst<
	//	opcode
	//	, (outs )
	//	, (ins GPR:$RA, GPR:$RC)
	//	//, "STORE \t [0 + $RC], $RA"
	//	, "STORE \t [$RC], $RA"
	//	, [(store GPR:$RA, GPR:$RC)]
	//	, IIStore
	//>;

}

//*
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


class MiniATBranchInstr<bits<6> opcode, string asmstr, PatFrag cond>
	:MiniATInst<
     		opcode
     		, (outs)
     		, (ins GPR:$RA, GPR:$RB, brtarget:$imm)
     		, !strconcat(asmstr, " \t [$imm], $RA, $RB")
     		, [(brcond (cond GPR:$RA, GPR:$RB), bb:$imm)//,
     			//(brcc (cond GPR:$RA, GPR:$RB), bb:$imm)
     		]//, [(brcond  (cond GPR:$RA, GPR:$RB), (add GPR:$RC, immSExt32:$imm))]
     		, IIBranch
     	>{
	let isBranch=1;
}


//multiclass MiniATBranchInstr<bits<6> opcode, string asmstr, PatFrag cond>{
//
//	let isBranch=1 in
//	def RI: MiniATInst<
//		opcode
//		, (outs )
//		, (ins GPR:$RA, GPR:$RB, brtarget:$imm)
//		, !strconcat(asmstr, " \t [$imm], $RA, $RB")
//		, [(brcond  (cond GPR:$RA, GPR:$RB), bb:$imm)]//, [(brcond  (cond GPR:$RA, GPR:$RB), (add GPR:$RC, immSExt32:$imm))]
//		, IIBranch
//	>;
	//def :Pat<
	//	(brcond  (cond GPR:$RA, GPR:$RB), (add GPR:$RC, immSExt32:$imm)),
	//	(RI $RA, $RB, $RC, $imm)
	//>;

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
//}


///*
//class MiniATPurePseudo<dag outs, dag ins, string asmstr, list<dag> pattern>:
//      MiniATInst<outs, ins, asmstr, pattern, IIPseudo, Pseudo> {
//  let isCodeGenOnly = 1;
//  let isPseudo = 1;
//}*/