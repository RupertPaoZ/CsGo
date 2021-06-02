#ifndef __CODEGENERATOR_H__
#define __CODEGENERATOR_H__

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/IR/ValueSymbolTable.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/ExecutionEngine/Interpreter.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Target/TargetMachine.h>

#include <stack>
#include <vector>
#include <memory>
#include <string>
#include <map>

#include "ast.h"

using llvm::BasicBlock;
using llvm::Value;
using llvm::LLVMContext;
using llvm::Module;
using llvm::StructType;
using llvm::Type;

class Block{
public:
    llvm::BasicBlock * block;
    // std::vector<Value*> returnValue;
    // Value * returnValue;
    std::map<std::string, Value*> localVar;
    std::map<std::string, llvm::Type*> types;
    // std::map<string, bool> isFuncArg;
    // std::map<string, std::vector<uint64_t>> arraySizes;
    // Type* returnType;
    // std::vector<BuildInType>

    Block(BasicBlock* _block): block(_block){}
};

class CodeGenerator{
private:
    std::vector<Block*> blockStack;

public:
    LLVMContext llvmContext;
    llvm::IRBuilder<> builder;
    std::unique_ptr<Module> theModule;
    llvm::Function *printf, *scanf;
    std::map<std::string, llvm::Type*> funcStack;

    CodeGenerator(): builder(llvmContext){
        theModule = std::unique_ptr<Module>(new Module("main", this->llvmContext));
    }

    llvm::Type* getLLVMType(BuildInType type){
        switch(type){
            case CG_INTEGER : return llvm::Type::getInt32Ty(llvmContext);
            case CG_FLOAT : return llvm::Type::getDoubleTy(llvmContext);
            case CG_CHAR : return llvm::Type::getInt8Ty(llvmContext);
            case CG_STRING : return Type::getInt8PtrTy(llvmContext); // todo
            case CG_VOID : return llvm::Type::getVoidTy(llvmContext);
            case CG_LONG : return llvm::Type::getInt64Ty(llvmContext);
        }
        //todo: error, no such type
    }

    llvm::Type* getLLVMPtrType(BuildInType type){
        switch(type){
            case CG_INTEGER : return llvm::Type::getInt32PtrTy(llvmContext);
            case CG_FLOAT : return llvm::Type::getDoublePtrTy(llvmContext);
            case CG_CHAR : return llvm::Type::getInt8PtrTy(llvmContext);
            // case CG_STRING : return Type::getInt8PtrTy(llvmContext); // todo
            // case CG_VOID : return llvm::Type::getVoidTy(llvmContext);
        }
        //todo: error, no such type
    }

    Value* getSymValue(std::string name){
        for(auto current=blockStack.rbegin(); current!=blockStack.rend(); current++){
            if((*current)->localVar.find(name) != (*current)->localVar.end()){
                return (*current)->localVar[name];
            }
        }
        return nullptr;
    }

    void setSymValue(std::string name, Value* value){
        blockStack.back()->localVar[name] = value;
    }

    llvm::Type* getSymType(std::string name){
        for(auto current=blockStack.rbegin(); current!=blockStack.rend(); current++){
            if((*current)->types.find(name) != (*current)->types.end()){
                return (*current)->types[name];
            }
        }
        return getLLVMType(CG_VOID);
    }

    void setSymType(std::string name, llvm::Type* type){
        blockStack.back()->types[name] = type;
    }

    void addFunc(std::string name, llvm::Type* type){
        funcStack[name] = type;
    }

    llvm::Type* getReturnType(std::string name){
        return funcStack[name];
    }

    // void setReturnValue(Value* ret){
    //     blockStack.back()->returnValue = ret;
    // }

    // Value* getReturnValue(){
    //     return blockStack.back()->returnValue;
    // }

    void pushBlock(BasicBlock* block){
        Block* newBlock = new Block(block);
        blockStack.push_back(newBlock);
    }

    void popBlock(){
        Block* endBlock = blockStack.back();
        blockStack.pop_back();
        delete endBlock;
    }

    void createPrintf()
    {
        std::vector<llvm::Type*> arg_types;
        arg_types.push_back(builder.getInt8PtrTy());
        auto printf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
        auto func = llvm::Function::Create(printf_type, llvm::Function::ExternalLinkage, llvm::Twine("printf"), theModule.get());
        func->setCallingConv(llvm::CallingConv::C);
        printf = func;
    }
    
    void createScanf()
    {
        std::vector<llvm::Type*> arg_types;
        arg_types.push_back(builder.getInt8PtrTy());
        auto scanf_type = llvm::FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
        auto func = llvm::Function::Create(scanf_type, llvm::Function::ExternalLinkage, llvm::Twine("scanf"), theModule.get());
        func->setCallingConv(llvm::CallingConv::C);
        scanf = func;
    }

    void generateCode(Program &);
};

// Value* LogErrorV(const char* str);
// Value* LogErrorV(string str);

#endif
