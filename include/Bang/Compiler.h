#ifndef COMPILER_H
#define COMPILER_H

#include "Bang/List.h"
#include "Bang/Path.h"

NAMESPACE_BANG_BEGIN

class Compiler
{
public:
    enum OutputType
    {
        Object,
        SharedLib,
        Executable
    };

    struct Job
    {
        List<Path> includePaths;
        List<Path> libDirs;
        List<String> libraries;
        List<String> flags;
        Path outputFile;
        OutputType outputMode = OutputType::Executable;

        void AddInputFile(const Path &path);
        void AddInputFiles(const Array<Path> &paths);
        void AddInputFile(const String &path);
        void AddInputFiles(const Array<String> &paths);
        const List<String>& GetInputFiles() const;

    private:
        List<String> inputFiles; // cpp or objects
    };

    struct Result
    {
        Job compileJob;
        bool success;
        String output;
    };

    static Result Compile(const Job &compileJob);

private:
    Compiler();
};

NAMESPACE_BANG_END

#endif // COMPILER_H
