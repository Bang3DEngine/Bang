#include "Bang/ImageIODDS.h"

#include "Bang/GL.h"
#include "Bang/Path.h"
#include "Bang/String.h"
#include "Bang/Texture2D.h"
#include "Bang/Texture3D.h"
#include "Bang/nv_dds.h"

using namespace Bang;

void ImageIODDS::ImportDDS2D(const Path &filepath, Texture2D *tex, bool *ok)
{
    nv_dds::CDDSImage ddsImg;
    ddsImg.load(filepath.GetAbsolute(), true);

    GL::Push(tex->GetGLBindTarget());

    tex->Bind();

    tex->CreateEmpty(ddsImg.get_width(), ddsImg.get_height());
    ddsImg.upload_texture2D(0, GLCAST(tex->GetGLBindTarget()));
    tex->SetFormat(SCAST<GL::ColorFormat>(ddsImg.get_format()));

    GL::Pop(tex->GetGLBindTarget());

    if (ok)
    {
        *ok = true;
    }
}

void ImageIODDS::ImportDDS3D(const Path &filepath, Texture3D *tex, bool *ok)
{
    nv_dds::CDDSImage ddsImg;
    ddsImg.load(filepath.GetAbsolute(), true);

    GL::Push(tex->GetGLBindTarget());

    tex->Bind();

    tex->CreateEmpty(
        ddsImg.get_width(), ddsImg.get_height(), ddsImg.get_depth());
    ddsImg.upload_texture3D();
    tex->SetFormat(SCAST<GL::ColorFormat>(ddsImg.get_format()));

    GL::Pop(tex->GetGLBindTarget());

    if (ok)
    {
        *ok = true;
    }
}
