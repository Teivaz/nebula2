//------------------------------------------------------------------------------
//  ncgfxshaderinclude.cc
//  2004 Haron
//------------------------------------------------------------------------------
#include "opengl/ncgfxshaderinclude.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nCgFXShaderInclude::nCgFXShaderInclude() :
    tmpFilePath("")
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nCgFXShaderInclude::~nCgFXShaderInclude()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Load CgFX-shader file.
*/
bool
nCgFXShaderInclude::Begin(const nString& sourceFile)
{
    // mangle pathname
    nString mangledPath;
    mangledPath = nFileServer2::Instance()->ManglePath(sourceFile.Get());

    // check if the shader file actually exist, a non-existing shader file is
    // not a fatal error (result is that no rendering will be done)
    if (!nFileServer2::Instance()->FileExists(mangledPath.Get()))
    {
        n_printf("nCgFXShaderInclude::Begin() WARNING: shader file '%s' does not exist!\n", mangledPath.Get());
        return false;
    }

    nFile* f = nFileServer2::Instance()->NewFileObject();
    n_assert(f);

    this->tmpFilePath.Append(mangledPath.ExtractDirName());
    this->tmpFilePath.Append("__tmp__.fx");

    if (!f->Open(this->tmpFilePath.Get(), "w"))
    {
        n_error("nCgFXShaderInclude::Begin(): Could not create file %s\n", this->tmpFilePath.Get());
        f->Release();
        return false;
    }

    //mangledPath = goochyFX;
    Include(f, mangledPath, nArray<nString>());

#ifdef __WIN32__
    this->tmpFilePath.ConvertBackslashes();
#endif

    f->Close();
    f->Release();

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nCgFXShaderInclude::End()
{
    if (!nFileServer2::Instance()->FileExists(this->tmpFilePath.Get()))
    {
        //nFileServer2::Instance()->DeleteFile(this->tmpFilePath.Get());
        this->tmpFilePath = "";
    }
}

//------------------------------------------------------------------------------
/**
    Recursive #include directives substituting
*/
bool
nCgFXShaderInclude::Include(nFile* dstfile, const nString& srcfile, nArray<nString>& includes)
{
    //n_printf("<CHECK>%s</CHECK>\n", srcfile.Get());

    nString shaderdir(srcfile.ExtractDirName());
    nFile* f = nFileServer2::Instance()->NewFileObject();
    n_assert(f);

    // open the file
    if (!f->Open(srcfile.Get(), "r"))
    {
        n_error("nCgFXShaderInclude::Include(): Could not open file %s\n", srcfile.Get());
        f->Release();
        return false;
    }

    char line[N_MAXPATH];
    nString includeFile;

    while (f->GetS(line, sizeof(line))) // scan each line from source file
    {
        bool needSave = true;
        char ifname[400];
        const char *word;
        nString sline(line);

        word = sline.GetFirstToken(" \t"); // first not whitespace

        //n_printf("<LINE>%s\n<WORD>%s\n", line, word);

        includeFile.Set(""); // clear content

        if (NULL != word)
            if (n_stricmp(word, "#include") == 0) // we found 'include' directive
            {
                word = sline.GetNextToken(" \t\n");

                n_strncpy2(ifname, &word[1], strlen(word)-2); // cut first and last (") (#include "lib.fx")
                nString fn = nString(ifname).ExtractFileName();
                if (0 == includes.Find(fn)) // check if this file was not included previously
                {
                    //n_printf("<FILE>%s\n", fn.Get());
                    needSave = false;
                    includeFile.Append(shaderdir);
                    includeFile.Append(ifname);
                    includes.Append(fn);

                    if (!this->Include(dstfile, includeFile, includes))
                    {
                        f->Close();
                        f->Release();
                        return false;
                    }
                    //n_printf("\t<INCLUDE>%s</INCLUDE>\n", includeFile.Get());
                }
            }
            else if (n_stricmp(word, "shared") == 0) // we found 'shared' keyword
            {
                //dstfile->PutS("uniform ");
                dstfile->PutS(sline.GetNextToken("\n")); // just skip it
                needSave = false;
            }

        if (needSave) // simply copy a string
        {
            dstfile->PutS(line);
            dstfile->PutChar((char)0x0A); // only for CgFX needs
        }
    }

    f->Close();
    f->Release();
    return true;
}
