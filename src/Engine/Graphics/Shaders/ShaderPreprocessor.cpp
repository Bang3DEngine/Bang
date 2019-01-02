#include "Bang/ShaderPreprocessor.h"

#include "Bang/Array.h"
#include "Bang/Array.tcc"
#include "Bang/Assets.h"
#include "Bang/CodePreprocessor.h"
#include "Bang/List.h"
#include "Bang/Path.h"
#include "Bang/Paths.h"
#include "Bang/String.h"

using namespace Bang;

const String ShaderPreprocessor::GLSLVersionString = "#version 330 core";

void ShaderPreprocessor::PreprocessCode(String *shaderSourceCode)
{
    const Path engShadersDir = Paths::GetEngineAssetsDir().Append("Shaders");

    Array<Path> includeDirs;
    includeDirs.PushBack(engShadersDir);
    includeDirs.PushBack(engShadersDir.Append("Include"));
    if (Assets::GetInstance())
    {
        Array<Path> lookUpPaths = Assets::GetInstance()->GetLookUpPaths();
        for (const Path &p : lookUpPaths)
        {
            includeDirs.PushBack(p);
        }
    }

    bool addVersion = !shaderSourceCode->BeginsWith("#version");
    CodePreprocessor::PreprocessCode(shaderSourceCode, includeDirs);

    String &code = *shaderSourceCode;
    if (addVersion)
    {
        code.Prepend(ShaderPreprocessor::GLSLVersionString + "\n");
    }

    Array<String> finalLines;
    List<String> lines = code.Split<List>('\n');
    for (const String &line : lines)
    {
        bool addLine = true;
        if (line.BeginsWith("#"))
        {
            if (line.BeginsWith("#cull_face"))
            {
                addLine = false;
            }
        }

        if (addLine)
        {
            finalLines.PushBack(line);
        }
    }

    *shaderSourceCode = String::Join(finalLines, "\n");
}

String ShaderPreprocessor::GetSectionSourceCode(const String &sourceCode,
                                                GL::ShaderType shaderType)
{
    ShaderSection shaderSection = Undef<ShaderSection>();
    switch (shaderType)
    {
        case GL::ShaderType::VERTEX:
            shaderSection = ShaderSection::VERTEX;
            break;
        case GL::ShaderType::GEOMETRY:
            shaderSection = ShaderSection::GEOMETRY;
            break;
        case GL::ShaderType::FRAGMENT:
            shaderSection = ShaderSection::FRAGMENT;
            break;
        default: ASSERT(false);
    }

    return GetSectionSourceCode(sourceCode, shaderSection);
}

String ShaderPreprocessor::GetSectionSourceCode(const String &sourceCode,
                                                ShaderSection shaderSection)
{
    const Array<String> keywords = GetSectionKeywords();

    String sourceCodeSection = "";
    uint keywordIndex = SCAST<uint>(shaderSection);
    if (keywordIndex != -1u)
    {
        const String keywordString = keywords[keywordIndex];
        long keywordStringIndex = sourceCode.IndexOf(keywordString);
        if (keywordStringIndex != -1)
        {
            long beginCodeIndex = sourceCode.IndexOf("\n", keywordStringIndex);
            if (beginCodeIndex != -1)
            {
                ++beginCodeIndex;
                long endCodeIndex = -1;
                for (uint i = 0; i < keywords.Size(); ++i)
                {
                    if (i != keywordStringIndex)
                    {
                        endCodeIndex =
                            sourceCode.IndexOf(keywords[i], beginCodeIndex);
                        if (endCodeIndex != -1)
                        {
                            --endCodeIndex;
                            break;
                        }
                    }
                }

                sourceCodeSection = sourceCode.SubString(
                    beginCodeIndex,
                    (endCodeIndex >= 0 ? endCodeIndex : String::npos));
            }
        }
    }

    return sourceCodeSection;
}

ShaderProgramProperties ShaderPreprocessor::GetShaderProperties(
    const String &sourceCode)
{
    String propertiesCode =
        GetSectionSourceCode(sourceCode, ShaderSection::PROPERTIES);

    ShaderProgramProperties spProps;
    Array<String> sourceCodeLines = sourceCode.Split<Array>('\n');
    for (const String &line : sourceCodeLines)
    {
        if (line.BeginsWith("#"))
        {
            Array<String> lineParts = line.Split<Array>(' ');
            if (lineParts.Size() >= 2)
            {
                const String keyStr = lineParts[0].SubString(1);
                const String &valueStr = lineParts[1];

                if (keyStr == "cull_face")
                {
                    const Array<String> keywordsStr = {
                        "none", "back", "front", "front_and_back"};
                    const Array<GL::CullFaceExt> keywordValues = {
                        GL::CullFaceExt::NONE,
                        GL::CullFaceExt::BACK,
                        GL::CullFaceExt::FRONT,
                        GL::CullFaceExt::FRONT_AND_BACK};

                    for (uint i = 0; i < keywordsStr.Size(); ++i)
                    {
                        if (valueStr == keywordsStr[i])
                        {
                            spProps.SetCullFace(keywordValues[i]);
                            break;
                        }
                    }
                }
                else if (keyStr == "wireframe")
                {
                    spProps.SetWireframe((valueStr == "true"));
                }
                else if (keyStr == "line_width")
                {
                    spProps.SetLineWidth(String::ToFloat(valueStr));
                }
                else if (keyStr == "render_pass")
                {
                    const Array<String> keywordsStr = {
                        "scene_opaque",
                        "scene_decals",
                        "scene_transparent",
                        "scene_before_adding_lights",
                        "scene_after_adding_lights",
                        "canvas",
                        "canvas_postprocess",
                        "overlay",
                        "overlay_postprocess"};
                    const Array<RenderPass> keywordValues = {
                        RenderPass::SCENE_OPAQUE,
                        RenderPass::SCENE_DECALS,
                        RenderPass::SCENE_TRANSPARENT,
                        RenderPass::SCENE_BEFORE_ADDING_LIGHTS,
                        RenderPass::SCENE_AFTER_ADDING_LIGHTS,
                        RenderPass::CANVAS,
                        RenderPass::CANVAS_POSTPROCESS,
                        RenderPass::OVERLAY,
                        RenderPass::OVERLAY_POSTPROCESS};

                    for (uint i = 0; i < keywordsStr.Size(); ++i)
                    {
                        if (valueStr == keywordsStr[i])
                        {
                            spProps.SetRenderPass(keywordValues[i]);
                            break;
                        }
                    }
                }
            }
        }
    }

    return spProps;
}

Array<String> ShaderPreprocessor::GetSectionKeywords()
{
    const String PropertiesKeyWord = "#properties";
    const String VertexKeyWord = "#vertex";
    const String GeometryKeyWord = "#geometry";
    const String FragmentKeyWord = "#fragment";
    const Array<String> keywords = {
        VertexKeyWord, GeometryKeyWord, FragmentKeyWord, PropertiesKeyWord};
    return keywords;
}
