#ifndef PROTOCOLEX_CONSTANT_H
#define PROTOCOLEX_CONSTANT_H

enum FunctionLoopIOType {
    NoIOLoop = 0,
    OnlyIO,
    OnlyLoop,
    IOOutOfLoop,
    IOInLoop,
    // there are io call in loop and out of loop
    IOLoop,
};


#ifndef BB_ID
#define BB_ID "bb_id"
#endif

#ifndef LOOP_FLAG
#define LOOP_FLAG "loop_flag"
#endif

#ifndef IO_FLAG
#define IO_FLAG "io_flag"
#endif

#ifndef LOOP_IO_FLAG
#define LOOP_IO_FLAG "loop_io_flag"
#endif

#ifndef IO_CALL_FLAG
#define IO_CALL_FLAG "io_call_flag"
#endif

#ifndef SYMBOLIC_FLAG
#define SYMBOLIC_FLAG "symbolic_flag"
#endif


#endif //PROTOCOLEX_CONSTANT_H
