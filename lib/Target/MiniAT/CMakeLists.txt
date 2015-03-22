set(LLVM_TARGET_DEFINITIONS MiniAT.td)

tablegen(LLVM MiniATGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MiniATGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MiniATGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MiniATGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM MiniATGenCallingConv.inc -gen-callingconv)
add_public_tablegen_target(MiniATCommonTableGen)

add_llvm_target(
  MiniATCodeGen
  #MiniATAsmPrinter.cpp
  #MiniATFrameLowering.cpp
  MiniATInstrInfo.cpp
  MiniATISelDAGToDAG.cpp
  #MiniATISelLowering.cpp
  #MiniATLowerThreadLocal.cpp
  #MiniATMachineFunctionInfo.cpp
  #MiniATMCInstLower.cpp
  MiniATRegisterInfo.cpp
  #MiniATSubtarget.cpp
  MiniATTargetMachine.cpp
  #MiniATTargetObjectFile.cpp
  #MiniATTargetTransformInfo.cpp
  #MiniATSelectionDAGInfo.cpp
  #MiniATFrameToArgsOffsetElim.cpp
  )

#add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
