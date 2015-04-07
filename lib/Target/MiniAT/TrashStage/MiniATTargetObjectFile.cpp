//===-- MiniATTargetObjectFile.cpp - MiniAT Object Files ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "MiniATTargetObjectFile.h"

#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Target/TargetMachine.h"

using namespace llvm;

void MiniATTargetObjectFile::Initialize(
        MCContext &Ctx
        , const TargetMachine &TM
    ){
        TargetLoweringObjectFileMiniAT::Initialize(Ctx, TM);


        this->TM = &TM;
    }