#include "llvm/IR/Function.h"
#include "llvm/Pass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/InstIterator.h"
//#include "llvm/IR/Instructions.h"
#include <string>
#include <vector>
#include "llvm/ADT/StringRef.h"
#include "llvm/IR/Value.h"
#include "llvm/Support/CommandLine.h"

using namespace llvm;
using namespace std;

cl::list<std::string> Lists("lists",cl::desc("Specify names"),cl::OneOrMore);


class LongestPathofCFG {
public:
	
	void calculateLP(BasicBlock* firstBB,StringRef targetname) {
	//	BasicBlock *firstBB=F.getEntryBlock();
		bblist.clear();
		lpbblist.clear();
		intrlist.clear();
		lpintrlist.clear();
		bblist.push_back(firstBB);
		int pathlen=0;
		havepath=false;
		maxpath=0;
//		StringRef targetname("%30");

		calculatebyBB(firstBB,pathlen,targetname);
		printThePath(firstBB,targetname);
	}
private:
	void calculatebyBB(BasicBlock* BB,int plen,StringRef targetname)
	{
		Instruction *tinst= BB->getTerminator();
		int nsucc=tinst->getNumSuccessors();
//		string condition=tinst->getOpcodeName();
//		for (Instruction &I: &)
//			errs()<<I<<"\n";
//		StringRef testinst(*tinst);

	//	errs()<<"the number is "<<tinst->getNumOperands();

		//		const Instruction* tmpinst=tinst->getOperand(0);
//		errs()<<", the opcode is "<<tmpinst->getOpcodeName();

//		errs()<<"\nthe name is "<<*(tinst->getOperand(0));
//		errs()<<"\nthe condition is "<<*tinst<<"\n";
		for (unsigned i=0;i<nsucc;++i)
		{
	//		if(i>0)
	//			bblist.pop_back();
			BasicBlock *succ=tinst->getSuccessor(i);
			bblist.push_back(succ);
			intrlist.push_back(tinst);
			string tmpname;
			raw_string_ostream TMPN(tmpname);
			succ->printAsOperand(TMPN,false);
			StringRef tname(TMPN.str());
//			errs()<<"now";
//			printThePath2();
			if(targetname.compare(tname)==0)	
			{
		//		plen=plen+1;
				havepath=true;	
				if((plen+1)>maxpath)
				{
					maxpath=plen+1;
					lpbblist=bblist;
					lpintrlist=intrlist;
				}
//				errs()<<"find the target\n";
			}
			else
			{
				calculatebyBB(succ,plen+1,targetname);
			}
			intrlist.pop_back();
			bblist.pop_back();
		}

	}
	void printThePath(BasicBlock* entry, StringRef target)
	{
		if( havepath == false)
		{
			string tmpn;
			raw_string_ostream FIRST(tmpn);
			entry->printAsOperand(FIRST,false);
			StringRef first(FIRST.str());
			errs()<<"there is no path from "<<first<<" to "<<target<<"\n";
			return;
		}
		errs()<<"the longest path is: ";
		for(int i=0;i<lpintrlist.size();i++)
		{
			string tmpn;
			raw_string_ostream TPN(tmpn);
			lpbblist[i]->printAsOperand(TPN,false);
			StringRef tn(TPN.str());
			errs()<<tn<<", ";
			
			if(strcmp("br",lpintrlist[i]->getOpcodeName())==0)
			{
					if(lpintrlist[i]->getNumOperands()!=1)
						errs()<<"["<<*(lpintrlist[i]->getOperand(0))<<"], ";
					else
						errs()<<"["<<lpintrlist[i]->getOpcodeName()<<"], ";
			}
			else if (strcmp("switch",lpintrlist[i]->getOpcodeName())==0)
			{
				errs()<<"["<<*(lpintrlist[i]->getOperand(0))<<"], ";
			}
			else
			{
				errs()<<"["<<lpintrlist[i]->getOpcodeName()<<"], ";
			}
		}
		string lasttmp;
		raw_string_ostream TPNL(lasttmp);
		lpbblist[lpbblist.size()-1]->printAsOperand(TPNL,false);
		StringRef ltn(TPNL.str());
		errs()<<ltn<<"\n";
	}
/*	void printThePath2()
	{
		for(int i=0;i<bblist.size();i++)
		{
			string tmpn;
			raw_string_ostream TPN(tmpn);
			lpbblist[i]->printAsOperand(TPN,false);
			StringRef tn(TPN.str());
			errs()<<tn<<" ";
		}
		errs()<<"\n";
	}*/
	int maxpath=0;
	bool havepath;
	vector<BasicBlock *> bblist;
	vector<BasicBlock *> lpbblist;
	vector<Instruction*> intrlist;
	vector<Instruction*> lpintrlist;
	

};
struct CFGFuncs: public FunctionPass {
	CFGFuncs():FunctionPass(ID) {}


	StringRef getTargetName()
	{
		if (index_of_namelists < Lists.size())
		{
			StringRef targetname(Lists[index_of_namelists]);
			index_of_namelists=index_of_namelists+1;
			return targetname;
		}
		StringRef targetname(Lists[Lists.size()-1]);
		return targetname;
	}	
	virtual bool runOnFunction(Function &F) {
	/*	//errs()<<Lists.size()<<"\n";
		if (index_of_namelists < Lists.size())
		{
			StringRef targetname(Lists[index_of_namelists]);
			index_of_namelists=index_of_namelists+1;
		}
		else
			StringRef targetname(Lists[List.size()-1]);
		
		for( auto &e: Lists){
			StringRef targetname(e);
			errs()<<targetname<<'\n';
		}
	*/
		errs()<<"the current function is ";
		errs().write_escaped(F.getName())<<'\n';
	/*	
		for(inst_iterator I = inst_begin(F), E=inst_end(F);I!=E;++I)
		{
			errs()<<"the opcode is "<<I->getOpcodeName()<<", the instruction is : ";
			errs()<<*I<<"\n";
		}
	*/	
		BasicBlock *BB=&F.getEntryBlock();
		LongestPathofCFG lpcfg;
		lpcfg.calculateLP(BB,getTargetName());
/*		
		BasicBlock *BB=&F.getEntryBlock();
		string answer;
	//	StringRef answer(BB->getName());
		raw_string_ostream ANS(answer);
		BB->printAsOperand(ANS,false);
		StringRef ans(ANS.str());

		StringRef targetname("%4");
		const TerminatorInst *tinst= BB->getTerminator();
		for (unsigned I =0, nsucc=tinst->getNumSuccessors();I<nsucc;++I)
		{
			BasicBlock *succ = tinst->getSuccessor(I);
			string tmpname;
			raw_string_ostream TMPN(tmpname);
			succ->printAsOperand(TMPN,false);
			StringRef tname(TMPN.str());
			if(targetname.compare(tname)==0)
				errs()<<"find the target\n";
		}
		



	//	string answer2="%0";
		StringRef answer2("%4");
	//
	//if(strcmp(ANS.str(),answer2)==0)
		if(ans.compare(answer2)==0)
			errs()<<"success";
		else
			errs()<<"***"<<ans<<"^^^^^"<<answer2<<"****";
		errs()<<"\n";		

*/

		return false;
	}

	virtual void getAnalysisUsage(AnalysisUsage &AU) const {
		AU.setPreservesAll();
	}
	static int index_of_namelists;
	static char ID;
};
char CFGFuncs::ID=0;
int CFGFuncs::index_of_namelists=0;

int main(int argc, char **argv) 
{
	errs()<<"into the main function\n";
}

static RegisterPass<CFGFuncs>
	X("cfg-funcs","CFG Function Pass");
