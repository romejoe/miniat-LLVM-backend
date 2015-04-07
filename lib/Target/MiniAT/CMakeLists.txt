set(LLVM_TARGET_DEFINITIONS MiniAT.td)

tablegen(LLVM MiniATGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM MiniATGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM MiniATGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM MiniATGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM MiniATGenCodeEmitter.inc -gen-emitter)
tablegen(LLVM MiniATGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM MiniATGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM MiniATGenCallingConv.inc -gen-callingconv)
tablegen(LLVM MiniATGenMCPseudoLowering.inc -gen-pseudo-lowering)

add_public_tablegen_target(MiniATCommonTableGen)

add_llvm_target(MiniATCodeGen
  MiniATAnalyzeImmediate.cpp
  MiniATAsmPrinter.cpp
  MiniATFrameLowering.cpp
  #MiniATFrameToArgsOffsetElim.cpp
  MiniATISelDAGToDAG.cpp
  MiniATStandardISelDAGToDAG.cpp
  #MiniATLowerThreadLocal.cpp
  #MiniATMachineFunctionInfo.cpp
  #MiniATMCInstLower.cpp
  MiniATRegisterInfo.cpp
  #MiniATTargetTransformInfo.cpp
  MiniATMCInstLower.cpp
  MiniATFrameLowering.cpp
  MiniATInstrInfo.cpp
  MiniATInstrInfo.cpp
  MiniATISelLowering.cpp
  MiniATMachineFunction.cpp
  MiniATRegisterInfo.cpp
  MiniATStandardFrameLowering.cpp
  MiniATStandardInstrInfo.cpp
  MiniATStandardISelLowering.cpp
  MiniATSelectionDAGInfo.cpp
  MiniATStandardRegisterInfo.cpp
  MiniATSubtarget.cpp
  MiniATTargetMachine.cpp
  #MiniATTargetObjectFile.cpp
  TargetLoweringObjectFileDummy.cpp


  )

add_subdirectory(InstPrinter)
add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
