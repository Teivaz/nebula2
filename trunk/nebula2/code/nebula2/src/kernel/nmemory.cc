#define N_KERNEL
//-------------------------------------------------------------------
//  nmemory.cc
//  Speicher-Manager fuer Nebula, zeit-optimiert Allokation sehr
//  kleiner Speicherbloecke.
//  (C) 1999 A.Weissflog
//-------------------------------------------------------------------
#ifndef N_TYPES_H
#include "kernel/ntypes.h"
#endif

#ifdef __NEBULA_MEM_MANAGER__

#ifndef N_MUTEX_H
#include "kernel/nmutex.h"
#endif

#ifndef N_LIST_H
#include "util/nlist.h"
#endif

#ifndef N_NODE_H
#include "util/nnode.h"
#endif

#ifndef N_KERNELSERVER_H
#include "kernel/nkernelserver.h"
#endif

#undef n_malloc
#undef n_calloc
#undef n_free

//------------------------------------------------------------------------------
//
//  NEBULA MEMORY MANAGER
//
//------------------------------------------------------------------------------
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
        N_MINBLOCK = 4,     // minimale Blockgroesse ist 2^4 
        N_MAXBLOCK = 10,    // maximale Blockgroesse ist 2^10
        N_NUMBLOCKSIZES = (1+(N_MAXBLOCK - N_MINBLOCK)),
        N_PREFIX_MAGIC_COOKIE  = 0xDEADBEEF,
        N_POSTFIX_MAGIC_COOKIE = 0xFEEBDAED,
    };
    
    nMutex mutex;
    nList chunks;
    nBlockHeader *blocks[N_NUMBLOCKSIZES];
    int b_size[N_NUMBLOCKSIZES];
    int b_num[N_NUMBLOCKSIZES];

    nMemManager();
    ~nMemManager();
    nBlockHeader *NewChunk(int, int);
    void FreeChunk(nChunkHeader *);
    void *Malloc(int, const char *, int);
    void Free(void *);
};

static nMemManager mem;

int n_memallocated = 0;
int n_memused      = 0;
int n_memnumalloc  = 0;

//-------------------------------------------------------------------
//  nMemManager()
//  27-Feb-99   floh    created
//-------------------------------------------------------------------
nMemManager::nMemManager()
{
    int i;
    for (i=0; i<(int)N_NUMBLOCKSIZES; i++) 
    {
        b_size[i] = (1<<(i+N_MINBLOCK));
        b_num[i]  = (16*1024) / b_size[i];
        blocks[i] = this->NewChunk(b_size[i],b_num[i]);
        if (NULL == blocks[i]) n_error("Out Of Mem!");
    }
}

//-------------------------------------------------------------------
//  ~nMemManager()
//  27-Feb-99   floh    created
//  28-May-99   floh    schreibt die MemLeaks jetzt in einen
//                      Logfile raus, weil stdout offensichtlich
//                      manchmal schon "zu" sein kann.
//  31-May-99   floh    schreibt MemLog nur noch, wenn auch tatsaechlich
//                      ein Memleak auftritt.
//-------------------------------------------------------------------
nMemManager::~nMemManager()
{
    // Chunkliste leermachen, und auf nicht freigegebene Bloecke
    // testen...
//    FILE *fp = NULL;
    
    nChunkHeader *ch;
    while ((ch = (nChunkHeader *) this->chunks.GetHead())) {
        char *b = ((char *)ch) + sizeof(nChunkHeader);

/*
    FIXME: memlog temporarely disabled, since it clutters the working directory

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

//-------------------------------------------------------------------
//  NewChunk()
//  Allokiert einen neuen Chunk und gibt Pointer auf ersten Block
//  im Chunk zurueck.
//  27-Feb-99   floh    created
//  13-May-99   floh    updated n_memallocated
//  27-May-99   floh    + klinkt sich jetzt in eine Chunkliste ein
//-------------------------------------------------------------------
nBlockHeader *nMemManager::NewChunk(int bsize, int bnum)
{
    int block_size = bsize + sizeof(nBlockHeader) + sizeof(nBlockPostfix);
    int chunk_size = sizeof(nChunkHeader) + (block_size*bnum);
    char *c = (char *) malloc(chunk_size);
    if (c) {
        n_memallocated += chunk_size;

        // ChunkHeader initialisieren
        nChunkHeader *ch = (nChunkHeader *) c;
        ch->nd.nNode::nNode();      // Konstruktor von nNode manuell ausfuehren
        ch->bnum  = bnum;
        ch->bsize = block_size;
        this->chunks.AddTail((nNode *)ch);

        // Blocks im Chunk initialisieren
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

//-------------------------------------------------------------------
//  FreeChunk()
//  Gibt einen allokierten Chunk wieder frei.
//  27-May-99   floh    created
//-------------------------------------------------------------------
void nMemManager::FreeChunk(nChunkHeader *ch)
{
    n_memallocated -= sizeof(nChunkHeader) + (ch->bsize*ch->bnum); 
    ch->nd.Remove();
    ch->nd.nNode::~nNode();
    free((void *)ch);
}

//-------------------------------------------------------------------
//  Malloc()
//  27-Feb-99   floh    created
//  13-May-99   floh    + updated n_memallocated, n_memused
//  27-May-99   floh    + Source-Name und Source-Zeile
//-------------------------------------------------------------------
void *nMemManager::Malloc(int size, const char *src_name, int src_line)
{
    mutex.Lock();
    char *p = NULL;

    // ermittle Offset auf die letzten N Buchstaben von src_name
    int len = strlen(src_name)+1;
    int off = len - nBlockHeader::N_SRCNAME_LEN;
    if (off < 0) off = 0;
    
    if (size > (1<<N_MAXBLOCK)) {
        // Speicherblock zu gross, also bekommt er seinen eigenen Chunk
        p = (char *) this->NewChunk(size,1);
        if (p) {
            nBlockHeader *bh  = (nBlockHeader *) p;
            nBlockPostfix *bp = (nBlockPostfix *) (p + size + sizeof(nBlockHeader));
            bh->next = (nBlockHeader *) -1L;    // damit n_free() Bescheid weiss
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
        // finde passende Blockgroesse
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
                // aktueller Chunk aufgebraucht, neuen allokieren
                blocks[i] = this->NewChunk(b_size[i],b_num[i]);
                if (NULL == blocks[i]) n_error("Out Of Mem!\n");
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

//-------------------------------------------------------------------
//  Free()
//  27-Feb-99   floh    created
//  13-May-99   floh    updated n_memallocated, n_memused
//-------------------------------------------------------------------
void nMemManager::Free(void *p)
{
    mutex.Lock();

    char *pc = (char *) p;
    nBlockHeader  *bh = (nBlockHeader *) (pc - sizeof(nBlockHeader));
    nBlockPostfix *bp;

    // ist der Block unbeschaedigt?
    if (bh->magic != (int)N_PREFIX_MAGIC_COOKIE) 
    {
        n_error("Free(): START OF MEM BLOCK CORRUPTED: src=%s, line=%d\n", bh->src_name, bh->src_line);
    }
    bp = (nBlockPostfix *) (pc + bh->size);
    if (bp->magic != (int)N_POSTFIX_MAGIC_COOKIE) {
        n_error("Free(): END OF MEM BLOCK CORRUPTED: src=%s, line=%d\n", bh->src_name, bh->src_line);
    }

    // Mem-Status-Variableb
    n_memused -= bh->size;
    n_memnumalloc--;

    // Src-Info zuruecksetzen
    strcpy(bh->src_name,"<freed>");
    bh->src_line = 0;

    if (((int)bh->next) == -1L) {
        // Block wurde als einzelner Chunk allokiert
        nChunkHeader *ch = (nChunkHeader *) (((char *)bh) - sizeof(nChunkHeader));
        bh->size = 0;
        this->FreeChunk(ch);
    } else {
        // Block wurde abgehandelt
        int i = (int) bh->next;
        nBlockHeader *next = blocks[i];
        blocks[i] = bh;
        bh->next = next;
        bh->size = 0;
    }
    mutex.Unlock();
}

//-------------------------------------------------------------------
//  n_malloc()
//  27-Feb-99   floh    created
//  27-May-99   floh    zusaetzliche Debugging-Parameter
//  04-Jun-99   floh    GCC-Warnings...
//-------------------------------------------------------------------
void *nn_malloc(size_t size, const char *file, int line)
{
    return mem.Malloc(size,file,line);
}

//-------------------------------------------------------------------
//  n_free()
//  27-Feb-99   floh    created
//-------------------------------------------------------------------
void n_free(void *p)
{
    mem.Free(p);
}

//-------------------------------------------------------------------
//  n_calloc()
//  27-Feb-99   floh    created
//-------------------------------------------------------------------
void *nn_calloc(size_t num, size_t size, const char *file, int line)
{
    size_t all_size = num*size;
    void *p = mem.Malloc(all_size,file,line);
    if (p) memset(p,0,sizeof(all_size));
    return p;
}

//-------------------------------------------------------------------
#elif __WIN32__
//-------------------------------------------------------------------
#ifdef __XBxX__
#include "xbox/nxbwrapper.h"
#else
#include <windows.h>
#endif

void *nn_malloc(size_t size, const char *file, int line)
{
    HGLOBAL ptr = GlobalAlloc(0, size);
    n_assert(ptr);
    return ptr;
}

void n_free(void *p)
{
    GlobalFree((HGLOBAL) p);
}

void *nn_calloc(size_t num, size_t size, const char *, int)
{
    HGLOBAL ptr = GlobalAlloc(GMEM_ZEROINIT, size*num);
    n_assert(ptr);
    return ptr;
}

//-------------------------------------------------------------------
#else
//-------------------------------------------------------------------
void *nn_malloc(size_t size, const char *, int)
{
    return malloc(size);
}

void n_free(void *p)
{
    free(p);
}

void *nn_calloc(size_t num, size_t size, const char *, int)
{
    return calloc(num,size);
}

//-------------------------------------------------------------------
#endif

//-------------------------------------------------------------------
//  EOF
//-------------------------------------------------------------------
