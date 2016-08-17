This is a project to bring out reflection capabilities in c++ through the use of debug symbol information.


## Known issues

# Function symbols dont have type information!
If you are building with MSVC it may be due to linking with incremental linking.
If your symbol named _func_ looks like @ILT+755(?func@@YAXXZ) in symbol::getName() then you might want to turn off incremental linking in the linker settings.
If you are using cmake, you can do it with the following line

    set_target_properties(${TEST_TARGET} PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")

See how it is done in the test/CMakeLists.txt file.