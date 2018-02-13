set(LIB_SOURCES
        src/library.cpp
        )

set(LIB_INCLUDE
        include/
        )

add_library(InstrumentCall STATIC
        ${LIB_SOURCES}
        )

target_include_directories(InstrumentCall
        PUBLIC ${LIB_INCLUDE}
        )

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(InstrumentCall PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(InstrumentCall PROPERTIES
        COMPILE_FLAGS "-fno-rtti -fPIC"
        )



## TODO: this is for testing runtime lib.
############################################################
# Create an executable
############################################################


# Source fles for the binary
set(BINARY_SOURCES
       src/test_1.cpp)

# Add an executable with the above sources
add_executable(testlib ${BINARY_SOURCES})

# link the new hello_library target with the hello_binary target
target_link_libraries(testlib
        PRIVATE InstrumentCall
        )
