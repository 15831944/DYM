#include "stdafx.h"
#include "ReadLine.h"

#include "AutoPtr.hpp"
const char *ReadLine( FILE * fp)
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


const char* newGUID()
{
	static char buf[VERSION_MAX] = {0};

	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf_s(buf, VERSION_MAX
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	return (const char*)buf;
}

 GUID  TranStrToGUID(const char *pstrGUID)
{
	GUID guidVersion = GUID_NULL;
	wchar_t wstr[VERSION_MAX];
	
	int nWSize = 0;
	mbstowcs_s(NULL, wstr, pstrGUID, strlen(pstrGUID));

	CLSIDFromString(wstr , (LPCLSID)&guidVersion);

	return guidVersion;
}