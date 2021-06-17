### 中间代码生成

#### LLVM概述

LLVM(Low Level Virtual Machine)是以C++编写的编译器基础设施，包含一系列模块化的编译器组件和工具教练用俩开发编译器前端和后端。LLVM起源于2000年伊利诺伊大学Vikram Adve和Chris Lattner的研究，它是为了任意一种编程语言而写成的程序，利用虚拟技术创造出编译阶段、链接阶段、运行阶段以及闲置阶段的优化，目前支持Ada、D语言、Fortran、GLSL、Java字节码、Swift、Python、Ruby等十多种语言。

- 前端：LLVM最初被用来取代现有于GCC堆栈的代码产生器，许多GCC的前端已经可以与其运行，其中Clang是一个新的编译器，同时支持C、Objective-C以及C++。
- 中间端：LLVM IR是一种类似汇编的底层语言，一种强类型的精简指令集，并对目标指令集进行了抽象。LLVM支持C++中对象形式、序列化bitcode形式和汇编形式。
- 后端：LLVM支持ARM、Qualcomm Hexagon、MPIS、Nvidia并行指令集等多种后端指令集。

#### LLVM IR

LLVM IR是LLVM的核心所在，通过将不同高级语言的前端变换成LLVM IR进行优化、链接后再传给不同目标的后端转换成为二进制代码，前端、优化、后端三个阶段互相解耦，这种模块化的设计使得LLVM优化不依赖于任何源码和目标机器。

![LLVM](.\img\LLVM.png)

##### IR布局

每个IR文件称为一个Module，它是其他所有IR对象的顶级容器，包含了目标信息、全局符号和所依赖的其他模块和符号表等对象的列表，其中全局符号又包括了全局变量、函数声明和函数定义。

函数由参数和多个基本块组成，其中第一个基本块称为entry基本块，这是函数开始执行的起点，另外LLVM的函数拥有独立的符号表，可以对标识符进行查询和搜索。

每一个基本块包含了标签和各种指令的集合，标签作为指令的索引用于实现指令间的跳转，指令包含Phi指令、一般指令以及终止指令等。

![LLVM_layout](.\img\LLVM_layout.png)

##### IR上下文环境

- LLVM::Context：提供用户创建变量等对象的上下文环境，尤其在多线程环境下至关重要
- LLVM::IRBuilder：提供创建LLVM指令并将其插入基础块的API

##### IR核心类

![LLVMclass](.\img\LLVMclass.png)

- llvm::Value表示一个类型的值，具有一个llvm::Type*成员和一个use list，前者指向值的类型类，后者跟踪使用了该值的其他对象，可以通过迭代器进行访问。

  - 值的存取分别可以通过llvm::LoadInst和llvm::StoreInst实现，也可以借助IRBuilder的CreateLoad和CreateStore实现。
- llvm::Type表示类型类，LLVM支持17种数据类型，可以通过Type ID判断类型：

```C++
 enum TypeID {
    // PrimitiveTypes - make sure LastPrimitiveTyID stays up to date.
    VoidTyID = 0,    ///<  0: type with no size
    HalfTyID,        ///<  1: 16-bit floating point type
    FloatTyID,       ///<  2: 32-bit floating point type
    DoubleTyID,      ///<  3: 64-bit floating point type
    X86_FP80TyID,    ///<  4: 80-bit floating point type (X87)
    FP128TyID,       ///<  5: 128-bit floating point type (112-bit mantissa)
    PPC_FP128TyID,   ///<  6: 128-bit floating point type (two 64-bits, PowerPC)
    LabelTyID,       ///<  7: Labels
    MetadataTyID,    ///<  8: Metadata
    X86_MMXTyID,     ///<  9: MMX vectors (64 bits, X86 specific)
    TokenTyID,       ///< 10: Tokens

    // Derived types... see DerivedTypes.h file.
    // Make sure FirstDerivedTyID stays up to date!
    IntegerTyID,     ///< 11: Arbitrary bit width integers
    FunctionTyID,    ///< 12: Functions
    StructTyID,      ///< 13: Structures
    ArrayTyID,       ///< 14: Arrays
    PointerTyID,     ///< 15: Pointers
    VectorTyID       ///< 16: SIMD 'packed' format, or other vector type
  };
```

- llvm::Constant表示各种常量的基类，包括ConstantInt整形常量、ConstantFP浮点型常量、ConstantArray数组常量、ConstantStruct结构体常量等。

#### IR生成

##### 运行环境设计

LLVM IR的生成依赖上下文环境，我们构造了CodeGenerator类来保存环境，在递归遍历AST节点的时候传递CodeGenerator的实例进行每个节点的IR生成。CodeGenerator包括的环境配置：

- 全局的上下文变量和构造器变量

```C++
LLVMContext llvmContext;
llvm::IRBuilder<> builder;
```

- 公有的模块实例、系统函数、函数栈
  - 模块实例是中间代码顶级容器，用于包含所有变量、函数和指令
  - 系统函数为``printf``和``scanf``，用于从标准输出和标准输入
  - 函数栈用于存储所有函数的返回值类型。由于函数可以返回多值，所以本语言实现过程中函数返回均以结构体形式返回（void返回、系统函数、main函数返回值除外）。用函数栈存储返回值类型有助于快速构造出对应的结构体类型。

```c++
std::unique_ptr<Module> theModule;
Function *printf, *scanf;
std::map<std::string, Type*> funcStack; 
```

* 符号表：由Block类的堆栈组成，而Block类对应语言中的一个代码块，存储llvm基础块、局部变量值以及类型。此处局部变量的值均存储指针的值，即``llvm::CreateAlloca``分配的空间地址；如果本身是指针则存储指针本身。

```c++
class Block{
public:
    BasicBlock * block;
    std::map<std::string, Value*> localVar;
    std::map<std::string, Type*> types;

    Block(BasicBlock* _block): block(_block){}
};
```

此外还需要一些类方法来快速修改运行环境：

* 创建和删除基本块
* 获取、设置变量的值以及类型：下面是获取变量值的方法。在基本块的堆栈中自顶向下寻找对应的变量名，符合语言中同变量名的就近原则。获取变量类型同理。在设置变量值或类型时，只需要在当前块（即栈顶）设置即可

```c++
    // Get the pointer of a variable
    Value* getSymValue(std::string name){
        // Search from the top of the stack
        // So if there are variables of the same name, we will get the closest one 
        for(auto current=blockStack.rbegin(); current!=blockStack.rend(); current++){
            if((*current)->localVar.find(name) != (*current)->localVar.end()){
                return (*current)->localVar[name];
            }
        }
        return nullptr;
    }

    // Store the pointer of a variable in the current block
    void setSymValue(std::string name, Value* value){
        blockStack.back()->localVar[name] = value;
    }
```

* 增加函数、获取函数返回值
* 创建系统函数：创建过程与一般函数类似。首先需要定义输入参数类型：第一个参数为8位int指针（字符串）以及可变数量的后续参数。定义返回类型：32位int。创建对应的函数类型和实例。

````c++
    // Create printf
    void createPrintf()
    {
        // Input type, the first argument must be a string
        std::vector<Type*> arg_types;
        arg_types.push_back(builder.getInt8PtrTy());
        // Return type is int32, "true" means alterable number of arguments
        auto printf_type = FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
        auto func = Function::Create(printf_type, Function::ExternalLinkage, llvm::Twine("printf"), theModule.get());
        func->setCallingConv(llvm::CallingConv::C);
        printf = func;
    }
    
    // Create scanf
    void createScanf()
    {
        // Input type, the first argument must be a string
        std::vector<Type*> arg_types;
        arg_types.push_back(builder.getInt8PtrTy());
        // Return type is int32, "true" means alterable number of arguments
        auto scanf_type = FunctionType::get(builder.getInt32Ty(), llvm::makeArrayRef(arg_types), true);
        auto func = Function::Create(scanf_type, Function::ExternalLinkage, llvm::Twine("scanf"), theModule.get());
        func->setCallingConv(llvm::CallingConv::C);
        scanf = func;
    }
````

##### 类型系统

系统定义了语言中的类型（CG_INTEGER等），此处需要把语法类型转换成llvm类型，定义了两个函数，分别获取对应的llvm值类型和指针类型：

````c++
    // Transform from CsGo types to llvm types
    Type* getLLVMType(BuildInType type){
        switch(type){
            case CG_INTEGER : return Type::getInt32Ty(llvmContext);
            case CG_FLOAT : return Type::getDoubleTy(llvmContext);
            case CG_CHAR : return Type::getInt8Ty(llvmContext);
            case CG_STRING : return Type::getInt8PtrTy(llvmContext);
            case CG_VOID : return Type::getVoidTy(llvmContext);
            case CG_LONG : return Type::getInt64Ty(llvmContext);
        }
    }

    // Get llvm pointer type of CsGo type
    Type* getLLVMPtrType(BuildInType type){
        switch(type){
            case CG_INTEGER : return Type::getInt32PtrTy(llvmContext);
            case CG_FLOAT : return Type::getDoublePtrTy(llvmContext);
            case CG_CHAR : return Type::getInt8PtrTy(llvmContext);
            case CG_STRING : return PointerType::getUnqual(Type::getInt8PtrTy(llvmContext));
        }
    }
````

##### 常量值获取

可以通过IRBuilder快速获取``llvm::Constant``值。string统一用全局字符串的方式存储。

````c++
// Generate int type
Value* Integer::Generate(CodeGenerator &codeGen){
    return codeGen.builder.getInt32(this->value);
}

// Generate float type, we use double here
Value* Float::Generate(CodeGenerator &codeGen){
    return llvm::ConstantFP::get(codeGen.builder.getDoubleTy(), this->value);
}

// Generate string type, we use global string here
// This is different from string variable, variable needs to be a pointer
// String in the code can be store in global data area
Value* String::Generate(CodeGenerator &codeGen){
    return codeGen.builder.CreateGlobalStringPtr(this->value);
}

// Generate char type, int8
Value* Char::Generate(CodeGenerator &codeGen){
    return codeGen.builder.getInt8(this->value);
}
````

##### 变量定义

变量区分一般变量和数组变量两种情况：

* 一般变量：得到llvm类型后直接使用``llvm::CreateAlloca``分配空间即可
* 数组变量：首先根据元素类型和元素个数获得对应的llvm的array类型，然后分配相应的arraytype空间

分配空间后在运行环境中设置对应的变量值（地址）和类型。

```c++
// Generate IR code for variable declaration
Value* VarDecl::Generate(CodeGenerator &codeGen){
    // Get type
    BuildInType inType = this->typeSpec->getType();
    Type* type = codeGen.getLLVMType(inType);
    Value* inst = nullptr;
    // If it's array, create arraytype
    if(this->isArray){
        llvm::ArrayType* arrayType = llvm::ArrayType::get(type, (uint64_t)(this->integer->getValue()));
        inst = codeGen.builder.CreateAlloca(arrayType);
        codeGen.setSymType(this->identifier->getID(), arrayType);
    }else{
        inst = codeGen.builder.CreateAlloca(type);
        codeGen.setSymType(this->identifier->getID(), type);
    }
    codeGen.setSymValue(this->identifier->getID(), inst);
    return inst;
}
```

##### 变量取值

考虑到在语言中变量取值的目的不同（如赋值表达式右边的变量需要取值，而左边的变量需要取地址），这里的变量取值统一返回地址，而后续其他类可以根据需要选择直接使用地址或者取值。对于一般变量，直接在符号表中获取即可（因为符号表中存储的就是变量地址）。而对于数组类型则比较复杂：

* 首先需要判断是否给定下标。如果没有给定（如函数参数传递中的``a[]``），则下标为0；否则为给定的下标。将第一个下标（0）和当前下标push到一个vector中，便于后面获取指针的函数使用。

````c++
        std::vector<Value*> indices;
        indices.push_back(llvm::ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
        Value* index;
        // If there is no index(eg: a[]), just get the first pointer
        // Otherwise index is need
        if(!this->isPointer()){
            index = this->simpleExpr->Generate(codeGen);
            indices.push_back(index);
        }else{
            indices.push_back(llvm::ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
        }
````

* 然后需要判断数组的类型。因为如果是在该数组定义的块中，由“变量定义”一节可以看到类型是``llvm::ArrayType``；但在其他块中，如在另一个函数里传递了数组变量，按照本语言函数参数的传参方法，传递的是地址首指针。对于这两种类型需要采取不同的方法获取元素地址。

  * 如果是arraytype，直接调用IRBuilder的``CreateInBoundsGEP``方法即可，需要传递arraytype的变量指针（可从符号表获得）和index的vector。

  * 如果是指针（首地址），则需要进行指针运算。如果没有指定index，则直接返回首地址即可；否则将地址转化为64位int（``CreatePtrToInt``），再与offset相加，再将相加后的64位int转化为指针（``CreateIntToPtr``）。此处需要注意：``offset=index*size``，而不同元素的size是不同的，所以这里需要获取指针所指向元素的类型（``getPointerElementType()``），再根据类型设置对应的size。此外，本系统中的int常量是用32位int存储的，而指针转化后的int是64位，需要将32位int转化为64位int（``CreateZExt``）才能进行运算。

````c++
        if(type->isArrayTy()){
            // Get pointer with index
            auto ptr = codeGen.builder.CreateInBoundsGEP(inst, indices);
            return ptr;
        }else{
            // If there is no index, just return the first pointer
            if(this->isPointer()) return inst;
            // Otherwise we need to get the pointer from the first pointer and offset
            // Get elementtype to calculate offset
            auto elementType = inst->getType()->getPointerElementType();
            int length;
            BuildInType ptrtype;
            // Get size
            if(elementType->isIntegerTy(32)){
                length = 4;
                ptrtype = CG_INTEGER;
            }else if(elementType->isIntegerTy(8)){
                length = 1;
                ptrtype = CG_CHAR;
            }else if(elementType->isDoubleTy()){
                length = 8;
                ptrtype = CG_FLOAT;
            }else if(elementType->isPointerTy()){
                length = 8;
                ptrtype = CG_STRING;
            }
            // Traverse pointer to int, and add the offset, traverse int to pointer
            Value* begin = codeGen.builder.CreatePtrToInt(inst, codeGen.getLLVMType(CG_LONG));
            Value* offset = codeGen.builder.CreateZExt(index, codeGen.getLLVMType(CG_LONG));
            Value* realoffset = codeGen.builder.CreateMul(offset, codeGen.builder.getInt64(length));
            Value* add = codeGen.builder.CreateAdd(begin, realoffset);
            auto ptr = codeGen.builder.CreateIntToPtr(add, codeGen.getLLVMPtrType(ptrtype));
            return ptr;
````

##### 操作数取值

操作数是语法中定义的Factor类，是运算表达式、赋值表达式等语句中的操作数。操作数可以是常量、变量、函数调用或一个表达式。

* 对于除变量以外的形式，直接调用对应类的Generate方法即可；
* 对于变量类型，由“变量取值”一节可以看到，变量取值返回的是地址，而操作数取值需要判断：如果是指针类型，则直接传递指针；否则需要将返回的地址的存储的值取出（``CreateLoad``）。

````c++
// Generate IR code for Factor
Value* Factor::Generate(CodeGenerator &codeGen){
    switch(this->factorType){
        case FT_SIMPLEEXPR: return this->simpleExpr->Generate(codeGen);
        // If this is a pointer type, return a pointer; otherwise load and return the value 
        case FT_VAR: if(this->variable->isPointer()) return this->variable->Generate(codeGen);
                     else return codeGen.builder.CreateLoad(this->variable->Generate(codeGen));
        case FT_CALL: return this->call->Generate(codeGen);
        case FT_FLOAT: return this->nfloat->Generate(codeGen);
        case FT_INTEGER: return this->integer->Generate(codeGen);
        case FT_CHAR: return this->nchar->Generate(codeGen);
        case FT_STRING: return this->nstring->Generate(codeGen);
    }
}
````

##### 二元运算表达式

二元运算表达式是指由规则``term->term mulop factor | factor ``，``additive_expr->additive_expr addop term | term``，``simple_expr->additive_expr relop additive_expr | additive_expr``，``logic_expr->logic_expr logop simple_expr | simple_expr``所建立的表达式。由于这些规则中涉及到的非终结符及ast节点类的功能十分相似，对应代码生成的过程也十分相似，故一起说明。基本流程为：

* 判断是否有运算符：如果有则调用两个操作数的Generate；否则只有一个操作数，调用该操作数的Generate并直接返回
* 如果有两个操作数，根据操作符创建相应的运算指令。部分运算指令（如加减乘除）需要考虑整型与浮点的不同情况。

下面的代码以term类为例：

````c++
// Generate IR code for Term
Value* Term::Generate(CodeGenerator &codeGen){
    // Generate IR code for factors
    Value* right = this->factor->Generate(codeGen);
    // If there is only a factor, just return the value
    if(this->mulOp==nullptr) return right;
    Value* left = this->term->Generate(codeGen);
    
    //todo : type check
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(!left || !right){
        return nullptr;
    }
    // Create IR instructions corresponding to operation type
    switch(this->mulOp->getOpType()){
        case MT_MUL: return isFloat ? codeGen.builder.CreateFMul(left, right, "multmpf") : codeGen.builder.CreateMul(left, right, "multmpi");
        case MT_DIV: return isFloat ? codeGen.builder.CreateFDiv(left, right, "divmpf") : codeGen.builder.CreateSDiv(left, right, "divmpi");
        case MT_MOD: return nullptr; //todo: mod
    }
    //todo:error
}
````

在处理加减运算的时候，需要注意一种特殊情况：指针运算（本语言中只支持指针的加减运算，如``a[] + i``）。此处的特殊处理与“变量取值”一节中的指针处理类似：``a[]``会返回头指针，``i``为整型，需要经过指针转整型、判断元素类型获取size、32位int转64位int、整型加减、整型转指针的流程。具体代码与“变量取值”一节相同，这里不再展示。

##### 赋值语句

本语言支持多值赋值，中间代码生成只要将对应位置进行赋值即可。需要注意的地方有：

* 赋值表达式左边是变量类，变量取值返回的是指针，所以赋值的时候直接``CreateStore``即可。

* 不能逐个做取值赋值，否则交叉赋值（如``a, b = b, a``）会有bug。需要将所有的值全部取出后再一一赋值。
* 对于函数调用的情况：函数返回的多值是存储在结构体里的，所以函数调用类的Generate返回结构体。此处需要将结构体的元素一一取出（``CreateExtractValue``），而返回结构体的类型信息会在函数定义的类中存储到全局的函数栈中。
* 赋值中遇到``_``直接跳过。

````c++
// Generate assign statements
Value* ExprStmt::Generate(CodeGenerator &codeGen){
    // Get var list and expr list
    VarList var = *(this->vars->getVarlist());
    ExprList expr = *(this->exprs->getExprlist());
    //todo: error(different terms)
    std::vector<Value*> tmp;
    // Traverse
    for(auto &thisExpr : expr){
        auto right = thisExpr->Generate(codeGen);
        std::string funcname = thisExpr->getCallName();
        if(funcname!="" && funcname != "printf" && funcname != "scanf"){
            // Get number of struct elements and traverse
            int num = codeGen.getReturnType(funcname)->getStructNumElements();
            for(int i=0;i<num;i++){
                // Extract value and assign to a variable
                Value* structValue = codeGen.builder.CreateExtractValue(right, (uint64_t)i);
                tmp.push_back(structValue);
            }
        }else{
            // Assign directly
            tmp.push_back(right);
        }
    }
    auto thisValue = tmp.begin();
    for(auto &thisVar : var){
        std::string name = thisVar->getID();
        if(name=="") {
            thisValue++;
            continue;
        }
        codeGen.builder.CreateStore(*thisValue, thisVar->Generate(codeGen));
        thisValue++;
    }
    return nullptr;
}
````

##### 函数定义

函数定义包括以下流程：

* 参数类型：需要判断是否为数组。如果是数组则对应参数类型是指针；否则是对应的llvm类型。

```c++
    if(!this->inParams->getVoid()){
        ParamList inParamList = *(this->inParams->getParamList());
        // Arraytype must be stored as pointer
        for(auto &param: inParamList){
            if(param->getIsArray()){
                argType.push_back(codeGen.getLLVMPtrType(param->getType()));
            }else{
                argType.push_back(codeGen.getLLVMType(param->getType()));
            }
        }
    } 
```

* 返回值类型：判断是否需要返回结构体。返回void、系统函数、main函数这三种情况不需要返回结构体，其他统一用结构体封装。结构体成员类型可以用``setBody``函数封装，需要将所有成员的类型push到vector中。

```c++
	if(this->outParams->getVoid()){
        type = codeGen.getLLVMType(CG_VOID);    
    }else if(this->identifier->getID()=="main"){
        //todo:error
        type = retType.at(0);
    }
    else{
        // Set a struct type
        auto structType = StructType::create(codeGen.llvmContext, "retType");
        structType->setBody(retType);
        type = structType;
    }
```

* 定义函数原型和实例，并修改运行环境：创建一个新的block，设置代码插入点，将返回值添加到函数栈中。

```c++
    // Create function
    functionType = FunctionType::get(type, argType, false);
    function = Function::Create(functionType, llvm::GlobalValue::ExternalLinkage, this->identifier->getID(), codeGen.theModule.get());
    BasicBlock* basicBlock = BasicBlock::Create(codeGen.llvmContext, "entry", function, nullptr);
    
    // Add function and push block
    codeGen.builder.SetInsertPoint(basicBlock);
    codeGen.pushBlock(basicBlock);
    codeGen.addFunc(funcName, type);
```

* 为传入参数分配空间并赋值。传入参数的值可以通过``function->args()``方法得到。此处逻辑与变量定义、变量取值一致。不过需要注意的是，由于符号表中存储的是变量的地址，所以设置符号表时，对于一般变量需要存储``CreateAlloca``返回的值；但对于指针类型，不能存储指针的地址而是指针本身，所以需要对``CreateAlloca``的地址进行``CreateLoad``操作后再存入符号表中。

````c++
    // Create stores for input parameter
    if(!this->inParams->getVoid()){
        ParamList inParamList = *(this->inParams->getParamList());
        auto thisParam = inParamList.begin();
        auto thisValue = function->args().begin();
        // Traverse input parameters
        for(;thisParam!=inParamList.end();thisParam++, thisValue++){
            thisValue->setName((*thisParam)->getName());
            Value* alloc = (*thisParam)->Generate(codeGen);
            // Store the value in local variables
            codeGen.builder.CreateStore(thisValue, alloc, false);
            // If it's pointer, just store it's value
            // Otherwise store it's pointer
            if((*thisParam)->getIsArray()){
                codeGen.setSymValue((*thisParam)->getName(), codeGen.builder.CreateLoad(alloc));
            }
            else codeGen.setSymValue((*thisParam)->getName(), alloc);
        }
    }
````

* 生成函数体的中间代码，直接调用对应类的Generate即可。生成完毕后修改运行环境，删除当前block。

````c++
    this->comStmt->Generate(codeGen);
    codeGen.popBlock();
````

##### 返回语句

返回语句的逻辑如下：

* 首先需要获取所在函数的返回值信息，``getParent``用于获取当前所在的函数。如果返回的是void，直接创建ret void的语句并返回。

```c++
    Function* function = codeGen.builder.GetInsertBlock()->getParent();
    Type* retType = function->getReturnType();
    // If it's void, just create a void return instruction
    if(retType->isVoidTy()) return codeGen.builder.CreateRetVoid();
```

* 如果返回struct，则需要一一设置结构体成员的值，设置方法为：``CreateInBoundGEP``获取每个成员的指针，``CreateStore``赋值。

````c++
    if(retType->isStructTy()){
        // Get all member types
        for(auto thisExpr=expr.begin(); thisExpr!=expr.end(); thisExpr++){
            Value* thisValue = (*thisExpr)->Generate(codeGen);
            std::vector<Value*> indices;
            indices.push_back(llvm::ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), 0, false));
            indices.push_back(llvm::ConstantInt::get(codeGen.getLLVMType(CG_INTEGER), (uint64_t)distance(expr.begin(),thisExpr), false));
            // Get the pointer and store the value
            auto ptr = codeGen.builder.CreateInBoundsGEP(inst, indices);
            codeGen.builder.CreateStore(thisValue, ptr);
        }    
    }
````

* 如果返回单值，则直接``CreateStore``即可。

````c++
else{
        // Just store the value, for main, scanf, printf
        codeGen.builder.CreateStore(expr.at(0)->Generate(codeGen), inst);
    }
````

* 创建返回值语句并返回。

```c++
    Value* returnValue = codeGen.builder.CreateLoad(inst);
    codeGen.builder.CreateRet(returnValue);
```

##### 函数调用

函数调用需要注意的一点是：语言中没有显式支持指针，所以对于scanf函数的处理与一般函数不同：scanf的参数需要传递指针。下面是函数调用的处理逻辑：

* 首先根据函数名获取函数实例，需要对系统函数和用户定义函数区分：系统函数直接获取；用户定义函数可以通过``getFunction``获取。同时对是否为scanf做标记。

````c++
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
````

* 设置参数：将所有参数的值push到一个vector中。scanf函数需要存指针的值。这里需要注意的是scanf第一个参数是string，这个参数本身就是以全局字符串的方式存储，不需要取地址。对于其他参数，需要取出指针。arg类返回值，这里获取指针的策略是通过arg类获取对应的var类（即”变量取值“小节对应的类），该类返回指针。

```c++
    if(this->args->getArgList()!=nullptr){
        // Get argument list and traverse
        ArgList argsn = *(this->args->getArgList());
        for(auto &thisArg : argsn){
            Value* value;
            bool isString;
            Variable* var = thisArg->getVariable(isString);
            // If this is scanf function and not the first string, We need to transfer pointer types
            // Otherwise we need to transfer the value
            if(isScanf && !isString){
                //todo:error
                value = var->Generate(codeGen);
            }else{ 
                value = thisArg->Generate(codeGen);
            }
            argsv.push_back(value);
            // If any argument codeGen fail, return null
            if( !argsv.back() ){
                return nullptr;
            }
        }
    }
```

* 创建call语句并获得返回值。

```c++
    Value *res = codeGen.builder.CreateCall(function, argsv);
```

##### if语句

if语句被抽象成四个基础块：原有块（计算cond值）、if块（cond为真）、else块（cond为假）、end块（if语句结束后的后续语句块）。此处分为有else和没有else两种情况，两种情况对应的DAG图为：

![if](.\img\if.png)

![noelse](.\img\noelse.png)

需要注意的点为：

* BasicBlock构造过程中可以指定插入的函数，如果没有指定，则需要push到BasicBlockList中。
* 每一个新块插入代码前需要设置insertpoint，同时设置运行环境。
* 检测每一个块是否已有终结语句。如在if块中已经有return语句，则不需要创建从if到end的无条件跳转。else同理。

```c++
// Generate IR code for if statement
Value* SelectStmt::Generate(CodeGenerator &codeGen){
    // Get condition value
    Value* condValue = this->logicExpr->Generate(codeGen);
    if( !condValue )
        return nullptr;
    // Get function where if statement is in
    Function* theFunction = codeGen.builder.GetInsertBlock()->getParent();

    // Create 3 bodies: if, else, and end(after if or else it will jump tp end)
    BasicBlock *ifbody = BasicBlock::Create(codeGen.llvmContext, "ifbody", theFunction);
    BasicBlock *elsebody = BasicBlock::Create(codeGen.llvmContext, "elsebody");
    BasicBlock *end = BasicBlock::Create(codeGen.llvmContext, "end");

    // whether there is else
    if( this->isIfelse ){
        codeGen.builder.CreateCondBr(condValue, ifbody, elsebody);
    } else{
        codeGen.builder.CreateCondBr(condValue, ifbody, end);
    }

    codeGen.builder.SetInsertPoint(ifbody);

    // Generate IR code for if body
    codeGen.pushBlock(ifbody);
    this->ifStmt->Generate(codeGen);
    codeGen.popBlock();

    // If there is return in if body, it will not jump to end block 
    ifbody = codeGen.builder.GetInsertBlock();
    if( ifbody->getTerminator() == nullptr ){   
        // Create jump to end block 
        codeGen.builder.CreateBr(end);
    }

    // If there has a else, create else block
    if( this->isIfelse ){
        theFunction->getBasicBlockList().push_back(elsebody);    
        codeGen.builder.SetInsertPoint(elsebody);            

        // Generate IR code for else body
        codeGen.pushBlock(elsebody);
        this->elStmt->Generate(codeGen);
        codeGen.popBlock();

        // If there is return in else body, it will not jump to end block
        elsebody = codeGen.builder.GetInsertBlock();
        if(elsebody->getTerminator() == nullptr){
            // Create jump to end block
            codeGen.builder.CreateBr(end);
        }
    }

    // Create end block
    theFunction->getBasicBlockList().push_back(end);        
    codeGen.builder.SetInsertPoint(end);        

    return nullptr;
}
```

##### while语句

while语句被抽象成三个基础块：cond块（计算cond值，需要反复计算）、body块（循环体）、end块（while语句结束后的后续语句块）。对应的DAG图为：

![while](.\img\while.png)

注意点和生成流程与if类似。

```c++
// Generate IR code for while statement
Value* IterStmt::Generate(CodeGenerator &codeGen){
    // Get the function where while statement is in
    Function* theFunction = codeGen.builder.GetInsertBlock()->getParent();

    // Create blocks for cond value, iter body and end
    BasicBlock *cond = BasicBlock::Create(codeGen.llvmContext, "cond", theFunction);
    BasicBlock *body = BasicBlock::Create(codeGen.llvmContext, "body");
    BasicBlock *end = BasicBlock::Create(codeGen.llvmContext, "end");

    // Former code jump to cond
    codeGen.builder.CreateBr(cond);
    codeGen.builder.SetInsertPoint(cond);
    
    // Generate cond value
    Value* condValue = this->logicExpr->Generate(codeGen);
    if( !condValue )
        return nullptr;

    // Create cond jump
    codeGen.builder.CreateCondBr(condValue, body, end);

    theFunction->getBasicBlockList().push_back(body);
    codeGen.builder.SetInsertPoint(body);

    // Create IR code for iter body
    codeGen.pushBlock(body);
    this->statement->Generate(codeGen);
    codeGen.popBlock();

    // If there is return in iter body, it will not jump to end
    body = codeGen.builder.GetInsertBlock();
    if(body->getTerminator() == nullptr){
        codeGen.builder.CreateBr(cond);
    }

    // Create end
    theFunction->getBasicBlockList().push_back(end);
    codeGen.builder.SetInsertPoint(end);

    return nullptr;
}
```



### 目标代码生成

#### 选择目标机器

LLVM 支持本地交叉编译。我们可以将代码编译为当前计算机的体系结构，也可以像针对其他体系结构一样轻松地进行编译。LLVM 提供了 `sys::getDefaultTargetTriple`，它返回当前计算机的目标三元组：

```c++
auto targetTriple = llvm::sys::getDefaultTargetTriple();
codeGen.theModule->setTargetTriple(targetTriple);
```

在获取Target前，初始化所有目标以发出目标代码：

```c++
InitializeAllTargetInfos();
InitializeAllTargets();
InitializeAllTargetMCs();
InitializeAllAsmParsers();
InitializeAllAsmPrinters();
```

使用目标三元组获得 Target：

```c++
    std::string error;
    auto Target = llvm::TargetRegistry::lookupTarget(targetTriple, error);

    if (!Target)
    {
        llvm::errs() << error;
        return;
    }
```

 `TargetMachine` 类提供了我们要定位的机器的完整机器描述：

```c++
    auto CPU = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto theTargetMachine = Target->createTargetMachine(targetTriple, CPU, features, opt, RM);
```

#### 配置 Module

配置模块，以指定目标和数据布局，可以方便了解目标和数据布局。

```c++
codeGen.theModule->setDataLayout(theTargetMachine->createDataLayout());
```

#### 生成目标代码

* 先定义要将文件写入的位置

```c++
    std::error_code EC;
    llvm::raw_fd_ostream dest(filename.c_str(), EC, llvm::sys::fs::OF_None);

    if (EC)
    {
        llvm::errs() << "Could not open file: " << EC.message();
        return;
    }
```

* 定义一个发出目标代码的过程，然后运行该 pass

```c++
    llvm::legacy::PassManager pass;
    auto fileType = llvm::CGFT_ObjectFile;

    if (theTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType))
    {
        llvm::errs() << "theTargetMachine can't emit a file of this type";
        return;
    }

    pass.run(*codeGen.theModule.get());
    dest.flush();
```

