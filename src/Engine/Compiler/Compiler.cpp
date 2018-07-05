#include "Bang/Compiler.h"

#include "Bang/Debug.h"
#include "Bang/SystemUtils.h"
#include "Bang/BangPreprocessor.h"

USING_NAMESPACE_BANG

Compiler::Result Compiler::Compile(const Compiler::Job &job)
{
    List<String> args;

    // Output mode
    if      (job.outputMode == OutputType::OBJECT)     { args.PushBack("-c"); }
    else if (job.outputMode == OutputType::SHARED_LIB)  { args.PushBack("-shared"); }
    else if (job.outputMode == OutputType::EXECUTABLE) { args.PushBack("-c"); }

    // Flags
    args.PushBack(job.flags);

    // Input Files
    args.PushBack(job.GetInputFiles());

    // Include paths
    List<String> incPaths = job.includePaths.To<List,String>();
    for (String &incPath : incPaths) { incPath.Prepend("-I"); }
    args.PushBack(incPaths);

    // Library directories
    List<String> libDirs = job.libDirs.To<List,String>();
    for (String &libDir : libDirs) { libDir.Prepend("-L"); }
    args.PushBack(libDirs);

    // Libraries
    List<String> libs = job.libraries.To<List,String>();
    for (String &lib : libs) { lib.Prepend("-l"); }
    args.PushBack(libs);

    // Output file
    args.PushBack( List<String>({"-o", job.outputFile.GetAbsolute()}) );


    Result result;
    result.compileJob = job;
    SystemUtils::System("/usr/bin/g++",
                        args,
                        &result.output,
                        &result.success);

    return result;
}

Compiler::Compiler()
{
}

void Compiler::Job::AddInputFile(const Path &path)
{
    inputFiles.PushBack( path.GetAbsolute() );
}

void Compiler::Job::AddInputFiles(const Array<Path> &paths)
{
    Array<String> pathsStr;
    for (const Path &p : paths) { pathsStr.PushBack(p.GetAbsolute()); }
    inputFiles.PushBack(pathsStr);
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
