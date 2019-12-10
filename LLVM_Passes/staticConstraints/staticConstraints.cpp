#include <string>
#include <vector>
#include <sstream> 

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"

using namespace llvm;

#define DEBUG_TYPE "staticConstraints"


namespace {
  // displayCode - Displays the program
  struct displayCode : public FunctionPass 
  {
    // Pass identification, replacement for typeid
    static char ID; 
    
    // LLVM pass initilizer
    displayCode() : FunctionPass(ID) 
    {
    }

    // For each function
    bool runOnFunction(Function &F) override 
    {
      // For each block in the code
      for(Function::iterator bb = F.begin(), ef = F.end(); bb != ef; ++bb)
      {
        // For each instruction in that block
        for(BasicBlock::iterator ii = bb->begin(), eb = bb->end(); ii != eb; ++ii)
        {

          // Get each of the instructions from the itterator
          Instruction* I = &*ii;

          // Get the raw instruction in a string format
          std::string raw_instruction;
          llvm::raw_string_ostream rso(raw_instruction);
          I->print(rso);

          // Print out the instruction
          errs() << "Instruction (" << I->getOpcodeName() << ") " << "\t" << ": " << raw_instruction << "\n" ;
        }
      }
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char displayCode::ID = 0;
static RegisterPass<displayCode> X("displayCode", "Display Code Static Pass");


namespace {
  // findConstraints - Finds all constraints in a program.
  struct findConstraints : public FunctionPass 
  {
    // Pass identification, replacement for typeid
    static char ID; 
    
    // LLVM pass initilizer
    findConstraints() : FunctionPass(ID) 
    {
      // Print out that you are starting
      errs() << "Generating Constraints\n\n" ;


    }

    // For each function
    bool runOnFunction(Function &F) override 
    {
      // Create two vectors to keep track of variable names and addresses
      std::vector<llvm::Value*> memory_address;
      std::vector<std::string> variable_name;

      // For each block in the code
      for(Function::iterator bb = F.begin(), ef = F.end(); bb != ef; ++bb)
      {
        // For each instruction in that block
        for(BasicBlock::iterator ii = bb->begin(), eb = bb->end(); ii != eb; ++ii)
        {

          // Get each of the instructions from the itterator
          Instruction* I = &*ii;

          // Get the raw instruction in a string format
          std::string raw_instruction;
          llvm::raw_string_ostream rso(raw_instruction);
          I->print(rso);

          // Print out the instruction
          errs() << "Instruction (" << I->getOpcodeName() << ") " << "\t" << ": " << raw_instruction << "\n" ;

          // If the statement is a load instruction
          if ( isa<LoadInst>(I))
          {
            // Create the load instruction
            // NOTE: lp is a llvm::value and represents the value being loaded into
            LoadInst *lp = dyn_cast<LoadInst>(I) ;

            // If we load a known variable into memory
            if (lp->getPointerOperand()->hasName())
            {
              // Get that variable name:
              std::string varname = lp->getPointerOperand()->getName() ;

              // Save the name and the memory address
              memory_address.push_back(lp) ;
              variable_name.push_back(varname) ;
            }
          }
          // If the statement is an if statement
          if ( isa<ICmpInst>(I) || isa<FCmpInst>(I) )
          {
            // Create a Cmp Statement
            CmpInst *cp = dyn_cast<CmpInst>(I) ; ;

            // String holding the constraint
            std::string constraints[3] ;

            // For all operands (-1 as the final is the predicate)
            for (unsigned op_num = 0; op_num <= cp->getNumOperands() - 1; op_num++)
            {
              // Create the string operand
              std::string operand_string ;
              // If the operand is a has a name
              if ( cp->getOperand(op_num)->hasName() )
              {
                // Get the name and save it
                operand_string = cp->getOperand(op_num)->getName() ;
              }
              // Check if it is a constant integer
              else if(ConstantInt* CI = dyn_cast<ConstantInt>(cp->getOperand(op_num))) 
              {
                // Get the value of the constant 
                operand_string = std::to_string(CI->getSExtValue());
              }
              else if(ConstantFP* CF = dyn_cast<ConstantFP>(cp->getOperand(op_num))) 
              {
                // Get the value of the constant 
                operand_string = std::to_string(CF->getValueAPF().convertToFloat());
              }
              else
              {
                bool Found = false ;
                // Search for the operand in the known memory addresses
                for (unsigned j = 0; j < memory_address.size(); j++)
                {
                  if (memory_address.at(j) == cp->getOperand(op_num))
                  {
                    // Set found == to true so we know its been found
                    Found = true ;

                    // Get the variable name
                    operand_string = variable_name.at(j) ;

                    // Exit the loop
                    break ;
                  }
                }

                // If we didnt find it, we dont have a clue what the memory address was
                if(!Found)
                { 
                  // Convert the memory address to a string
                  std::stringstream ss ;
                  ss << cp->getOperand(op_num) ;  
                  operand_string = ss.str() ;
                  // // Print out the error
                  errs() << "Unknown Operand: " << operand_string << "\n" ; 
                }
              }
              constraints[op_num] = operand_string ;
            }

            // Get the predicate and convert it back from enum
            int pred_enum = cp->getPredicate() ;

            switch(pred_enum) {
              case llvm::CmpInst::Predicate::FCMP_OEQ :
              case llvm::CmpInst::Predicate::ICMP_EQ  :
                constraints[2] = " == " ;
                break;
              case llvm::CmpInst::Predicate::FCMP_OGT : 
              case llvm::CmpInst::Predicate::ICMP_SGT  :
                constraints[2] = " > " ;
                break;
              case llvm::CmpInst::Predicate::FCMP_OGE :
              case llvm::CmpInst::Predicate::ICMP_SGE  :
                constraints[2] = " >= " ;
                break;
              case llvm::CmpInst::Predicate::FCMP_OLT :
              case llvm::CmpInst::Predicate::ICMP_SLT  :
                constraints[2] = " < " ;
                break;
              case llvm::CmpInst::Predicate::FCMP_OLE :
              case llvm::CmpInst::Predicate::ICMP_SLE  :
                constraints[2] = " <= " ;
                break; 
              case llvm::CmpInst::Predicate::FCMP_ONE :
              case llvm::CmpInst::Predicate::ICMP_NE  :
                constraints[2] = " != " ;
                break; 
              default :  
                constraints[2] = " " + std::to_string(pred_enum) + " unknown " ;
            }


            // Print the constraint in human readable form
            errs() << "Constraint: " << constraints[0] << constraints[2] << constraints[1] << "\n";

          }
        }
      }
      return false;
    }

    // We don't modify the program, so we preserve all analyses.
    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
    }
  };
}

char findConstraints::ID = 0;
static RegisterPass<findConstraints> Y("findConstraints", "Constraint Finding Pass");
