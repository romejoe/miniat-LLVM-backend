//===-- MiniATMachineFunctionInfo.h - Private data used for MiniAT ----*- C++ -*-=//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MiniAT specific subclass of MachineFunctionInfo.
//
//===----------------------------------------------------------------------===//

#ifndef MINIAT_MACHINE_FUNCTION_INFO_H
#define MINIAT_MACHINE_FUNCTION_INFO_H

#include "llvm/ADT/StringMap.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineMemOperand.h"
#include "llvm/CodeGen/PseudoSourceValue.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/ValueMap.h"
#include "llvm/Target/TargetFrameLowering.h"
#include "llvm/Target/TargetMachine.h"
#include <map>
#include <string>
#include <utility>

namespace llvm {

/// \brief A class derived from PseudoSourceValue that represents a GOT entry
/// resolved by lazy-binding.
class MiniATCallEntry : public PseudoSourceValue {
public:
  explicit MiniATCallEntry(const StringRef &N);
  explicit MiniATCallEntry(const GlobalValue *V);
  bool isConstant(const MachineFrameInfo *) const override;
  bool isAliased(const MachineFrameInfo *) const override;
  bool mayAlias(const MachineFrameInfo *) const override;

private:
  void printCustom(raw_ostream &O) const override;
#ifndef NDEBUG
  std::string Name;
  const GlobalValue *Val;
#endif
};

/// MiniATFunctionInfo - This class is derived from MachineFunction private
/// MiniAT target-specific information for each MachineFunction.
class MiniATFunctionInfo : public MachineFunctionInfo {
public:
  MiniATFunctionInfo(MachineFunction& MF)
  : MF(MF), 
    VarArgsFrameIndex(0), 
    MaxCallFrameSize(0)
    {}

  ~MiniATFunctionInfo();

  int getVarArgsFrameIndex() const { return VarArgsFrameIndex; }
  void setVarArgsFrameIndex(int Index) { VarArgsFrameIndex = Index; }

  void setReturnStackOffset(unsigned int i);

  unsigned int getReturnStackOffset();

private:
  virtual void anchor();

  MachineFunction& MF;

    /// VarArgsFrameIndex - FrameIndex for start of varargs area.
  int VarArgsFrameIndex;

  unsigned MaxCallFrameSize;

  /// MiniATCallEntry maps.
  StringMap<const MiniATCallEntry *> ExternalCallEntries;
  ValueMap<const GlobalValue *, const MiniATCallEntry *> GlobalCallEntries;
};

} // end of namespace llvm

#endif // MINIAT_MACHINE_FUNCTION_INFO_H

