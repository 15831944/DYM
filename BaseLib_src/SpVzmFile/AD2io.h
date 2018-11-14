// AD2io.h
/*----------------------------------------------------------------------+
|		AD2io											 		        |
|       Author:     DuanYanSong  2008/12/06								|
|            Ver 1.0													|
|       Copyright (c) 2008, Supresoft Corporation                       |
|	         All rights reserved.                                       |
|       http://www.supresoft.com.cn                                     |
|		ysduan@supresoft.com.cn;ysduan@sohu.com                         |
+----------------------------------------------------------------------*/
#ifndef AD2IO_H_DUANYANSONG_2008_12_06_15_22_2432543
#define AD2IO_H_DUANYANSONG_2008_12_06_15_22_2432543

///////////////////////////////////////////////////////////////
// For AdIO
///////////////////////////////////////////////////////////////
#include <io.h>
#include <fcntl.h>
                
#define READFLAGS  (O_RDONLY | O_BINARY)
#define WRITEFLAGS (O_WRONLY | O_BINARY)
#define MAXODIOBLOCKS 16
                
/* this version uses the critical error handlers in the
example programs */
extern short criterrhandler(short num);

#ifndef ODIOFILESTRUDEFINED
struct odioblockstru {
    char *buf;        /* this buffer */
    long  startaddr;  /* address from which it came in the file */
    short validbytes; /* number of valid bytes it holds */
    long  lru;        /* least recently used counter */
};

typedef struct ODIO_FILESTRU {
    long physfilepos;        /* where the file pointer is */
    long bufpos;             /* position from which buf was filled */
    int handle;              /* file handle */
    int lvl;                 /* bytes left in buf */
    unsigned short bufbytes; /* valid bytes read into buffer */
    char *nexchptr;          /* pointer to next char in buffer */
    char *cbuf;              /* pointer to the buffer currently being used */
    short eofflag;           /* 1 if filbuf fails */
    short usingblock;        /* which block is currently in use */
    struct odioblockstru datablock[MAXODIOBLOCKS];  /* the data being held */
} ODIO_FILE;
#define ODIOFILESTRUDEFINED
#endif


/* odio functions */
/* write functions */
int   odiow_fclose(void *f){ return(fclose((FILE *)f)); }
void* odiow_fopen(void *path,char *accessmodes){ FILE *tfile; if ((tfile=fopen( (char*)path,accessmodes))==NULL) return(NULL); setvbuf(tfile,NULL,_IOFBF,8192); return(tfile); }
int   odiow_fputs(const char *str,void *f){ return(fputs(str,(FILE *)f)); }
int   odiow_fread (void *buf, unsigned int size, unsigned int num, void *f){ return(fread(buf,size,num,(FILE *)f)); }
int   odiow_fseek (void *f, long offset, int whence){ return(fseek((FILE *)f,offset,whence)); }
long  odiow_ftell(void *f){ return(ftell((FILE *)f)); }
int   odiow_fwrite(void *b,int size,int count, void *f){ return(fwrite(b,size,count,(FILE *)f)); }

/* odio functions */
/* read functions */
short odioreadbufs     = min(8,MAXODIOBLOCKS);/* number of read buffers */
short odioblocksize    = 8192;                /* size of each read buffer */
long  odiopositionmask = ~(8192-1);           /* mask to allow position check */
                                              /* a full implementation should probably reset odiolru to 0L every time adInitAd2() is called. */
long  odiolru=0L;                             /* counter to track use of blocks */

void odior_filbuf _((ODIO_FILE *f));
unsigned char odior_fgetc _((void *f));
void* odior_fopen _((void *path));
int   odior_fclose _((void *f));
int   odior_fseek _((void *f, long offset, int whence));
int   odior_fread _((void *buf, unsigned int size, unsigned int num, void *f));
char* odior_fgetdxfline _((char *s, unsigned int n, void *f));
long  odior_ftell _((void *f));

#define odior_getc(f) \
    ((f)->datablock[(f)->usingblock].lru=odiolru++, \
    (--((f)->lvl) >= 0) ? (*(f)->nexchptr++) : \
odior_fgetc(f))

long odior_ftell(void *f2)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    return(f->bufpos + (f->nexchptr - f->cbuf));
}

unsigned char odior_fgetc(void *f2)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    long origbufpos;    
    f->datablock[f->usingblock].lru=odiolru++;
    if ((f->lvl)<=0) {
        origbufpos=f->bufpos;
        f->bufpos+=f->bufbytes;
        odior_filbuf(f);
        if (f->bufpos==origbufpos) {
            f->eofflag=1;
            return(0);
        }
    }
    f->lvl--;
    return(*f->nexchptr++);
}

short odior_feof(void *f){ return(((ODIO_FILE *)f)->eofflag); }
int odior_ungetc(int ch,void *f2){ ODIO_FILE *f=(ODIO_FILE *)f2; f->nexchptr--; f->lvl++; f->eofflag=0; *(f)->nexchptr = ch; return(1); }

#define OD_CR 13
#define OD_LF 10

char *odior_fgetdxfline(char *s, unsigned int n, void *f2)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    char lastchar=0,onemorech;
    
    if (!n || odior_feof(f)) return(NULL);
    f->datablock[f->usingblock].lru=odiolru++;
    while (n && lastchar!=OD_CR && lastchar!=OD_LF && !odior_feof(f)) {
        *s++ = lastchar = (--((f)->lvl) >= 0) ? (*(f)->nexchptr++) : odior_fgetc(f);
        n--;
    }
    if (odior_feof(f) || n==0) s--;
    *s=0;
    
    if (lastchar==OD_CR && !odior_feof(f)) {
        onemorech=odior_getc(f);
        if (odior_feof(f)) f->eofflag=0;  /* not really eof yet, next one is */
        else if (onemorech!=OD_LF) odior_ungetc(onemorech,f);
    }else if (lastchar==OD_LF && !odior_feof(f)) {
        onemorech=odior_getc(f);
        if (odior_feof(f)) f->eofflag=0;  /* not really eof yet, next one is */
        else if (onemorech!=OD_CR) odior_ungetc(onemorech,f);
    }
    return(s);
}

int odior_fseek(void *f2, long offset, int whence)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    unsigned short bytestoadvance;
    
    if (whence==1) { offset+=(f->bufpos+(f->nexchptr - f->cbuf)); }
    
    f->eofflag=0;
    /* if it's not in the area we're holding, seek to it */
    if (offset < f->bufpos || offset >= f->bufpos + (long)f->bufbytes) {
        f->bufpos=offset & odiopositionmask;
        odior_filbuf(f);  /* locates it if we're already holding in another block */
    }
    f->nexchptr=(char *)(f->cbuf + (bytestoadvance=(unsigned short)(offset - f->bufpos)));
    f->lvl = f->bufbytes - bytestoadvance;
    
    return(0);
}

/* this assumes we will always find enough data to satisfy the request */
int odior_fread(void *bufvoid, unsigned int size, unsigned int num, void *f2)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    long bytesleft;
    unsigned short bytestoread;
    unsigned char *buf=(unsigned char *)bufvoid;
    
    f->datablock[f->usingblock].lru=odiolru++;
    bytesleft=(long)size*(long)num;
    
    while (bytesleft > 0L && !odior_feof(f)) {
        if (f->lvl <= 0) {
            f->bufpos+=f->bufbytes;
            odior_filbuf(f);  /* get something in here */
        }
        
        if ((long)f->lvl<bytesleft) bytestoread=(unsigned short)f->lvl;
        else bytestoread=(unsigned short)bytesleft;
        
        memcpy(buf,f->nexchptr,bytestoread);
        f->lvl -= bytestoread;
        f->nexchptr += bytestoread;
        buf += bytestoread;
        bytesleft -= bytestoread;
    }
    return(num);
}

void *odior_fopen(void *path)
{
    ODIO_FILE *f; short i;
    if ((f=(ODIO_FILE *)odmem_malloc(sizeof(ODIO_FILE)))==NULL) return(NULL);
    if ((f->handle=open( (char*)path,READFLAGS) )==-1) { odmem_free(f); return(NULL); }
    f->eofflag=f->bufbytes=f->lvl=0;
    f->bufpos=0L;  /* to start reads from 0L */
    f->cbuf=NULL;
    f->nexchptr=(char *)f->cbuf;
    f->usingblock = -1;
    f->physfilepos=0L;
    
    for (i=0; i<odioreadbufs; i++)
        f->datablock[i].buf=NULL;
    for (i=0; i<odioreadbufs; i++) {
        if ((f->datablock[i].buf=(char *)odmem_malloc(odioblocksize))==NULL) {
            odior_fclose(f);
            criterrhandler(AD_CRITERR_MALLOCERROR);
        }
        f->datablock[i].validbytes=0;
        f->datablock[i].lru = -1L;
        f->datablock[i].startaddr = -1L;
    }
    odior_fseek(f,0L,0);  /* initial seek, gets a buffer & stuff */
    if (f->eofflag) {   /* file is empty */
        odior_fclose(f);
        return(NULL);
    }
    return(f);
}

int odior_fclose(void *f2)
{
    ODIO_FILE *f=(ODIO_FILE *)f2;
    short retval,i; 
    for (i=0; i<odioreadbufs; i++) {
        if (f->datablock[i].buf!=NULL) odmem_free(f->datablock[i].buf);
        f->datablock[i].lru = -1L;
        f->datablock[i].validbytes=0;
        f->datablock[i].startaddr = -1L;
    }
    retval=close(f->handle);
    odmem_free(f);
    return(retval);
}

long odior_flength(void *f){ return(_filelength(((ODIO_FILE *)f)->handle));  }

void odior_filbuf(ODIO_FILE *f)
{
    short i,minindex;
    long minlru;
    struct odioblockstru *minptr;
    char readerror;
    
    f->usingblock = -1;
    
    for (i=0; i<odioreadbufs; i++) {
        if (f->datablock[i].startaddr==f->bufpos)
            break;
    }
    
    if (i<odioreadbufs) {   /* we are already holding this part of the file */
        f->cbuf=f->datablock[i].buf;
        f->bufpos=f->datablock[i].startaddr;
        f->lvl=f->bufbytes=f->datablock[i].validbytes;
        f->nexchptr=(char *)f->cbuf;
        f->datablock[i].lru=odiolru++;
        f->usingblock=i;
        return;
    }
    
    minptr=NULL; minindex=0;
    
    for (i=0; i<odioreadbufs; i++) {
        if (f->datablock[i].startaddr==-1L) {
            minindex=i;
            minptr=&f->datablock[i];
            break;
        }
    }
    
    /* if all were used, then look for the least recently used one */
    if (minptr==NULL) {
        minlru=0x7FFFFFFF;
        minptr=NULL;
        minindex=0;
        
        for (i=0; i<odioreadbufs; i++) {
            if (f->datablock[i].lru<0L) f->datablock[i].lru=0L;
            if (f->datablock[i].lru<minlru) {
                minlru=f->datablock[i].lru;
                minptr=&f->datablock[i];
                minindex=i;
            }
        }
    }
    
    if (minptr==NULL) return;  /* couldn't find one */
    /* if we are not already physically at the read location, move there */
    /* then read into the buffer */
    
    readerror=0;
    do {
        if (f->physfilepos!=f->bufpos || readerror) {
            lseek(f->handle,f->bufpos,0);
        }
        readerror=0;
        minptr->validbytes=read(f->handle,minptr->buf,odioblocksize);
        /* check for error */
        if (minptr->validbytes==-1) readerror=1;
        if (readerror) criterrhandler(AD_CRITERR_FILEREADERROR);
    } while (readerror);
    f->physfilepos=f->bufpos+minptr->validbytes;
    
    minptr->startaddr=f->bufpos;
    minptr->lru=odiolru++;
    f->bufbytes = f->lvl = minptr->validbytes;
    f->cbuf=minptr->buf;
    if (f->lvl==0) f->eofflag=1;
    else f->eofflag=0;
    f->nexchptr=(char *)f->cbuf;
    f->usingblock=minindex;
}

/* odio functions */
/* memory functions */
void *odmem_malloc(int size){ return(malloc(size)); }
void  odmem_free(void *loc){ free(loc); }
short odvm_init(void){ return(1); }
short odvm_term (void){ return(1); }
short odvm_readbytes(char *ptr,AD_VMADDR vmloc,unsigned short bytes){ memcpy(ptr,vmloc,bytes); return(1); }
short odvm_writebytes(AD_VMADDR vmloc,char *ptr,unsigned short bytes){ memcpy(vmloc,ptr,bytes); return(1); }
short odvm_free(AD_VMADDR ptr){ free(ptr); return(1); }
AD_VMADDR odvm_malloc(unsigned num_bytes){ return((AD_VMADDR)malloc(num_bytes));  }


#endif
