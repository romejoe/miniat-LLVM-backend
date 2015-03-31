//===-- MiniATSEISelDAGToDAG.h - A Dag to Dag Inst Selector for MiniATSE -----===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Subclass of MiniATDAGToDAGISel specialized for mips32/64.
//
//===----------------------------------------------------------------------===//

#ifndef MINIATSEISELDAGTODAG_H
#define MINIATSEISELDAGTODAG_H

#include "MiniATISelDAGToDAG.h"

namespace llvm {

    class MiniATSEDAGToDAGISel : public MiniATDAGToDAGISel {

    public:
        explicit MiniATSEDAGToDAGISel(MiniATTargetMachine &TM) : MiniATDAGToDAGISel(TM) {}

    private:

        bool runOnMachineFunction(MachineFunction &MF) override;

        std::pair<bool, SDNode*> selectNode(SDNode *Node) override;

        void processFunctionAfterISel(MachineFunction &MF) override;

        // Insert instructions to initialize the global base register in the
        // first MBB of the function.
//  void initGlobalBaseReg(MachineFunction &MF);

    };

    FunctionPass *createMiniATSEISelDag(MiniATTargetMachine &TM);

}

#endif