HOME=/home/stly/Documents/ProtocolEx/cmake-build-debug
WORK=/home/stly/Documents/ProtocolEx/stubs/midilib


opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct  ${WORK}/m2rtttl.bc> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mfc120.bc> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mididump.bc> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/miditest.bc> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mozart.bc> /dev/null

#---------------loop identifier---------------------------
opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/m2rtttl.bcc> /dev/null

#opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/mfc120.opt.id> /dev/null

#opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/test-case.opt.id> /dev/null

#opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/mididump.opt.id> /dev/null

#opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/miditest.opt.id> /dev/null

#opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/mozart.opt.id> /dev/null

#---------------------------------------------------------------
rm -rf SD.inst
(opt -load ${HOME}/lib/SideEffect/libSideEffectPass.so -sideeffect-analysis -strFuncName ConvertMIDI -numLoopHeader 598 ${WORK}/m2rtttl.bcc> /dev/null ) &>> SD.inst
rm -rf action.table
( opt -load ${HOME}/lib/Constraints/libConstraintsPass.so -collect-constraints -strFuncName ConvertMIDI -numLoopHeader 598 -strInstFile SD.inst -strSTName struct.MIDI_MSG ${WORK}/m2rtttl.bcc > /dev/null ) &>> action.table


