#include "Bang/Compiler.h"

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/StreamOperators.h"
#include "Bang/SystemUtils.h"

using namespace Bang;

Compiler::Result Compiler::Compile(const Compiler::Job &job)
{
    constexpr bool gcc =
#ifdef __linux__
        true
#else
        false
#endif
        ;

    Array<String> args;

    // Output
    switch (job.outputMode)
    {
        case OutputType::OBJECT:
            if (gcc)
            {
                args.PushBack("-c");
            }
            else
            {
                args.PushBack("/Fo" + job.outputFile.GetAbsolute());
            }
            break;

        case OutputType::EXECUTABLE:
            if (gcc)
            {
                args.PushBack("-c");
            }
            else
            {
                args.PushBack("/MT" + job.outputFile.GetAbsolute());
            }
            break;

        case OutputType::SHARED_LIB:
            if (gcc)
            {
                args.PushBack("-shared");
            }
            else
            {
                args.PushBack("/LD" + job.outputFile.GetAbsolute());
            }
            break;
    }

    // Flags
    args.PushBack(job.flags);

    // Input Files
    args.PushBack(job.GetInputFiles());

    // Include paths
    const String incPrefix = (gcc ? "-I" : "/I");
    Array<String> incPaths = job.includePaths.To<Array, String>();
    for (String &incPath : incPaths)
    {
        incPath.Prepend(incPrefix + "\"");
        incPath.Append("\"");
    }
    args.PushBack(incPaths);

    // Output file for gcc
    if (gcc)
    {
        args.PushBack(Array<String>({"-o", job.outputFile.GetAbsolute()}));
    }

    // Linking
    if (job.libraries.Size() >= 1)
    {
        if (!gcc)
        {
            args.PushBack("/link");
        }

        // Library directories
        const String libDirPrefix = (gcc ? "-L" : "/LIBPATH:");
        Array<String> libDirs = job.libDirs.To<Array, String>();
        for (String &libDir : libDirs)
        {
            libDir.Prepend(libDirPrefix + "\"");
            libDir.Append("\"");
        }
        args.PushBack(libDirs);

        // Libraries
        const String libPrefix = (gcc ? "-l" : "");
        Array<String> libs = job.libraries.To<Array, String>();
        for (String &lib : libs)
        {
            lib.Prepend(libPrefix);
        }
        args.PushBack(libs);
    }

    Result result;
    result.compileJob = job;
    SystemUtils::System(
        job.compilerPath.GetAbsolute(), args, &result.output, &result.success);

    return result;
}

Compiler::Compiler()
{
}

void Compiler::Job::AddInputFile(const Path &path)
{
    inputFiles.PushBack("\"" + path.GetAbsolute() + "\"");
}

void Compiler::Job::AddInputFiles(const Array<Path> &paths)
{
    for (const Path &p : paths)
    {
        AddInputFile(p);
    }
}

void Compiler::Job::AddInputFile(const String &path)
{
    inputFiles.PushBack(path);
}

void Compiler::Job::AddInputFiles(const Array<String> &paths)
{
    inputFiles.PushBack(paths);
}

const Array<String> &Compiler::Job::GetInputFiles() const
{
    return inputFiles;
}
