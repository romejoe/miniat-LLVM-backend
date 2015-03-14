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

//===----------------------------------------------------------------------===//
// MiniAT Return Value Calling Convention
//===----------------------------------------------------------------------===//
def RetCC_MiniAT : CallingConv<[
  // i32 are returned in register r2
  CCIfType<[i32], CCAssignToReg<[r2]>>
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
//  CCIfType<[i32], CCAssignToReg<[R0, R1, R2, R3]>>,

  // Integer values get stored in stack slots that are 4 bytes in
  // size and 4-byte aligned.
  CCIfType<[i32], CCAssignToStack<4, 4>>
]>;
