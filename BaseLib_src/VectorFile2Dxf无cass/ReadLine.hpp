// ReadLine.hpp: main  file for the ReadLine
//
/*----------------------------------------------------------------------+
|	ReadLine.hpp												|
|	Author: huangyang 2013/05/23										|
|		Ver 1.0 														|
|	Copyright (c) 2013, Supresoft Corporation							|
|		All rights reserved huangyang.									|
|	http://www.supresoft.com.cn											|
|	eMail:huangyang@supresoft.com.cn									|
+----------------------------------------------------------------------*/

#ifndef READLINE_HPP_HUANYYANG_2013_05_23_13_33_96413
#define READLINE_HPP_HUANYYANG_2013_05_23_13_33_96413

#pragma once
#include "AutoPtr.hpp"

#include <Windows.h>
static const char *ReadLine( FILE * fp)
{
	static CGrowSelfAryPtr<char> sBuffer;
    static char *pszRLBuffer = NULL;
    static int  nRLBufferSize = 0;
    int         nLength, nReadSoFar = 0, nStripped = 0, i;
	
	/* -------------------------------------------------------------------- */
	/*      Cleanup case.                                                   */
	/* -------------------------------------------------------------------- */
    if( fp == NULL )
    {
		sBuffer.RemoveAll();
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
			sBuffer.SetSize(nRLBufferSize);
            pszRLBuffer = sBuffer.Get();
            if( pszRLBuffer == NULL )
            {
                nRLBufferSize = 0;
                return NULL;
            }
        }
		
		/* -------------------------------------------------------------------- */
		/*      Do the actual read.                                             */
		/* -------------------------------------------------------------------- */
        if( NULL == fgets(pszRLBuffer+nReadSoFar, nRLBufferSize-nReadSoFar, fp) )
        {
			sBuffer.RemoveAll();
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

#endif //READLINE_HPP_HUANYYANG_2013_05_23_13_33_96413