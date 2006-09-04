//------------------------------------------------------------------------------
/**
    @class nMemManager
    @ingroup NebulaKernelMemory

    Nebula memory management class. Optimizes memory allocations and tracks
    memory leaks and corruptions.

    (C) 1999 RadonLabs GmbH
*/
#include "kernel/ntypes.h"

#if defined(__NEBULA_MEM_MANAGER__) || defined(DOXYGEN)

#include "kernel/nmutex.h"
#include "util/nlist.h"
#include "util/nnode.h"
#include "kernel/nkernelserver.h"

#undef n_malloc
#undef n_calloc
#undef n_free

struct nChunkHeader
{
    nNode nd;           // muss im ersten Element bleiben!
    int bnum;           // Block-Anzahl
    int bsize;          // Block-Groesse
};

struct nBlockHeader
{
    enum
    {
        N_SRCNAME_LEN = 16,
    };
    struct nBlockHeader *next;
    int size;
    char src_name[N_SRCNAME_LEN];
    int  src_line;
    int magic;
};

struct nBlockPostfix
{
    int magic;
};

class nMemManager
{
public:
    enum
    {
        N_MINBLOCK = 4,     ///< minimal block size is 2^4
        N_MAXBLOCK = 10,    ///< maximum block size is 2^10
        N_NUMBLOCKSIZES = (1+(N_MAXBLOCK - N_MINBLOCK)),
        N_PREFIX_MAGIC_COOKIE  = 0xDEADBEEF,
        N_POSTFIX_MAGIC_COOKIE = 0xFEEBDAED,
    };

    nMutex mutex;
    nList chunks;
    nBlockHeader *blocks[N_NUMBLOCKSIZES];
    int b_size[N_NUMBLOCKSIZES];
    int b_num[N_NUMBLOCKSIZES];

    /// constructor
    nMemManager();
    /// destructor
    ~nMemManager();
    /// Get new chunk of memory
    nBlockHeader *NewChunk(int, int);
    /// Release a chunk of memory
    void FreeChunk(nChunkHeader *);
    /// Allocate memory
    void *Malloc(int, const char *, int);
    /// Reallocate memory
    void *Realloc(void *, int, const char *, int);
    /// Free memory
    void Free(void *);
};

static nMemManager mem;

int n_memallocated = 0;
int n_memused      = 0;
int n_memnumalloc  = 0;

//------------------------------------------------------------------------------
/**
    Creates the memory chunks.
*/
nMemManager::nMemManager()
{
    int i;
    for (i=0; i<(int)N_NUMBLOCKSIZES; i++)
    {
        b_size[i] = (1<<(i+N_MINBLOCK));
        b_num[i]  = (16*1024) / b_size[i];
        blocks[i] = this->NewChunk(b_size[i],b_num[i]);
        if (NULL == blocks[i]) n_error("Out Of Memory!");
    }
}

//------------------------------------------------------------------------------
/**
    Frees all memory chunks. Detects memory leaks.

    @todo memlog is disabled, since it clutters the working directory
*/
nMemManager::~nMemManager()
{
//    FILE *fp = NULL;

    nChunkHeader *ch;
    while ((ch = (nChunkHeader *) this->chunks.GetHead())) {
        char *b = ((char *)ch) + sizeof(nChunkHeader);

/*
    FIXME: memlog disabled temporally, since it clutters the working directory

        int i;
        for (i=0; i<ch->bnum; i++,b+=ch->bsize) {
            nBlockHeader *bh = (nBlockHeader *)b;
            if (bh->size != 0) {
                // Erster MemLeak? Dann Logfile erzeugen.
                if (!fp) {
                    fp = fopen("n_memlog.txt","w");
                    if (fp) {
                        time_t ltime;
                        struct tm *t;
                        time(&ltime);
                        t = localtime(&ltime);
                        fprintf(fp,"Nebula Memory Log\n=================\n%s\n\n",asctime(t));
                    }
                }
                if (fp) {
                    fprintf(fp,"*** MEM LEAK DETECTED! ***\n");
                    fprintf(fp,"size = %d\n",bh->size);
                    fprintf(fp,"src  = %s\n",bh->src_name);
                    fprintf(fp,"line = %d\n",bh->src_line);
                }
            }
        }
*/
        this->FreeChunk(ch);
    }
/*
    if (fp) {
        fprintf(fp,"---\nEOF\n");
        fclose(fp);
    }
*/
}

//------------------------------------------------------------------------------
/**
    Allocate a new chunk and return pointer to first block in chunk.

    @param bsize block size
    @param bnum number of blocks in chunk
*/
nBlockHeader *nMemManager::NewChunk(int bsize, int bnum)
{
    int block_size = bsize + sizeof(nBlockHeader) + sizeof(nBlockPostfix);
    int chunk_size = sizeof(nChunkHeader) + (block_size*bnum);
    char *c = (char *) malloc(chunk_size);
    if (c) {
        n_memallocated += chunk_size;

        // initialize chunk header
        nChunkHeader *ch = (nChunkHeader *) c;
        ch->nd.nNode::nNode();      // manually construct node.
        ch->bnum  = bnum;
        ch->bsize = block_size;
        this->chunks.AddTail((nNode *)ch);

        // initialize the blocks
        char *f = c + sizeof(nChunkHeader); // first Block
        char *p = c + chunk_size;           // last Block + 1
        char *next = NULL;
        while (p > f) {
            p -= block_size;
            nBlockHeader *bh = (nBlockHeader *)p;
            bh->next = (nBlockHeader *) next;
            bh->size = 0;
            strcpy(bh->src_name,"<new>");
            bh->src_line = 0;
            bh->magic = N_PREFIX_MAGIC_COOKIE;
            next = p;
        }
        return (nBlockHeader *) f;
    }
    return NULL;
}

//------------------------------------------------------------------------------
/**
    Releases the given chunk back to the memory pool.

    @param ch pointer to chunk to release
*/
void nMemManager::FreeChunk(nChunkHeader *ch)
{
    n_memallocated -= sizeof(nChunkHeader) + (ch->bsize*ch->bnum);
    ch->nd.Remove();
    ch->nd.nNode::~nNode();
    free((void *)ch);
}

//------------------------------------------------------------------------------
/**
    Custom Malloc. Uses given source file and line to report memory leaks.

    @param size amount of memory to allocate
    @param src_name name of source file
    @param src_line line number within source file
*/
void *nMemManager::Malloc(int size, const char *src_name, int src_line)
{
    mutex.Lock();
    char *p = NULL;

    // ensure source name fits
    int len = strlen(src_name)+1;
    int off = len - nBlockHeader::N_SRCNAME_LEN;
    if (off < 0) off = 0;

    if (size > (1<<N_MAXBLOCK)) {
        // memory wanted is bigger than max block size, so create a new individual chunk.
        p = (char *) this->NewChunk(size,1);
        if (p) {
            nBlockHeader *bh  = (nBlockHeader *) p;
            nBlockPostfix *bp = (nBlockPostfix *) (p + size + sizeof(nBlockHeader));
            bh->next = (nBlockHeader *) -1L;    // flag for n_free()/Free()
            bh->size = size;
            strcpy(bh->src_name,&(src_name[off]));
            bh->src_line = src_line;
            bh->magic = N_PREFIX_MAGIC_COOKIE;
            bp->magic = N_POSTFIX_MAGIC_COOKIE;
            p += sizeof(nBlockHeader);
            n_memused += size;
            n_memnumalloc++;
        }
    } else {
        // find suitable block size
        int i;
        for (i=0; i<(int)(N_NUMBLOCKSIZES-1); i++) {
            int mask = ~(b_size[i]-1);
            if ((size & mask) == 0) {
                break;
            }
        }
        p = (char *) blocks[i];
        if (p) {
            nBlockHeader  *bh = (nBlockHeader *)  p;
            nBlockPostfix *bp = (nBlockPostfix *) (p + size + sizeof(nBlockHeader));
            blocks[i] = bh->next;
            if (NULL == blocks[i]) {
                // need a new chunk
                blocks[i] = this->NewChunk(b_size[i],b_num[i]);
                if (NULL == blocks[i]) n_error("Out Of Memory!\n");
            }
            bh->next  = (nBlockHeader *) i;
            bh->size  = size;
            strcpy(bh->src_name,&(src_name[off]));
            bh->src_line = src_line;
            bh->magic = N_PREFIX_MAGIC_COOKIE;
            bp->magic = N_POSTFIX_MAGIC_COOKIE;
            p += sizeof(nBlockHeader);
            n_memused += size;
            n_memnumalloc++;
        }
    }
    mutex.Unlock();
    return p;
}


//------------------------------------------------------------------------------
/**
    Custom simplistic Realloc which just uses malloc & memcpy rather than
    actually trying to 'grow' the memory in place.

    @param oldp pointer to memory which should be grown/shrunk
    @param size amount of memory to reallocate
    @param src_name name of source file
    @param src_line line number within source file
*/
void *nMemManager::Realloc(void *oldp, int size, const char *src_name, int src_line)
{
    if(oldp == NULL)
    {
        return this->Malloc(size, src_name, src_line);
    }

    if(size == 0)
    {
        this->Free(oldp);
    }

    char *oldpc = (char *) oldp;
    nBlockHeader  *bh = (nBlockHeader *) (oldpc - sizeof(nBlockHeader));

    int old_size = bh->size;

    void *p = this->Malloc(size, src_name, src_line);

    if(p != NULL)
    {
        memcpy(p, oldp, (size > old_size) ? old_size : size);
        this->Free(oldp);
    }

    return p;
}


//------------------------------------------------------------------------------
/**
    Frees a previously allocated block of memory.

    @param p pointer to allocated memory.
*/
void nMemManager::Free(void *p)
{
    mutex.Lock();

    char *pc = (char *) p;
    nBlockHeader  *bh = (nBlockHeader *) (pc - sizeof(nBlockHeader));
    nBlockPostfix *bp;

    // is the block corrupt?
    if (bh->magic != (int)N_PREFIX_MAGIC_COOKIE)
    {
        n_error("Free(): START OF MEM BLOCK CORRUPTED: src=%s, line=%d\n", bh->src_name, bh->src_line);
    }
    bp = (nBlockPostfix *) (pc + bh->size);
    if (bp->magic != (int)N_POSTFIX_MAGIC_COOKIE) {
        n_error("Free(): END OF MEM BLOCK CORRUPTED: src=%s, line=%d\n", bh->src_name, bh->src_line);
    }

    // update memory stats
    n_memused -= bh->size;
    n_memnumalloc--;

    strcpy(bh->src_name,"<freed>");
    bh->src_line = 0;

    if (((int)bh->next) == -1L) {
        // block was allocated as individual chunk
        nChunkHeader *ch = (nChunkHeader *) (((char *)bh) - sizeof(nChunkHeader));
        bh->size = 0;
        this->FreeChunk(ch);
    } else {
        int i = (int) bh->next;
        nBlockHeader *next = blocks[i];
        blocks[i] = bh;
        bh->next = next;
        bh->size = 0;
    }
    mutex.Unlock();
}

//------------------------------------------------------------------------------
/**
    @ingroup NebulaKernelMemory

    Uses the memory manager.
*/
void *nn_malloc(size_t size, const char *file, int line)
{
    return mem.Malloc(size,file,line);
}

//------------------------------------------------------------------------------
/**
    @ingroup NebulaKernelMemory

    Uses the memory manager.
*/
void n_free(void *p)
{
    mem.Free(p);
}

//------------------------------------------------------------------------------
/**
    @ingroup NebulaKernelMemory

    Uses the memory manager.
*/
void *nn_calloc(size_t num, size_t size, const char *file, int line)
{
    size_t all_size = num*size;
    void *p = mem.Malloc(all_size,file,line);
    if (p) memset(p,0,sizeof(all_size));
    return p;
}

//------------------------------------------------------------------------------
/**
    @ingroup NebulaKernelMemory

    Uses the memory manager.
*/
void *nn_realloc(void *p, size_t size, const char *file, int line)
{
    return mem.Realloc(p, size, file, line);
}
//-------------------------------------------------------------------
#elif __WIN32__
//-------------------------------------------------------------------
#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#else
#include <windows.h>
#endif

//------------------------------------------------------------------------------
/**
    Uses Windows memory management.

    @todo should these be updated to use heap allocations?
    http://msdn.microsoft.com/library/default.asp?url=/library/en-us/memory/base/heap_functions.asp
*/
void *nn_malloc(size_t size, const char *file, int line)
{
    HGLOBAL ptr = GlobalAlloc(0, size);
    n_assert(ptr);
    return ptr;
}

//------------------------------------------------------------------------------
/**
    Uses Windows memory management.
*/
void nn_free(void *p)
{
    GlobalFree((HGLOBAL) p);
}

//------------------------------------------------------------------------------
/**
    Uses Windows memory management.
*/
void *nn_calloc(size_t num, size_t size, const char *, int)
{
    HGLOBAL ptr = GlobalAlloc(GMEM_ZEROINIT, size*num);
    n_assert(ptr);
    return ptr;
}


//------------------------------------------------------------------------------
/**
    Uses Windows memory management.
*/
void *nn_realloc(void *p, size_t size, const char *, int)
{
    HGLOBAL ptr = GlobalReAlloc((HGLOBAL)p, size, GMEM_MOVEABLE);
    n_assert(ptr);
    return ptr;
}
//-------------------------------------------------------------------
#else
//-------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
    Uses standard memory management.
*/
void *nn_malloc(size_t size, const char *, int)
{
    return malloc(size);
}

//------------------------------------------------------------------------------
/**
    Uses standard memory management.
*/
void n_free(void *p)
{
    free(p);
}

//------------------------------------------------------------------------------
/**
    Uses standard memory management.
*/
void *nn_calloc(size_t num, size_t size, const char *, int)
{
    return calloc(num,size);
}

//------------------------------------------------------------------------------
/**
    Uses standard memory management.
*/
void *nn_realloc(void *p, size_t size, const char *, int)
{
    return realloc(p, size);
}

//-------------------------------------------------------------------
#endif

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
