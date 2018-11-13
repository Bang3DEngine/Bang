#include "Bang/Compiler.h"

#include <vector>

#include "Bang/Array.tcc"
#include "Bang/Debug.h"
#include "Bang/File.h"
#include "Bang/Paths.h"
#include "Bang/Random.h"
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
    constexpr bool msvc = !gcc;

    Array<String> args;

    // Output
    String debugAppend = "";
#ifdef _WIN32
#ifdef DEBUG
    debugAppend = "d";
#endif
#endif
    if (gcc)
    {
        switch (job.outputMode)
        {
            case OutputType::OBJECT: args.PushBack("-c"); break;
            case OutputType::EXECUTABLE: args.PushBack("-c"); break;
            case OutputType::SHARED_LIB: args.PushBack("-shared"); break;
        }
    }
    else if (msvc)
    {
        args.PushBack("/EHsc");
    }

    // Include paths (must be before input files for msvc)
    const String incPrefix = (gcc ? "-I" : "/I");
    Array<String> incPaths = job.includePaths.To<Array, String>();
    for (String &incPath : incPaths)
    {
        incPath.Prepend(incPrefix + "\"");
        incPath.Append("\"");
    }
    args.PushBack(incPaths);

    // Flags (must be before input files for msvc)
    args.PushBack(job.flags);

    // Input Files (must be before msvc /link option)
    Array<String> inputPaths = job.GetInputFiles().To<Array, String>();
    args.PushBack(inputPaths);

    if (msvc)
    {
        if (job.outputMode == OutputType::EXECUTABLE)
        {
            args.PushBack("/link");
        }

        if (job.outputMode == OutputType::OBJECT)
        {
            args.PushBack("/c");
            args.PushBack("/Fo\"" + job.outputFile.GetAbsolute() + "\"");
        }
        else
        {
            args.PushBack("/link");
            args.PushBack("/MACHINE:x64");
            args.PushBack("/MANIFEST:NO");
            args.PushBack("/NXCOMPAT");
            args.PushBack("/IGNOREDL");
            args.PushBack("/DLL");
            args.PushBack("/OUT:\"" + job.outputFile.GetAbsolute() + "\"");
        }

        args.PushBack("/MD" + debugAppend);
    }

    // Output file for gcc
    if (gcc)
    {
        args.PushBack(Array<String>({"-o", job.outputFile.GetAbsolute()}));
    }

    // Libraries stuff
    if (job.libraries.Size() >= 1)
    {
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
        Array<String> libs = job.libraries.To<Array, String>();
        const String libPrefix = (gcc ? "-l" : "");
        for (String &lib : libs)
        {
            lib.Prepend(libPrefix);
        }

        args.PushBack(libs);
    }

    Result result;
    result.compileJob = job;
    Path compilerPath =
        (job.outputMode == OutputType::SHARED_LIB ? Paths::GetLinkerPath()
                                                  : Paths::GetCompilerPath());
    String compileCommand = "\"" + compilerPath.GetAbsolute() + "\"";
    if (msvc)
    {
        // We need to prepare the environment. To execute several commands,
        // we create a tmp file having all these commands

        Path tmpCommandPath;
        do
        {
            tmpCommandPath = Paths::GetProjectLibrariesDir()
                                 .Append(String::ToString(Time::GetNow()) +
                                         String::ToString(Random::GetValue()))
                                 .WithExtension("bat");
        } while (tmpCommandPath.Exists());

        String prepareEnvCommand =
            "\"" + Paths::GetMSVCConfigureArchitectureBatPath().GetAbsolute() +
            "\" x64";
        compileCommand = compileCommand + " " + String::Join(args, " ");

        File::Write(tmpCommandPath,
                    String("CALL ") + prepareEnvCommand + " \n " +
                        String("CALL ") + compileCommand);

        SystemUtils::System(
            tmpCommandPath.GetAbsolute(), {}, &result.output, &result.success);

        // File::Remove(tmpCommandPath);
    }
    else
    {
        SystemUtils::System(
            compileCommand, args, &result.output, &result.success);
    }

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
