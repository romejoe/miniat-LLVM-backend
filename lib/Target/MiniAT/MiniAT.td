
include "llvm/Target/Target.td"

//Register Information
include "MiniATRegisterInfo.td"

include "MiniATSchedule.td"
//Instruction Information
include "MiniATInstrInfo.td"



//Calling Convention Information
include "MiniATCallingConv.td"

def MiniATInstrInfo : InstrInfo;


class Proc<string Name, list<SubtargetFeature> Features>
 : Processor<Name, MiniATGenericItineraries, Features>;

def : Proc<"miniat", []>;

def MiniAT : Target {
  // Pull in Instruction Info:
  let InstructionSet = MiniATInstrInfo;
}

