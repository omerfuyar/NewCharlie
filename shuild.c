#define SHUC_SHORT_LOG
#define SHUC_NO_RUN_LOG
#define SHUC_ENABLE_INCREMENTAL
#define SHUILD_IMPLEMENTATION
#include "dependencies/shuild/shuild.h"

int main(int argc, char **argv)
{
    SHU_CompilerTryConfigure("gcc");
    SHU_UtilAutomate(argc, argv);

    SHU_CompilerAddFlags(SHUM_FLAGS_STANDARD_C23);

    if (argc > 1)
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_DEBUG SHUM_FLAGS_WARNING_HIGH SHUM_FLAGS_WARNING_ERROR);
    }
    else
    {
        SHU_CompilerAddFlags(SHUM_FLAGS_OPTIMIZATION_HIGH);
    }

    SHU_ModuleBegin("NewCharlie", NULL);
    SHU_ModuleAddIncludeDirectory("include/");
    SHU_ModuleAddIncludeDirectory("dependencies/");
    SHU_ModuleAddSourceFile("src/");
    SHU_ModuleCompile(NULL, SHUM_MODULE_EXECUTABLE);

    return 0;
}
