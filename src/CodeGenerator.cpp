#include "CodeGenerator.h"

using namespace llvm;
using namespace std;

void CodeGenerator::generateCode(Program & root){
    
    //std::vector<Type*> sysArgs;
    FunctionType* mainFuncType = FunctionType::get(Type::getVoidTy(this->llvmContext), false);
    Function* mainFunc = Function::Create(mainFuncType, GlobalValue::ExternalLinkage, "main");
    BasicBlock* block = BasicBlock::Create(this->llvmContext, "entry");
    createPrintf();
    createScanf();

    pushBlock(block);
    
    root.Generate(*this);
    //cout<<"begin"<<endl;
    popBlock();

    //cout << "Code generate success" << endl;

    llvm::legacy::PassManager passManager;
    passManager.add(createPrintModulePass(outs()));
    passManager.run(*(this->theModule.get()));
    // this->theModule->print(llvm::outs(), nullptr);
    // this->theModule->dump();
    return;
}

llvm::Value* NodeWithChildren::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* LogOp::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* RelOp::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* AddOp::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* MulOp::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Program::Generate(CodeGenerator &codeGen){
    //cout<<"Program!"<<endl;
    DeclList decl = *(this->getDeclList());
    for(auto& thisDecl : decl){
        thisDecl->Generate(codeGen);
    }
    return nullptr;
}

llvm::Value* SysType::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* TypeSpec::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Identifier::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Param::Generate(CodeGenerator &codeGen){
    //cout<<"param generate!"<<endl;
    BuildInType inType = this->typeSpec->getType();
    
    //todo: array type
    Value* inst = nullptr;
    if(this->isArray){
        llvm::Type* type = codeGen.getLLVMPtrType(inType);
        //cout<<"param : array type"<<endl;
        inst = codeGen.builder.CreateAlloca(type);
        cout<<"alloc success"<<endl;
        codeGen.setSymType(this->identifier->getID(), type);
    }else{
        llvm::Type* type = codeGen.getLLVMType(inType);
        inst = codeGen.builder.CreateAlloca(type);
        codeGen.setSymType(this->identifier->getID(), type);
    }
    // codeGen.setSymValue(this->identifier->getID(), inst);
    return inst;
}

llvm::Value* Params::Generate(CodeGenerator &codeGen){
    // if(this->isVoid)
    //     return nullptr;
    // cout<<"params generate!"<<endl;
    // for(auto &thisParam: *(this->paramList)){
    //     thisParam->Generate(codeGen);
    // }
    return nullptr;
}

llvm::Value* Statement::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Factor::Generate(CodeGenerator &codeGen){
    //cout<<"factor "<<this->factorType<<endl;
    switch(this->factorType){
        case FT_SIMPLEEXPR: return this->simpleExpr->Generate(codeGen);
        case FT_VAR: if(this->variable->isPointer()) return this->variable->Generate(codeGen);  //todo error
                     else return codeGen.builder.CreateLoad(this->variable->Generate(codeGen));
        case FT_CALL: return this->call->Generate(codeGen);
        case FT_FLOAT: return this->nfloat->Generate(codeGen);
        case FT_INTEGER: return this->integer->Generate(codeGen);
        case FT_CHAR: return this->nchar->Generate(codeGen);
        case FT_STRING: return this->nstring->Generate(codeGen);
    }
    //todo : error
}

llvm::Value* Term::Generate(CodeGenerator &codeGen){
    Value* right = this->factor->Generate(codeGen);
    if(this->mulOp==nullptr) return right;
    Value* left = this->term->Generate(codeGen);
    
    //todo : type check
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(!left || !right){
        return nullptr;
    }
    switch(this->mulOp->getOpType()){
        case MT_MUL: return isFloat ? codeGen.builder.CreateFMul(left, right, "multmpf") : codeGen.builder.CreateMul(left, right, "multmpi");
        case MT_DIV: return isFloat ? codeGen.builder.CreateFDiv(left, right, "divmpf") : codeGen.builder.CreateSDiv(left, right, "divmpi");
        //case MT_DIV: return codeGen.builder.CreateSDiv(left, right, "tmpDiv");
        case MT_MOD: return nullptr; //todo: mod
    }
    //todo:error
}

llvm::Value* Call::Generate(CodeGenerator &codeGen){
    string name = this->identifier->getID();
    llvm::Function* function;
    bool isScanf;
    if(name=="printf"){
        function = codeGen.printf;
        isScanf = false;
    }else if(name=="scanf"){
        function = codeGen.scanf;
        isScanf = true;
    }else{
        function = codeGen.theModule->getFunction(this->identifier->getID());
        isScanf = false;
    }
    //todo:error no such function
    // if (function == nullptr)
    // {
    //     throw domain_error("[ERROR] Function not defined: " + this->function->getName());
    // }
    vector<llvm::Value*> argsv;
    //todo:error args number check
    if(this->args->getArgList()!=nullptr){
        ArgList argsn = *(this->args->getArgList());
        //argsv.push_back(codeGen.builder.CreateGlobalStringPtr("%d"));
        for(auto &thisArg : argsn){
            Value* value;
            bool isString;
            Variable* var = thisArg->getVariable(isString);
            if(isScanf && !isString){
                //todo:error
                if(var==nullptr) cout<<"null!"<<endl;
                //cout<<"scanf gen"<<endl;
                value = var->Generate(codeGen);
                //cout<<"null1"<<endl;
            }
            else { value = thisArg->Generate(codeGen); /*cout<<"not scanf gen!"<<endl;*/}
            argsv.push_back(value);
            // if(value->getType()->isPointerTy()) cout<<"pointer!"<<endl;
            if( !argsv.back() ){        // if any argument codegen fail
                return nullptr;
            }
        }
    }
    //cout << "call gen end"<<endl;
    llvm::Value *res = codeGen.builder.CreateCall(function, argsv);
    return res;
}

llvm::Value* Variable::Generate(CodeGenerator &codeGen){
    //cout<<"variable gen!"<<endl;
    if(this->isArray){
        vector<Value*> indices;
        indices.push_back(ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
        Value* index;
        if(!this->isPointer()){
            index = this->simpleExpr->Generate(codeGen);
            indices.push_back(index);
        }else{
            indices.push_back(ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
        }
        auto inst = codeGen.getSymValue(this->identifier->getID());
        auto type = codeGen.getSymType(this->identifier->getID());
        if(type->isArrayTy()){
            auto ptr = codeGen.builder.CreateInBoundsGEP(inst, indices);
            return ptr;
        }else{
            if(this->isPointer()) return inst;
            auto elementType = inst->getType()->getPointerElementType();
            int length;
            BuildInType ptrtype;
            if(elementType->isIntegerTy(32)){
                length = 4;
                ptrtype = CG_INTEGER;
            }else if(elementType->isIntegerTy(8)){
                length = 1;
                ptrtype = CG_CHAR;
            }else if(elementType->isDoubleTy()){
                length = 8;
                ptrtype = CG_FLOAT;
            }
            Value* begin = codeGen.builder.CreatePtrToInt(inst, codeGen.getLLVMType(CG_LONG));
            Value* offset = codeGen.builder.CreateZExt(index, codeGen.getLLVMType(CG_LONG));
            Value* realoffset = codeGen.builder.CreateMul(offset, codeGen.builder.getInt64(length));
            Value* add = codeGen.builder.CreateAdd(begin, realoffset);
            auto ptr = codeGen.builder.CreateIntToPtr(add, codeGen.getLLVMPtrType(ptrtype));
            return ptr;
        }
    }else{
        Value* value = codeGen.getSymValue(this->identifier->getID());
        // return codeGen.builder.CreateLoad(value);
        return value;
    }
}

llvm::Value* UnderScore::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* LogicExpr::Generate(CodeGenerator &codeGen){
    Value* right = this->simpleExpr->Generate(codeGen);
    if(this->logOp==nullptr) return right;
    Value* left = this->logicExpr->Generate(codeGen);
    
    //todo : type check
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(!left || !right){
        return nullptr;
    }
    if(!this->logOp->getOpType()){
        return codeGen.builder.CreateAnd(left, right, "andtmp");
    }
    // else{
    //     return isFloat ? codeGen.builder.CreateFSub(left, right, "subtmpf") : codeGen.builder.CreateSub(left, right, "subtmpi");
    // }
    //todo: error
}

llvm::Value* AddiExpr::Generate(CodeGenerator &codeGen){
    Value* right = this->term->Generate(codeGen);
    if(this->addOp==nullptr) return right;
    Value* left = this->addiExpr->Generate(codeGen);
    
    //todo : type check
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(!left || !right){
        return nullptr;
    }
    if(this->addOp->getOpType()){
        return isFloat ? codeGen.builder.CreateFAdd(left, right, "addtmpf") : codeGen.builder.CreateAdd(left, right, "addtmpi");
    }else{
        return isFloat ? codeGen.builder.CreateFSub(left, right, "subtmpf") : codeGen.builder.CreateSub(left, right, "subtmpi");
    }
    //todo: error
}

llvm::Value* SimpleExpr::Generate(CodeGenerator &codeGen){
    Value* left = this->addiExpr1->Generate(codeGen);
    if(this->relOp==nullptr) return left;
    Value* right = this->addiExpr2->Generate(codeGen);
    //todo : type check
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(!left || !right){
        return nullptr;
    }
    switch(this->relOp->getOpType()){
        case REL_LE : return codeGen.builder.CreateICmpSLE(left, right, "tmpSLE");
        case REL_LT : return codeGen.builder.CreateICmpSLT(left, right, "tmpSLT");
        case REL_GE : return codeGen.builder.CreateICmpSGE(left, right, "tmpSGE");
        case REL_GT : return codeGen.builder.CreateICmpSGT(left, right, "tmpSGT");
        case REL_EQ : return codeGen.builder.CreateICmpEQ(left, right, "tmpEQ");
        case REL_UNE : return codeGen.builder.CreateICmpNE(left, right, "tmpNE");
    }
    //todo: error
}

llvm::Value* Decl::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* LocalDecls::Generate(CodeGenerator &codeGen){
    for(auto &thisVar: *(this->localList)){
        thisVar->Generate(codeGen);
    }
    return nullptr;
}

llvm::Value* Stmts::Generate(CodeGenerator &codeGen){
    for(auto &thisStmt : *(this->stmtList)){
        thisStmt->Generate(codeGen);
    }
    return nullptr;
}

llvm::Value* ComStmt::Generate(CodeGenerator &codeGen){
    //cout<<"stmt ge"<<endl;
    this->localDecls->Generate(codeGen);
    //cout<<"stmt ge"<<endl;
    this->stmts->Generate(codeGen);
    return nullptr;
}

llvm::Value* Vars::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Exprs::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* ExprStmt::Generate(CodeGenerator &codeGen){
    //cout<<"expr stmt!"<<endl;
    VarList var = *(this->vars->getVarlist());
    ExprList expr = *(this->exprs->getExprlist());
    //todo: error(different terms)
    auto thisVar = var.begin();
    auto thisExpr = expr.begin();
    //cout<<"1"<<endl;
    for(;thisVar!=var.end(), thisExpr!=expr.end();thisExpr++){
        // string name = (*thisVar)->getID();
        // cout<<name<<endl;
        // if(name=="") continue;
        // codeGen.builder.CreateStore((*thisExpr)->Generate(codeGen), codeGen.getSymValue(name));
        auto right = (*thisExpr)->Generate(codeGen);
        string funcname = (*thisExpr)->getCallName();
        if(funcname!="" && funcname != "printf" && funcname != "scanf"){
            //cout<<"struct!"<<endl;
            int num = codeGen.getReturnType(funcname)->getStructNumElements();
            for(int i=0;i<num;i++){
                string name = (*thisVar)->getID();
                if(name=="") continue;
                Value* structValue = codeGen.builder.CreateExtractValue(right, (uint64_t)i);
                codeGen.builder.CreateStore(structValue, (*thisVar)->Generate(codeGen));
                thisVar++;
            }
        }else{
            string name = (*thisVar)->getID();
            //cout<<name<<endl;
            if(name=="") continue;
            codeGen.builder.CreateStore(right, (*thisVar)->Generate(codeGen));
            thisVar++;
        }
    }
    //cout<<"out expr stmt!"<<endl;
    return nullptr;
}

llvm::Value* FuncStmt::Generate(CodeGenerator &codeGen){
    this->call->Generate(codeGen);
    return nullptr;
}

llvm::Value* SelectStmt::Generate(CodeGenerator &codeGen){
    //cout << "Generating if statement" << endl;
    Value* condValue = this->logicExpr->Generate(codeGen);
    if( !condValue )
        return nullptr;

    // condValue = CastToBoolean(context, condValue);

    Function* theFunction = codeGen.builder.GetInsertBlock()->getParent();      // the function where if statement is in

    BasicBlock *ifbody = BasicBlock::Create(codeGen.llvmContext, "ifbody", theFunction);
    BasicBlock *elsebody = BasicBlock::Create(codeGen.llvmContext, "elsebody");
    BasicBlock *end = BasicBlock::Create(codeGen.llvmContext, "end");

    if( this->isIfelse ){
        codeGen.builder.CreateCondBr(condValue, ifbody, elsebody);
    } else{
        codeGen.builder.CreateCondBr(condValue, ifbody, end);
    }

    codeGen.builder.SetInsertPoint(ifbody);

    codeGen.pushBlock(ifbody);

    this->ifStmt->Generate(codeGen);

    codeGen.popBlock();

    ifbody = codeGen.builder.GetInsertBlock();

    if( ifbody->getTerminator() == nullptr ){       //
        codeGen.builder.CreateBr(end);
    }

    if( this->isIfelse ){
        theFunction->getBasicBlockList().push_back(elsebody);    //
        codeGen.builder.SetInsertPoint(elsebody);            //

        codeGen.pushBlock(elsebody);

        this->elStmt->Generate(codeGen);

        codeGen.popBlock();

        elsebody = codeGen.builder.GetInsertBlock();
        if(elsebody->getTerminator() == nullptr){
            codeGen.builder.CreateBr(end);
        }
    }

    theFunction->getBasicBlockList().push_back(end);        //
    codeGen.builder.SetInsertPoint(end);        //

    return nullptr;
}

llvm::Value* IterStmt::Generate(CodeGenerator &codeGen){
    Function* theFunction = codeGen.builder.GetInsertBlock()->getParent();

    BasicBlock *cond = BasicBlock::Create(codeGen.llvmContext, "cond", theFunction);
    BasicBlock *body = BasicBlock::Create(codeGen.llvmContext, "body");
    BasicBlock *end = BasicBlock::Create(codeGen.llvmContext, "end");

    codeGen.builder.CreateBr(cond);
    codeGen.builder.SetInsertPoint(cond);
    
    Value* condValue = this->logicExpr->Generate(codeGen);
    
    if( !condValue )
        return nullptr;

    // condValue = CastToBoolean(context, condValue);

    // fall to the block
    codeGen.builder.CreateCondBr(condValue, body, end);

    theFunction->getBasicBlockList().push_back(body);
    codeGen.builder.SetInsertPoint(body);

    codeGen.pushBlock(body);

    this->statement->Generate(codeGen);

    codeGen.popBlock();

    body = codeGen.builder.GetInsertBlock();
    if(body->getTerminator() == nullptr){
        codeGen.builder.CreateBr(cond);
    }

    // insert the after block
    theFunction->getBasicBlockList().push_back(end);
    codeGen.builder.SetInsertPoint(end);

    return nullptr;
}

llvm::Value* RetStmt::Generate(CodeGenerator &codeGen){
    //cout<<"ret stmt!"<<endl;
    Function* function = codeGen.builder.GetInsertBlock()->getParent();
    Type* retType = function->getReturnType();
    if(retType->isVoidTy()) return codeGen.builder.CreateRetVoid();
    ExprList expr = *(this->exprs->getExprlist());
    auto inst = codeGen.builder.CreateAlloca(retType);
    if(retType->isStructTy()){
        for(auto thisExpr=expr.begin(); thisExpr!=expr.end(); thisExpr++){
            Value* thisValue = (*thisExpr)->Generate(codeGen);
            vector<Value*> indices;
            indices.push_back(ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
            indices.push_back(ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), (uint64_t)distance(expr.begin(),thisExpr), false));

            auto ptr = codeGen.builder.CreateInBoundsGEP(inst, indices);
            codeGen.builder.CreateStore(thisValue, ptr);
        }    
    }else{
        codeGen.builder.CreateStore(expr.at(0)->Generate(codeGen), inst);
    }
    Value* returnValue = codeGen.builder.CreateLoad(inst);
    // codeGen.setReturnValue(returnValue);
    codeGen.builder.CreateRet(returnValue);
    return returnValue;
}

llvm::Value* Args::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Void::Generate(CodeGenerator &codeGen){
    return nullptr;
}

llvm::Value* Integer::Generate(CodeGenerator &codeGen){
    return codeGen.builder.getInt32(this->value);
}

llvm::Value* Float::Generate(CodeGenerator &codeGen){
    return ConstantFP::get(codeGen.builder.getDoubleTy(), this->value);
}

llvm::Value* String::Generate(CodeGenerator &codeGen){
    return codeGen.builder.CreateGlobalStringPtr(this->value);
}

llvm::Value* Char::Generate(CodeGenerator &codeGen){
    return codeGen.builder.getInt8(this->value);
}

llvm::Value* VarDecl::Generate(CodeGenerator &codeGen){
    BuildInType inType = this->typeSpec->getType();
    llvm::Type* type = codeGen.getLLVMType(inType);
    //todo: array type
    Value* inst = nullptr;
    if(this->isArray){
        ArrayType* arrayType = ArrayType::get(type, (uint64_t)(this->integer->getValue()));
        inst = codeGen.builder.CreateAlloca(arrayType);
        codeGen.setSymType(this->identifier->getID(), arrayType);
    }else{
        inst = codeGen.builder.CreateAlloca(type);
        codeGen.setSymType(this->identifier->getID(), type);
    }
    codeGen.setSymValue(this->identifier->getID(), inst);
    return inst;
}

llvm::Value* FuncDecl::Generate(CodeGenerator &codeGen){
    //cout<<"FuncDecl!"<<endl;
    vector<Type*> argType, retType;
    
    if(!this->inParams->getVoid()){
        ParamList inParamList = *(this->inParams->getParamList());
        for(auto &param: inParamList){
            if(param->getIsArray()){
                argType.push_back(codeGen.getLLVMPtrType(param->getType()));
            }else{
                argType.push_back(codeGen.getLLVMType(param->getType()));
            }
        }
    } 
    if(!this->outParams->getVoid()){
        ParamList outParamList = *(this->outParams->getParamList());
        for(auto &param: outParamList){
            retType.push_back(codeGen.getLLVMType(param->getType()));
        }
    }
    FunctionType*  functionType;
    Function* function;
    string funcName = this->identifier->getID();
    Type* type;
    if(this->outParams->getVoid()){
        type = codeGen.getLLVMType(CG_VOID);    
    }else if(this->identifier->getID()=="main"){
        //todo:error
        type = retType.at(0);
    }
    else{
        auto structType = StructType::create(codeGen.llvmContext, "retType");
        structType->setBody(retType);
        type = structType;
    }
    //cout<<2<<endl;
    functionType = FunctionType::get(type, argType, false);
    //cout<<3<<endl;
    function = Function::Create(functionType, GlobalValue::ExternalLinkage, this->identifier->getID(), codeGen.theModule.get());
    BasicBlock* basicBlock = BasicBlock::Create(codeGen.llvmContext, "entry", function, nullptr);
    
    codeGen.builder.SetInsertPoint(basicBlock);
    codeGen.pushBlock(basicBlock);
    codeGen.addFunc(funcName, type);

    if(!this->inParams->getVoid()){
        ParamList inParamList = *(this->inParams->getParamList());
        auto thisParam = inParamList.begin();
        auto thisValue = function->args().begin();
        for(;thisParam!=inParamList.end();thisParam++, thisValue++){
            thisValue->setName((*thisParam)->getName());
            Value* alloc = (*thisParam)->Generate(codeGen);
            codeGen.builder.CreateStore(thisValue, alloc, false);
            if((*thisParam)->getIsArray()){
                codeGen.setSymValue((*thisParam)->getName(), codeGen.builder.CreateLoad(alloc));
            }
            else codeGen.setSymValue((*thisParam)->getName(), alloc);
        }
    }

    this->comStmt->Generate(codeGen);

    codeGen.popBlock();
    return function;
}