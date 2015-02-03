
class MiniATReg<string n>:Register<n>{
	let Namespace = "MiniAT";
}

class MiniATGenReg<string n>:MiniATReg;
/*
def r0:MiniATReg<"r0">; //Zero Register
def r1:MiniATReg<"r1">; //Predicate Register
//General Registers
foreach i = 2-252 in {
	def r#i:MiniATReg<!strconcat("r", !cast<string>(i))>;
}


def r253:MiniATReg<"r253">; //RSP
def r254:MiniATReg<"r254">; //SP
def r255:MiniATReg<"r255">; //PC
*/

foreach i = 0-255 in {
	def r#i:MiniATReg<!strconcat("r", !cast<string>(i))>;	
}

/**
	Register Classes
*/

def GeneralRegister:RegisterClass<"MiniAT", [i32], 32, (sequence "r%u", 0, 255)>;
