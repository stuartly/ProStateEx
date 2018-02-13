add_library(BasicBlockListPass MODULE
        # List your source files here.
        GetBasicBlockList.cpp
        )

target_link_libraries(BasicBlockListPass CommonLib)

# Use C++11 to compile our pass (i.e., supply -std=c++11).
target_compile_features(BasicBlockListPass PRIVATE cxx_range_for cxx_auto_type)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(BasicBlockListPass PROPERTIES
        COMPILE_FLAGS "-fno-rtti"
        )
