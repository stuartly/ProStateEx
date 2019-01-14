HOME=/home/stly/Documents/ProtocolEx/cmake-build-debug
WORK=/home/stly/Documents/ProtocolEx/stubs/midilib


opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct  ${WORK}/m2rtttl.opt.id> /dev/null


#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mfc120.opt.id> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mididump.opt.id> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/miditest.opt.id> /dev/null

#opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct < ${WORK}/mozart.opt.id> /dev/null


opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/m2rtttl.opt.id> /dev/null

