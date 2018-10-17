#ifndef COMPILER_H
#define COMPILER_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
#include "Bang/List.h"
#include "Bang/List.tcc"
#include "Bang/Path.h"
#include "Bang/String.h"

namespace Bang
{
class Compiler
{
public:
    enum OutputType
    {
        OBJECT,
        SHARED_LIB,
        EXECUTABLE
    };

    struct Job
    {
        List<Path> includePaths;
        List<Path> libDirs;
        List<String> libraries;
        List<String> flags;
        Path outputFile;
        OutputType outputMode = OutputType::EXECUTABLE;

        void AddInputFile(const Path &path);
        void AddInputFiles(const Array<Path> &paths);
        void AddInputFile(const String &path);
        void AddInputFiles(const Array<String> &paths);
        const List<String> &GetInputFiles() const;

    private:
        List<String> inputFiles;  // cpp or objects
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
}

#endif  // COMPILER_H
