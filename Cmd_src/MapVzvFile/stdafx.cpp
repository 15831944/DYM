// stdafx.cpp : 只包括标准包含文件的源文件
// MapVzvFile.pch 将作为预编译头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

const char *ReadLine( FILE * fp)
{
    static char *pszRLBuffer = NULL;
    static int  nRLBufferSize = 0;
    int         nLength, nReadSoFar = 0, nStripped = 0, i;
	
	/* -------------------------------------------------------------------- */
	/*      Cleanup case.                                                   */
	/* -------------------------------------------------------------------- */
    if( fp == NULL )
    {
        free( pszRLBuffer );
        pszRLBuffer = NULL;
        nRLBufferSize = 0;
        return NULL;
    }
	
	/* -------------------------------------------------------------------- */
	/*      Loop reading chunks of the line till we get to the end of       */
	/*      the line.                                                       */
	/* -------------------------------------------------------------------- */
    do {
		/* -------------------------------------------------------------------- */
		/*      Grow the working buffer if we have it nearly full.  Fail out    */
		/*      of read line if we can't reallocate it big enough (for          */
		/*      instance for a _very large_ file with no newlines).             */
		/* -------------------------------------------------------------------- */
        if( nRLBufferSize-nReadSoFar < 128 )
        {
            nRLBufferSize = nRLBufferSize*2 + 128;
            pszRLBuffer = (char *) realloc(pszRLBuffer, nRLBufferSize);
            if( pszRLBuffer == NULL )
            {
                nRLBufferSize = 0;
                return NULL;
            }
        }
		
		/* -------------------------------------------------------------------- */
		/*      Do the actual read.                                             */
		/* -------------------------------------------------------------------- */
        if( fgets( pszRLBuffer+nReadSoFar, nRLBufferSize-nReadSoFar, fp )
            == NULL )
        {
            free( pszRLBuffer );
            pszRLBuffer = NULL;
            nRLBufferSize = 0;
			
            return NULL;
        }
		
        nReadSoFar = strlen(pszRLBuffer);
		
    } while( nReadSoFar == nRLBufferSize - 1
		&& pszRLBuffer[nRLBufferSize-2] != 13
		&& pszRLBuffer[nRLBufferSize-2] != 10 );
	
	/* -------------------------------------------------------------------- */
	/*      Clear CR and LF off the end.                                    */
	/* -------------------------------------------------------------------- */
    nLength = strlen(pszRLBuffer);
    if( nLength > 0
        && (pszRLBuffer[nLength-1] == 10 || pszRLBuffer[nLength-1] == 13) )
    {
        pszRLBuffer[--nLength] = '\0';
        nStripped++;
    }
    
    if( nLength > 0
        && (pszRLBuffer[nLength-1] == 10 || pszRLBuffer[nLength-1] == 13) )
    {
        pszRLBuffer[--nLength] = '\0';
        nStripped++;
    }
	
	/* -------------------------------------------------------------------- */
	/*      Check that there aren't any extra CR or LF characters           */
	/*      embedded in what is left.  I have encountered files with        */
	/*      embedded CR (13) characters that should have acted as line      */
	/*      terminators but got sucked up by VSIFGetc().                    */
	/* -------------------------------------------------------------------- */
    for( i = 0; i < nLength; i++ )
    {
        if( pszRLBuffer[i] == 10 || pszRLBuffer[i] == 13 )
        {
		/* we need to chop off the buffer here, and seek the input back
		to after the character that should have been the line
			terminator. */
            fseek( fp, (i+1) - (nLength+nStripped), SEEK_CUR );
            pszRLBuffer[i] = '\0';
        }
    }
	
    return( pszRLBuffer );
}
