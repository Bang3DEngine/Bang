#include "Bang/Compiler.h"

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/StreamOperators.h"
#include "Bang/SystemUtils.h"

using namespace Bang;

Compiler::Result Compiler::Compile(const Compiler::Job &job)
{
    List<String> args;

    // Output mode
    switch (job.outputMode)
    {
        case OutputType::OBJECT:
        case OutputType::EXECUTABLE: args.PushBack("-c"); break;

        case OutputType::SHARED_LIB: args.PushBack("-shared"); break;
    }

    // Flags
    args.PushBack(job.flags);

    // Input Files
    args.PushBack(job.GetInputFiles());

    // Include paths
    List<String> incPaths = job.includePaths.To<List, String>();
    for (String &incPath : incPaths)
    {
        incPath.Prepend("-I\"");
        incPath.Append("\"");
    }
    args.PushBack(incPaths);

    // Library directories
    List<String> libDirs = job.libDirs.To<List, String>();
    for (String &libDir : libDirs)
    {
        libDir.Prepend("-L\"");
        libDir.Append("\"");
    }
    args.PushBack(libDirs);

    // Libraries
    List<String> libs = job.libraries.To<List, String>();
    for (String &lib : libs)
    {
        lib.Prepend("-l");
    }
    args.PushBack(libs);

    // Output file
    args.PushBack(List<String>({"-o", job.outputFile.GetAbsolute()}));

    Result result;
    result.compileJob = job;
    SystemUtils::System("/usr/bin/g++", args, &result.output, &result.success);

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

const List<String> &Compiler::Job::GetInputFiles() const
{
    return inputFiles;
}
