#include <fstream>
#include <boost/algorithm/string.hpp>

#include "Common/BasicBlockCommon.h"
#include "Common/Constant.h"
#include "MakeSymbolicForKlee/SymbolicHelper.h"

#include "llvm/ADT/Statistic.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/MDBuilder.h"


using namespace std;
using namespace llvm;

static RegisterPass<SymbolicHelper> X(
        "make-symbolic",
        "make symbolic for a given path, which is using to klee.",
        false,
        true);


#define DEBUG_TYPE "ignoretagger"
STATISTIC(NumIgnore, "Number of instructions igonre");


static cl::opt<std::string> strPathFile("strPathFile",
                                        cl::desc("File Containing the Input Path"), cl::Optional,
                                        cl::value_desc("strPathFile"));


static cl::opt<bool> ignoreCallInst("ignoreCallInst",
                                    cl::desc("Ignore call function could not be found in current bc file"),
                                    cl::init(true));


void insertIgnore(Module *M, Instruction *II) {
    ++NumIgnore;
    IntegerType *IntType = IntegerType::get(M->getContext(), 32);
    MDBuilder MDHelper(M->getContext());
    Constant *InsID = ConstantInt::get(IntType, NumIgnore);
    SmallVector<Metadata *, 1> Vals;
    Vals.push_back(MDHelper.createConstant(InsID));
    II->setMetadata("klee_ignore", MDNode::get(M->getContext(), Vals));
}


void markSymbolicFlag(Module *M, Instruction *Inst) {
    IntegerType * IntType = IntegerType::get(M->getContext(), 32);
    MDBuilder MDHelper(M->getContext());
    Constant * InsID = ConstantInt::get(IntType, 1);
    SmallVector<Metadata *, 1> Vals;
    Vals.push_back(MDHelper.createConstant(InsID));
    Inst->setMetadata(SYMBOLIC_FLAG, MDNode::get(M->getContext(), Vals));
}


char SymbolicHelper::ID = 0;

void SymbolicHelper::getAnalysisUsage(AnalysisUsage &AU) const {
    AU.setPreservesAll();
}

SymbolicHelper::SymbolicHelper() : ModulePass(ID) {
    PassRegistry &Registry = *PassRegistry::getPassRegistry();
}

void SymbolicHelper::print(raw_ostream &O, const Module *M) const {
    return;
}

/*!
 * This function is for parsing path file.
 *
 * @param strPathFile - The full path of file,
 *                    which contains basic block ids.
 * @result pathIds - the list of basic block's ids.
 */
vector<int> GetBlockIdsFromPathFile(const string &strPathFile) {

    std::vector<std::string> blockIds_str;
    std::vector<int> blockIds;
    std::ifstream file(strPathFile);
    char delim = '-';

    if (file) {
        std::string item;

        while (getline(file, item, delim)) {
            blockIds_str.push_back(item);
        }

        for (auto id = blockIds_str.begin(); id != blockIds_str.end(); id++) {
            blockIds.push_back(stoi(*id));
        }

        return blockIds;

    } else {
        // TODO: exception,tips: file is not found, please check file exist?
        errs() << "//--PATH-Constraints:" << "Path file is not found! \n";
        return blockIds;
    }
}

void SymbolicHelper::MakeSymbolics(Module &M,
                                   const std::vector<int> &blockIds) {

    vector<BasicBlock> blocks;

    if (!blockIds.empty()) {
        for (Module::iterator FI = M.begin(); FI != M.end(); FI++) {

            Function *f = &*FI;

            for (Function::iterator BI = f->begin(); BI != f->end(); BI++) {

                BasicBlock *BB = &*BI;
                unsigned _id = GetBasicBlockID(BB);

                if (std::find(blockIds.begin(), blockIds.end(), _id) != blockIds.end()) {
                    // Found the item
                    // get it parent function
                    Function *TargetFunction = M.getFunction(BI->getParent()->getName());
                    if (!TargetFunction) {
                        errs() << "Could not find target function, please check the basic block Id." << "\n";
                        exit(1);
                    } else {
                        errs() << "Find target function,"
                                " it will insert make symbolic instructions and mark ignore label." << "\n";
                        insertMakeSymbolicInst(&M, TargetFunction);
                        errs() << "Success insert make symbolic instructions!" << "\n";
                        break;
                    }
                }
            }
        }
    } else {
        errs() << "//---PATH-Constraints:" << "The list of basic block id is empty!\n";
    }
}

/**
 * Make a symbolic variable according to S.
 * @param M   is target module.
 * @param S   is a alloc instruction.
 * @param Inst  insert before the Instruction.
 * @param Label is the name to mark a symbolic.
 */
void SymbolicHelper::callMakeSymbolic(Module *M, Value *S, Instruction *Inst,
                                      const std::string Label, unsigned size) {
    // klee make symbolic function
    ArrayType *ArrayTy_Str = ArrayType::get(
            IntegerType::get(M->getContext(), 8),
            Label.size() + 1);

    GlobalVariable *gvar_array_x = new GlobalVariable(
            /*Module=*/*M,
            /*Type=*/ArrayTy_Str,
            /*isConstant=*/true,
            /*Linkage=*/GlobalValue::PrivateLinkage,
            /*Initializer=*/0, // has initializer, specified below
            /*Name=*/Label);
    gvar_array_x->setAlignment(1);

    // Constant Definitions
    Constant *const_array_x = ConstantDataArray::getString(
            M->getContext(), Label, true);

    // Global Variable Definitions
    gvar_array_x->setInitializer(const_array_x);

    ConstantInt *const_int32_x = ConstantInt::get(
            M->getContext(), APInt(32, StringRef("0"), 10));

    std::vector<Constant *> const_ptr_x_indices;
    const_ptr_x_indices.push_back(const_int32_x);
    const_ptr_x_indices.push_back(const_int32_x);

    Constant *const_ptr_x = ConstantExpr::getGetElementPtr(
            ArrayTy_Str, gvar_array_x, const_ptr_x_indices);

    // TODO fixed alloc size
    ConstantInt *const_int64_klee;
    if (size) {
        const_int64_klee = ConstantInt::get(
                M->getContext(),
                APInt(64, StringRef(std::to_string(size)), 10));
    } else {

        DataLayout *dl = new DataLayout(M);
        Type *type_1 = S->getType()->getContainedType(0);

        while (isa<PointerType>(type_1))
            type_1 = type_1->getContainedType(0);

        const_int64_klee = ConstantInt::get(
                M->getContext(),
                APInt(64, StringRef(std::to_string(dl->getTypeAllocSize(type_1))), 10));
    }

    // klee make symbolic function type
    PointerType *KleePointerTy = PointerType::get(
            IntegerType::get(M->getContext(), 8), 0);
    std::vector<Type *> FuncTy_args;
    FuncTy_args.push_back(KleePointerTy);
    FuncTy_args.push_back(IntegerType::get(M->getContext(), 64));
    FuncTy_args.push_back(KleePointerTy);
    FunctionType *KleeMakeSymbolicFuncTy = FunctionType::get(
            /*Result=*/Type::getVoidTy(M->getContext()),
            /*Params=*/FuncTy_args,
            /*isVarArg=*/false);

    // klee make symbolic function
    Function *klee_make_symbolic_func = M->getFunction("klee_make_symbolic");
    if (!klee_make_symbolic_func) {
        klee_make_symbolic_func = Function::Create(
                /*Type=*/KleeMakeSymbolicFuncTy,
                /*Linkage=*/GlobalValue::ExternalLinkage,
                /*Name=*/"klee_make_symbolic", M); // (external, no body)
        klee_make_symbolic_func->setCallingConv(CallingConv::C);
    }

    // call klee make symbolic function
    CastInst *klee_ptr_bitcast = new BitCastInst(S, KleePointerTy, "", Inst);
    std::vector<Value *> klee_make_symbolic_params;
    klee_make_symbolic_params.push_back(klee_ptr_bitcast);
    klee_make_symbolic_params.push_back(const_int64_klee);
    klee_make_symbolic_params.push_back(const_ptr_x);
    CallInst *void_klee_inst = CallInst::Create(
            klee_make_symbolic_func, klee_make_symbolic_params, "", Inst);

    void_klee_inst->setCallingConv(CallingConv::C);
    void_klee_inst->setTailCall(false);
    AttributeList void_klee_PAL;
    void_klee_inst->setAttributes(void_klee_PAL);
}


void SymbolicHelper::MakeSymbolicAllocInst(Module *M, Function *TargetFunction) {

    // TODO: FIXME before the first gaven BB
    Instruction *insertBefore = NULL;
    DataLayout *dl = new DataLayout(M);
    int DEFAULT_ARRAY_SIZE = 100;

    // use to add klee_ignore metadata
    size_t arg_size = TargetFunction->arg_size();

    for (Function::iterator BI = TargetFunction->begin(); BI != TargetFunction->end(); BI++) {

        BasicBlock *BB = &*BI;
        int current_inst_index = 0;
        bool find = false;

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II) {
            Instruction *inst = &*II;

            if (inst->getOpcode() == Instruction::Store) {
                if (!find) {
                    find = true;
                    insertBefore = inst;
                }
                if (current_inst_index < arg_size) {
                    insertIgnore(M, inst);
                    current_inst_index++;
                    continue;
                } else {
                    break;
                }
            }
        }

        if (insertBefore == NULL) {
            for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II) {

                Instruction *inst = &*II;
                if (inst->getOpcode() == Instruction::Alloca) {
                    continue;
                }

                if (inst->getOpcode() == Instruction::Call) {
                    CallSite ci(inst);
                    Value *fp = ci.getCalledValue();
                    if (isa<InlineAsm>(fp)) {
                        continue;
                    }
                } else {
                    insertBefore = inst;
                    break;
                }
            }
        }

        if(insertBefore != NULL) {
            // mark a flag for klee, symbolic_flag
            markSymbolicFlag(M, insertBefore);
        }

        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); ++II) {

            Instruction *inst = &*II;
            if (inst->getOpcode() == Instruction::Alloca) {
                string symbolicName = inst->getName();
                if (II->getName() != "retval") {

                    Type *type_1 = inst->getType()->getContainedType(0);
                    if (isa<PointerType>(type_1)) {

                        while (isa<PointerType>(type_1))
                            type_1 = type_1->getContainedType(0);

                        if (isa<FunctionType>(type_1)) {
                            insertIgnore(M, inst);
                            continue;

                        } else if (isa<IntegerType>(type_1)) { // char[] or int []
                            ArrayType *ArrayTy = ArrayType::get(type_1, DEFAULT_ARRAY_SIZE);
                            AllocaInst *ptr_s = new AllocaInst(ArrayTy, 0, "", insertBefore);
                            ptr_s->setAlignment(type_1->getIntegerBitWidth() / 8);
                            this->callMakeSymbolic(M, ptr_s, insertBefore, symbolicName);
                            BitCastInst *bitCaInst = new BitCastInst(
                                    ptr_s, inst->getType()->getContainedType(0), "", insertBefore);
                            StoreInst *store_inst = new StoreInst(bitCaInst, inst, false, insertBefore);
                            store_inst->setAlignment(dl->getABITypeAlignment(inst->getType()));

                        } else if (isa<StructType>(type_1)) {
                            string struct_name = type_1->getStructName();
                            StructType *StructTy = M->getTypeByName(struct_name);
                            AllocaInst *ptr_s = new AllocaInst(StructTy, 0, struct_name, insertBefore);
                            const StructLayout *st_dl = dl->getStructLayout(StructTy);
                            ptr_s->setAlignment(st_dl->getAlignment());
                            this->callMakeSymbolic(M, ptr_s, insertBefore, struct_name);
                            StoreInst *store_inst = new StoreInst(ptr_s, inst, false, insertBefore);
                            store_inst->setAlignment(dl->getABITypeAlignment(inst->getType()));
                        }

                    } else {  // integer type / array type
                        this->callMakeSymbolic(M, inst, insertBefore, symbolicName);
                    }
                }
            } else
                continue;
        }
        break;
    }

}

void SymbolicHelper::MakeSymbolicCallInst(Module *M, Function *TargetFunction) {

    for (Function::iterator BI = TargetFunction->begin(); BI != TargetFunction->end(); BI++) {

        BasicBlock *BB = &*BI;
        for (BasicBlock::iterator II = BB->begin(); II != BB->end(); II++) {

            Instruction *inst = &*II;

            if (inst->getOpcode() == Instruction::Call) {
                CallSite ci(inst);
                Function *Callee = dyn_cast<Function>(ci.getCalledValue()->stripPointerCasts());

                if (!Callee) {

                    Value *fp = ci.getCalledValue();
                    if (isa<InlineAsm>(fp)) {
                        continue;
                    }

                    // If function could not found in current bc file, we will ignore it.
                    Type *retType = fp->getType()->getContainedType(0)->getContainedType(0);
                    insertIgnore(M, inst);

                    if (retType->isVoidTy()) {
                        // if it is void type, we will not do nothing!
                        continue;
                    }

                    // TODO: if code is "(simple(x) == 10)", it will not work!
                    // bc: %call = call i32 @simple(i32 %2)
                    //     %cmp3 = icmp eq i32 %call, 10
//                    inst = &*(++II);
//                    while ((II != BB->end()) && (inst->getOpcode() != Instruction::Store)) {
//                        insertIgnore(M, inst);
//                        inst = &*(++II);
//                    }

                    insertIgnore(M, inst);
                }

            } else {
                // If there is an function type pointer, we will ignore it.
                if (inst->getNumOperands() > 0) {
                    Type *operandTy = inst->getOperand(0)->getType();

                    while (isa<PointerType>(operandTy)) {
                        operandTy = operandTy->getContainedType(0);
                    }

                    if (isa<FunctionType>(operandTy)) {
                        insertIgnore(M, inst);
                    }
                }
            }
        }
    }
}

void SymbolicHelper::insertMakeSymbolicInst(Module *M, Function *TargetFunction) {

    MakeSymbolicAllocInst(M, TargetFunction);

    if (ignoreCallInst) {
        // FIXME: maybe we need fix it!
        MakeSymbolicCallInst(M, TargetFunction);
//        MakeSymbolicCallInst(M);
    }

}

bool SymbolicHelper::runOnModule(Module &M) {

    vector<int> blockIds = GetBlockIdsFromPathFile(strPathFile);
    MakeSymbolics(M, blockIds);
    return false;
}
