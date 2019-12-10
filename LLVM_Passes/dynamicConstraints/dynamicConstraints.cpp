#include <string>
#include <vector>
#include <sstream> 
#include <cstdint>

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Type.h"


using namespace llvm;

#define DEBUG_TYPE "dynamicConstraints"

namespace {
  // findConstraints - Finds all constraints in a program.
  struct insertConstraints : public FunctionPass 
  {
    // Pass identification, replacement for typeid
    static char ID; 
    
    // LLVM pass initilizer
    insertConstraints() : FunctionPass(ID) 
    {
      // Print out that you are starting
      errs() << "Generating Constraints\n\n" ;
    }

    void insert_err()
    {
      errs() << "Working!!!!!!!!!!!!!!!!!!!!!!\n" ;
    }

    // For each function
    bool runOnFunction(Function &F) override 
    {
      // Create a pointer to the function "print_constraints" returns void, args are char*
      LLVMContext &Context = F.getContext();
      Constant *log_bool = F.getParent()->getOrInsertFunction("print_boolean", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getInt1Ty(Context));
      Constant *log_string = F.getParent()->getOrInsertFunction("print_string", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getInt8PtrTy(Context));
      Constant *log_i32 = F.getParent()->getOrInsertFunction("print_i32", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getInt32Ty(Context));
      Constant *log_i64 = F.getParent()->getOrInsertFunction("print_i64", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getInt64Ty(Context));
      Constant *log_float = F.getParent()->getOrInsertFunction("print_float", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getFloatTy(Context));
      Constant *log_double = F.getParent()->getOrInsertFunction("print_double", Type::getVoidTy(Context), Type::getInt8PtrTy(Context), Type::getDoubleTy(Context));
      
      // Create two vectors to keep track of variable names and addresses
      std::vector<llvm::Value*> memory_address;
      std::vector<std::string> variable_name;

      // Used to check if we have not found the result of a an IcmpInst
      bool unchecked_cmp = false ;

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

          if ( isa<BranchInst>(I) && (unchecked_cmp == true))
          {
            // Get the value of the brach instruction
            BranchInst *bi = dyn_cast<BranchInst>(I) ;

            // Print out the result of the compare:
            IRBuilder<> builder(bi);
            std::string prefix_str = "Predicate Evaluation";
            llvm::Value* prefix_str_v = builder.CreateGlobalStringPtr(prefix_str.c_str());
            llvm::Value* pred_res = bi->getCondition() ;

            // Add the arguments to a vector
            std::vector< llvm::Value* > args_bool ;
            args_bool.push_back(prefix_str_v) ;
            args_bool.push_back(pred_res) ;
            llvm::ArrayRef< llvm::Value* > argsRef_bool(args_bool) ;

            // Call the string print function
            builder.CreateCall(log_bool, argsRef_bool) ;

            // Set that we have checked the compare instruction
            unchecked_cmp = false;
          }

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

            // Build the constraints and print them out
            IRBuilder<> builder(cp);
            std::string prefix_str = "Constraint Hit";
            llvm::Value* prefix_str_v = builder.CreateGlobalStringPtr(prefix_str.c_str());
            std::string con_str = constraints[0] + constraints[2] + constraints[1];
            llvm::Value* con_str_v = builder.CreateGlobalStringPtr(con_str.c_str());

            // Add the arguments to a vector
            std::vector< llvm::Value* > args_str ;
            args_str.push_back(prefix_str_v) ;
            args_str.push_back(con_str_v) ;
            llvm::ArrayRef< llvm::Value* > argsRef_str(args_str) ;

            // Call the string print function
            builder.CreateCall(log_string, argsRef_str) ;


            // Print out the value of the operands
            for (unsigned op_num = 0; op_num <= cp->getNumOperands() - 1; op_num++) 
            {
              IRBuilder<> builder(cp) ;
              std::string prefix_str = "Operand (" + constraints[op_num] + ")";
              llvm::Value* prefix_str_v = builder.CreateGlobalStringPtr(prefix_str.c_str()) ;
              llvm::Value* op = cp->getOperand(op_num) ;

              // Add the arguments to a vector
              std::vector< llvm::Value* > args_int ;
              args_int.push_back(prefix_str_v) ;
              args_int.push_back(op) ;
              llvm::ArrayRef< llvm::Value* > argsRef_int(args_int) ;

              // Check the type of the operand:
              if (op->getType()->getTypeID() == llvm::Type::TypeID::FloatTyID)
              {
                builder.CreateCall(log_float, argsRef_int) ;
              }
              else if (op->getType()->getTypeID() == llvm::Type::TypeID::DoubleTyID)
              {
                builder.CreateCall(log_double, argsRef_int) ;
              }
              else if (op->getType()->getTypeID() == llvm::Type::TypeID::IntegerTyID)
              {
                if (op->getType()->isIntegerTy(32))
                {
                  // Call the int print function
                  builder.CreateCall(log_i32, argsRef_int) ;
                }
                else if (op->getType()->isIntegerTy(64))
                {
                  // Call the int print function
                  builder.CreateCall(log_i64, argsRef_int) ;
                } 

              }
              else
              {
                errs() << "Unknown type (" << op->getType()->getTypeID() << ")\n" ;
              } 
            }

            // Set that we need to evaluate the next branch instruction
            // You could probably do this by checking converting the instruction to a value and evaluating that, I just couldnt figure out how to do that :( .....
            unchecked_cmp = true;
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

char insertConstraints::ID = 0;
static RegisterPass<insertConstraints> X("insertConstraints", "Constraint Finding Pass");










