HOME=/home/stly/Documents/ProtocolEx
WORKING=${HOME}/stubs/midilib


#preprocess()
{	
#	opt -load ${HOME}/cmake-build-debug/lib/NoOPT/libNoOPTPass.so -no-opt ${WORKING}/m2rtttl.bc > ${WORKING}/m2rtttl.noopt
#	opt -mem2reg ${WORKING}/test-case.noopt > ${WORKING}/test-case.opt
	opt -load ${HOME}/cmake-build-debug/lib/IDAssigner/libIDAssignerPass.so -tag-id ${WORKING}/m2rtttl.bc > ${WORKING}/m2rtttl.bcc

}


