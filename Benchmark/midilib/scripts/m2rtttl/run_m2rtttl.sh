HOME=/home/stly/Documents/ProtocolEx/cmake-build-debug
WORK=/home/stly/Documents/ProtocolEx/stubs/midilib


opt -load ${HOME}/lib/STIdentifier/libSTIdentifierPass.so -identify-struct  ${WORK}/m2rtttl.bc> /dev/null

#---------------loop identifier---------------------------
opt -load ${HOME}/lib/LoopIdentifier/libLoopdentifierPass.so -identify-loop -StructOfInterestPath ${WORK}/struct-of-interest ${WORK}/m2rtttl.bc> /dev/null

