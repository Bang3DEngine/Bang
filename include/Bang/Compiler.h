#ifndef COMPILER_H
#define COMPILER_H

#include "Bang/Array.h"
#include "Bang/BangDefines.h"
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
        Array<Path> includePaths;
        Array<Path> libDirs;
        Array<String> libraries;
        Array<String> flags;
        Path outputFile;
        OutputType outputMode = OutputType::EXECUTABLE;

        void AddInputFile(const Path &path);
        void AddInputFiles(const Array<Path> &paths);
        void AddInputFile(const String &path);
        void AddInputFiles(const Array<String> &paths);
        const Array<String> &GetInputFiles() const;

    private:
        Array<String> inputFiles;  // cpp or objects
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
}  // namespace Bang

#endif  // COMPILER_H
