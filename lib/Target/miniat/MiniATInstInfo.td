class Instruction<string Name>{
	string Namespace = "MiniAT";
	string NAME = !strconcat(Namespace,"_", Name);
}

class ArithmeticInstruction<string asmstr>:Instruction<asmstr>{
	//dag OutOperandList = (outs 
	string AsmString = !strconcat(asmstr, "\t $dst,\t $src1,\t ($src2 + $const)");
}

//standard arithmetic operations
def ADD:ArithmeticInstruction<"ADD">;
def SUB:ArithmeticInstruction<"SUB">;
def MULT:ArithmeticInstruction<"MULT">;
def DIV:ArithmeticInstruction<"DIV">;
def MOD:ArithmeticInstruction<"MOD">;

//standard logic functions
def AND:ArithmeticInstruction<"AND">;
def OR:ArithmeticInstruction<"OR">;
def EXOR:ArithmeticInstruction<"EXOR">;
def SHL:ArithmeticInstruction<"SHL">;
def SHR:ArithmeticInstruction<"SHR">;

