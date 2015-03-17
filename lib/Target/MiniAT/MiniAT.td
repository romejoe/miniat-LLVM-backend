
include "llvm/Target/Target.td"

//Register Information
include "MiniATRegisterInfo.td"

//Instruction Information
include "MiniATInstrInfo.td"

//Calling Convention Information
include "MiniATCallingConv.td"

def MiniATInstrInfo : InstrInfo;


class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"standard", []>;

def MiniAT : Target {
  // Pull in Instruction Info:
  let InstructionSet = MiniATInstrInfo;
}
