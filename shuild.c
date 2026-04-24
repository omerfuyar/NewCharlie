#define SHUC_SHORT_LOG
#define SHUC_NO_RUN_LOG
#define SHUC_ENABLE_INCREMENTAL
#define SHUILD_IMPLEMENTATION
#include "dependencies/shuild/shuild.h"

// https://canva.link/9n9dtdppwdr2202

#define STACK_SIZE "4194304"

int main(int argc, char **argv)
{
    SHU_CompilerConfigure(SHUM_COMPILER_GCC, "x86_64-w64-mingw32-gcc");
    // SHU_CompilerTryConfigure("gcc");
    SHU_UtilAutomate(argc, argv);

    SHU_CompilerAddFlags(SHUM_FLAGS_STANDARD_C23 " -static -fno-stack-protector");

    if (argc > 1)
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_WARNING_HIGH);
        SHU_CompilerAddFlags(SHUM_FLAGS_DEBUG SHUM_FLAGS_WARNING_ERROR " -Wno-missing-declarations -Wno-switch -Wno-format-truncation -Wno-unused-function -DDEBUG -Wno-format-nonliteral");
    }
    else
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    }

    //! SHU_CompilerAddFlags(" -Wl,--stack," STACK_SIZE);
    //! SHU_CompilerAddFlags(" -Wl,-z,stack-size=" STACK_SIZE);

    SHU_ModuleBegin("NewCharlie", NULL);
    SHU_ModuleAddIncludeDirectory("include/");
    SHU_ModuleAddIncludeDirectory("dependencies/");
    SHU_ModuleAddSourceFile("src/");
    SHU_ModuleCompile(NULL, SHUM_MODULE_EXECUTABLE);

    return 0;
}
