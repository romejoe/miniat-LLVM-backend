//===- MiniATCallingConv.td - Calling Conventions for MiniAT -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
// This describes the calling conventions for MiniAT architecture.
//===----------------------------------------------------------------------===//

/// CCIfSubtarget - Match if the current subtarget has a feature F.
class CCIfSubtarget<string F, CCAction A>:
  CCIf<!strconcat("State.getTarget().getSubtarget<MiniATSubtarget>().", F), A>;

//===----------------------------------------------------------------------===//
// MiniAT Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_MiniAT : CallingConv<[
  // i32 are returned in register r2
  CCIfType<[i32], CCAssignToReg<[r2,r3,r4,r5]>>
]>;

//===----------------------------------------------------------------------===//
// MiniAT Argument Calling Conventions
//===----------------------------------------------------------------------===//
def CC_MiniAT : CallingConv<[
  // Promote i8/i16 arguments to i32.
   CCIfType<[i8, i16], CCPromoteToType<i32>>,

  // The 'nest' parameter, if any, is passed in R11.
//  CCIfNest<CCAssignToReg<[R11]>>,

  // The first 4 integer arguments are passed in integer registers.
  CCIfType<[i32], CCAssignToReg<[r2,r3,r4,r5]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;

//===----------------------------------------------------------------------===//
// MiniAT Calling Convention Dispatch
//===----------------------------------------------------------------------===//

def CSR_O32 : CalleeSavedRegs<(add (sequence "r%u", 1, 254))>;