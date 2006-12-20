//------------------------------------------------------------------------------
//  nglshaderinclude.cc
//  2004 Haron
//------------------------------------------------------------------------------
#include "opengl/nglshaderinclude.h"
#include "kernel/nfileserver2.h"

//------------------------------------------------------------------------------
/**
*/
nGLShaderInclude::nGLShaderInclude() :
    tmpFilePath(""),
    tmpFile(NULL)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
nGLShaderInclude::~nGLShaderInclude()
{
    this->End();
}

//------------------------------------------------------------------------------
/**
    Load shader file.
*/
bool
nGLShaderInclude::Begin(const nString& sourceFile)
{
    n_assert(!this->tmpFile);

    // mangle pathname
    nString mangledPath;
    mangledPath = nFileServer2::Instance()->ManglePath(sourceFile.Get());

    // check if the shader file actually exist, a non-existing shader file is
    // not a fatal error (result is that no rendering will be done)
    if (!nFileServer2::Instance()->FileExists(mangledPath.Get()))
    {
        n_printf("nGLShaderInclude::Begin() WARNING: shader file '%s' does not exist!\n", mangledPath.Get());
        return false;
    }

    this->tmpFile = nFileServer2::Instance()->NewFileObject();
    n_assert(this->tmpFile);

    this->tmpFilePath = mangledPath.ExtractDirName();
    this->tmpFilePath.Append("__tmp__.");
    this->tmpFilePath.Append(mangledPath.GetExtension());
    
#ifdef __WIN32__
    this->tmpFilePath.ConvertBackslashes();
#endif

    if (!this->tmpFile->Open(this->tmpFilePath.Get(), "rw"))
    {
        n_error("nGLShaderInclude::Begin(): Could not create file %s\n", this->tmpFilePath.Get());
        this->tmpFile->Release();
        return false;
    }

    Include(this->tmpFile, mangledPath, nArray<nString>());

    this->tmpFile->Seek(0, nFile::START);

    return true;
}

//------------------------------------------------------------------------------
/**
*/
void
nGLShaderInclude::GetSource(nString& src)
{
    n_assert(this->tmpFile);
    int s = this->tmpFile->GetSize();
    if (s > 0)
    {
        char* buf = n_new_array(char, s);
        int bytes_read = this->tmpFile->Read(buf, s);
        //char line[N_MAXPATH];
        //while (this->tmpFile->GetS(line, sizeof(line)))
        //{
        //    src.Append(line);
        //    n_printf("TT: %s", line);
        //}
        if (bytes_read == s )
        {
            src.Set(buf, s);
        }
        //else
        //{
        //    src.Clear();
        //}
        //n_printf("TEST(%d):\n%s\n;TEST\n", s, src.Get());
        n_delete_array(buf);
    }
}

//------------------------------------------------------------------------------
/**
*/
void
nGLShaderInclude::End()
{
    if (this->tmpFile)
    {
        this->tmpFile->Close();
        this->tmpFile->Release();
        this->tmpFile = NULL;
    }
    
    if (!this->tmpFilePath.IsEmpty() && nFileServer2::Instance()->FileExists(this->tmpFilePath.Get()))
    {
        nFileServer2::Instance()->DeleteFile(this->tmpFilePath.Get());
    }
    
    this->tmpFilePath = "";
}

//------------------------------------------------------------------------------
/**
    Recursive #include directives substituting
*/
bool
nGLShaderInclude::Include(nFile* dstfile, const nString& srcfile, nArray<nString>& includes)
{
    //n_printf("<CHECK>%s</CHECK>\n", srcfile.Get());

    nString shaderdir(srcfile.ExtractDirName());
    nFile* f = nFileServer2::Instance()->NewFileObject();
    n_assert(f);

    // open the file
    if (!f->Open(srcfile.Get(), "r"))
    {
        n_error("nGLShaderInclude::Include(): Could not open file %s\n", srcfile.Get());
        f->Release();
        return false;
    }

    includes.PushBack(srcfile);

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

    includes.Erase(includes.Size()-1); // includes.PopBack();

    f->Close();
    f->Release();
    return true;
}
