#include <set>
#include <map>
#include <vector>
#include "ReachAnalysis/ReachAnalysis.h"
#include "llvm/IR/CFG.h"

using namespace std;
using namespace llvm;


BasicBlock * nextstep(BasicBlock * x, BasicBlock * y)
{

	map<BasicBlock *, vector<BasicBlock *> > mapBlockSucc;
	set<BasicBlock *> setVisited;

	for(succ_iterator si = succ_begin(x) ; si != succ_end(x); si ++)
	{
		if(*si == y)
		{
			return *si;
		}

		setVisited.insert(*si);		
	}

	for(succ_iterator si = succ_begin(x) ; si != succ_end(x); si ++)
	{
		vector<BasicBlock *> vecSucc;

		for(succ_iterator ssi = succ_begin(*si); ssi != succ_end(*si); ssi ++ )
		{
			if(setVisited.find(*ssi) == setVisited.end())
			{
				vecSucc.push_back(*ssi);
			}
		}

		mapBlockSucc[*si] = vecSucc;
	}


	while(true)
	{
		unsigned numTotal = 0;

		map<BasicBlock *, vector<BasicBlock *> >::iterator itMapBegin = mapBlockSucc.begin();
		map<BasicBlock *, vector<BasicBlock *> >::iterator itMapEnd   = mapBlockSucc.end();

		while(itMapBegin != itMapEnd)
		{
			numTotal += itMapBegin->second.size();
			itMapBegin ++;
		}

		if(numTotal == 0)
		{
			break;
		}

		
		itMapBegin = mapBlockSucc.begin();

		while(itMapBegin != itMapEnd)
		{
			for(unsigned i = 0; i < itMapBegin->second.size(); i ++ )
			{
				if(itMapBegin->second[i] == y)
				{
					return itMapBegin->first;
				}

				setVisited.insert(itMapBegin->second[i]);
			}

			itMapBegin++;
		}

		map<BasicBlock *, vector<BasicBlock *> > mapNewBlockSucc;

		itMapBegin = mapBlockSucc.begin();

		while(itMapBegin != itMapEnd)
		{
			vector<BasicBlock *> vecTmp;

			for(unsigned i = 0; i < itMapBegin->second.size(); i ++ )
			{
				for(succ_iterator ssi = succ_begin(itMapBegin->second[i]); ssi != succ_end(itMapBegin->second[i]); ssi ++ )
				{
					if(setVisited.find(*ssi) == setVisited.end())
					{
						vecTmp.push_back(*ssi);
					}
				}
			}

			mapNewBlockSucc[itMapBegin->first] = vecTmp;

			itMapBegin++;
		}


		mapBlockSucc = mapNewBlockSucc;
	}

	return NULL;
}