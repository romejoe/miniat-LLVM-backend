
class MiniATReg<string n>:Register<n>{
	let Namespace = "MiniAT";
}


foreach i = 0-255 in {
	def r#i:MiniATReg<!strconcat("r", !cast<string>(i))>, DwarfRegNum<[i]>;	
}

//	Register Classes
def GPR:RegisterClass<"MiniAT", [i32], 32, (sequence "r%u", 0, 255)>;
//def GGPR:RegisterClass<"MiniAT", [i32], 32, (sequence "r%u", 2, 252)>;

def SPR:RegisterClass<"MiniAT", [i32], 32, 
	(add
		r0, //Zero Register
		r1, //Predicate Register
		r253, //RSP
		r254, //SP
		r255 //PC

	)>;

