HOME=/home/songlh/workspace/protocol/ProtocolEx
WORKING=${HOME}/stubs/midilib

preprocess()
{
	opt -load ${HOME}/build/lib/NoOPT/libNoOPTPass.so -no-opt ${WORKING}/m2rtttl.bc > ${WORKING}/m2rtttl.noopt
	opt -mem2reg ${WORKING}/m2rtttl.noopt > ${WORKING}/m2rtttl.opt
	opt -load ${HOME}/build/lib/IDAssigner/libIDAssignerPass.so -tag-id ${WORKING}/m2rtttl.opt > ${WORKING}/m2rtttl.opt.id
	opt -load ${HOME}/build/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -strFuncName ConvertMIDI ${WORKING}/m2rtttl.opt.id > /dev/null
	

}

#opt -load ${HOME}/build/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -strFuncName ConvertMIDI ${WORKING}/m2rtttl.opt.id > /dev/null

preprocess
rm -rf SD.inst
(opt -load ${HOME}/build/lib/SideEffect/libSideEffectPass.so -sideeffect-analysis -strFuncName ConvertMIDI -numLoopHeader 598 ${WORKING}/m2rtttl.opt.id > /dev/null ) &>> SD.inst
rm -rf action.table
( opt -load ${HOME}/build/lib/Constraints/libConstraintsPass.so -collect-constraints -strFuncName ConvertMIDI -numLoopHeader 598 -strInstFile SD.inst -strSTName struct.MIDI_MSG ${WORKING}/m2rtttl.opt.id > /dev/null ) &>> action.table

