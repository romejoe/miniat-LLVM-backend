
class MiniATReg<string n, bits<8> num>:Register<n>{
	let Namespace = "MiniAT";
	field bits<8> Num = num;
}


foreach i = 0-255 in {
	def r#i:MiniATReg<!strconcat("r", !cast<string>(i)), i>, DwarfRegNum<[i]>;
}

//	Register Classes
def GPR:RegisterClass<"MiniAT", [i32], 32, (sequence "r%u", 0, 255)>;


def SPR:RegisterClass<"MiniAT", [i32], 32, 
	(add
		r0, //Zero Register
		r1, //Predicate Register
		//r251, //FP
		r252, //LR
		r253, //RSP
		r254, //SP
		r255 //PC

	)>;

//def ZeroReg:RegisterClass<"MiniAT", [i32], 32, (add r0)>;

