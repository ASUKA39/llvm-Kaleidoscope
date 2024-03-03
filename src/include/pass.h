#ifndef __PASS_H__
#define __PASS_H__

#include <memory>
#include <string>

#include "IR.h"

#include "IR/LegacyPassManager.h"
#include "IR/PassInstrumentation.h"
#include "Analysis/CGSCCPassManager.h"
#include "Analysis/LoopAnalysisManager.h"
#include "Passes/PassBuilder.h"
#include "Passes/StandardInstrumentations.h"

extern std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;

#endif