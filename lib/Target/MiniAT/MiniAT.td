
include "llvm/Target/Target.td"

//Register Information
include "MiniATGenRegisterInfo.td"

//Instruction Information
include "MiniATGenInstrInfo.td"

//Calling Convention Information
include "MiniATGenCallingConv.td"

def MiniATInstrInfo : InstrInfo;


class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, NoItineraries, Features>;

def : Proc<"standard", []>;

def MiniAT : Target {
  // Pull in Instruction Info:
  let InstructionSet = MiniATInstrInfo;
}
