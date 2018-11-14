/*----------------------------------------------------------------------+
|		Import4DWG											 	        |
|       Author:     DuanYanSong  2008/12/01								|
|            Ver 1.0													|
|       Copyright (c) 2008, WHU RSGIS DPGrid R&D Group.                 |
|	         All rights reserved.                                       |
|		ysduan@sohu.com                                                 |
+----------------------------------------------------------------------*/

#include "stdafx.h"
#include "math.h" 
#include "stdio.h" 
#include "stdlib.h" 
#include "conio.h" 
#include "resource.h"

#include "SpVzmFile.h"
#include "ImportFileDef.h"

#include "OcsWcs.hpp"
#include "Ad2.h"
#include "AD2io.h"

///////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////
static char gs_strMsg[32];
static int  gs_cancel;
static CSpVzmFile *gs_this=NULL;

static void loadMeter( short percent )
{ 
    static short lastV=-1; 
    if ( lastV!=percent ){
        sprintf_s(gs_strMsg,"Load DWG/DXF %d %%%",percent );
        gs_this->PrintMsg( gs_strMsg );
        gs_this->ProgStep( gs_cancel );
        lastV = percent;
    }
}

static short criterrhandler(short num)
{  
    if ( num==AD_CRITERR_FILEREADERROR || num==AD_CRITERR_DISKFULL        || num==AD_CRITERR_WRITEABORT ||
         num==AD_CRITERR_MALLOCERROR   || num==AD_CRITERR_BUFFEROVERRUN   || num==AD_CRITERR_BADDATA  )
    {
        char strMsg[128]; sprintf_s( strMsg,"Open DWG/DXF file error . \nError = %d ",num );
		::MessageBox( GetFocus(),strMsg,"Error",MB_OK );
    }
    return 0;
}

static PAD_DWGHDR   adhd;
static PAD_ENT_HDR  adenhd;
static PAD_ENT      aden;
static PAD_TB       adtb;
static PAD_XD       adxd;
static short allocateadptrs(void)
{
    if ((adhd=(PAD_DWGHDR)malloc(sizeof(AD_DWGHDR)))!=NULL) {
        if ((adenhd=(PAD_ENT_HDR)malloc(sizeof(AD_ENT_HDR)))!=NULL) {
            if ((aden=(PAD_ENT)malloc(sizeof(AD_ENT)))!=NULL) {
                if ((adtb=(PAD_TB)malloc(sizeof(AD_TB)))!=NULL) {
                    if ((adxd=(PAD_XD)malloc(sizeof(AD_XD)))!=NULL) {
                        return(1);
                    }
                    free(adtb);
                }
                free(aden);
            }
            free(adenhd);
        }
        free(adhd);
    }
    return(0);
}

static void freeadptrs(void){ free(adxd);  free(adtb); free(aden); free(adenhd); free(adhd); }

static HMODULE ModuleFromAddress(PVOID pv) 
{
	MEMORY_BASIC_INFORMATION mbi;
	return ( (::VirtualQuery(pv,&mbi,sizeof(mbi))!=0 )?(HMODULE)mbi.AllocationBase:NULL );
}

static BOOL GetEnt4Blk(CSpVzmFile *pDpv,AD_DB_HANDLE dwghandle,AD_OBJHANDLE blkHandle,
                       double &minX,double &minY,double &maxX,double &maxY,BOOL bBlk=FALSE,
                       double offX=0,double offY=0,double offZ=0,
                       double scaleX=1,double scaleY=1,double scaleZ=1,
                       double rotangle=0 )
{
    //static int recursiveTime=0; _cprintf( ">>>>>> RecursiveTime= %d \n", recursiveTime++ );
    class CSpEntityIns : public CSpVzmEntity{
    public:
        BOOL  AppendVtx(double x,double y,double z,int cd=20){ if (m_pO2W) m_o2w.CvtO2W( x,y,z,&x,&y,&z ); AppendPt(x,y,z,cd); return TRUE; };
        BOOL  AppendCircle(double cx,double cy,double cz,double rad){
                if (m_pO2W) m_o2w.CvtO2W( cx,cy,cz,&cx,&cy,&cz );
                for ( double a=0;a<6.2;a+=0.15 ) AppendPt( cx+cos(a)*rad,cy+sin(a)*rad,cz );
                SetClosed(TRUE); return TRUE;
            };
        BOOL  AppendArc( double cx,double cy,double cz,double rad,double sA,double eA ){
                if (m_pO2W) m_o2w.CvtO2W( cx,cy,cz,&cx,&cy,&cz );
                eA += (eA<sA?6.28:0);
                for ( double a=sA;a<eA;a+=0.15 ) AppendPt( cx+cos(a)*rad,cy+sin(a)*rad,cz );
                AppendPt( cx+cos(eA)*rad,cy+sin(eA)*rad,cz ); return TRUE;
            };
        BOOL AppendElps( double cx,double cy,double cz,double *off,double e){
                if (m_pO2W) m_o2w.CvtO2W( cx,cy,cz,&cx,&cy,&cz );
                double sx=off[1]*e,sy=off[0]*e;
                AppendPt( cx-off[0],cy-off[1],cz ); 
                AppendPt( cx-sx    ,cy+sy    ,cz ); 
                AppendPt( cx+off[0],cy+off[1],cz ); 
                AppendPt( cx+sx    ,cy-sy    ,cz );                 
                SetClosed(TRUE); return TRUE;
            };
        void  SetO2W( double *pO2W=NULL ){ m_pO2W=pO2W; if (m_pO2W) m_o2w.SetPar(m_pO2W); };
		void SetClosed(BOOL bClosed=TRUE) {
			WORD state=GetEntState();
			if(bClosed) SetEntState(state|ST_OBJ_CLOSE);
			else SetEntState(state&(~ST_OBJ_CLOSE)); };

        COcs2Wcs m_o2w;
        double*  m_pO2W;
    }actEnt;

    short returnval; AD_ENT_HDR vhdr; int i,cancel; 
    double baseX=0,baseY=0,baseZ=0;VCTENTTXT entTxt; ENTEXT entExt;
    AD_VMADDR entlist = adEntityList( dwghandle,blkHandle );
    adStartEntityGet( entlist );cancel=0; if ( !bBlk ) pDpv->ProgBegin( adNumEntities(dwghandle,blkHandle)/256 );
    do{
        if (!(returnval=adGetEntity(entlist,adenhd,aden))) continue;
        if ( !bBlk ){ if ((cancel++%256)==0) pDpv->ProgStep(cancel); }

        actEnt.Init( 0,0 ); actEnt.SetO2W(NULL);
        if ( adenhd->extrusion[0]!=0 || adenhd->extrusion[1]!=0 || adenhd->extrusion[2]!=1 ) 
            actEnt.SetO2W(adenhd->extrusion);
        if ( bBlk ){            
            actEnt.SetO2W(adenhd->extrusion);
            actEnt.m_o2w.SetBase(baseX,baseY,baseZ);
            actEnt.m_o2w.SetScale(scaleX,scaleY,scaleZ);
            actEnt.m_o2w.SetOffset(offX,offY,offZ);
        }

        switch ( adenhd->enttype ) 
        {
            case AD_ENT_POINT: /* point */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendVtx( aden->point.pt0[0],aden->point.pt0[1],aden->point.pt0[2] );
                break;
            case AD_ENT_RAY   :
            case AD_ENT_XLINE :
            case AD_ENT_LINE3D: /* 3dline */
            case AD_ENT_LINE  : /* line   */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendVtx( aden->line.pt0[0],aden->line.pt0[1],aden->line.pt0[2] );
                actEnt.AppendVtx( aden->line.pt1[0],aden->line.pt1[1],aden->line.pt1[2] );
                break;
            case AD_ENT_CIRCLE:  /* circle */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendCircle( aden->circle.pt0[0],aden->circle.pt0[1],aden->circle.pt0[2],aden->circle.radius );
                
                memset( &entExt,0,sizeof(entExt) );
                entExt.extType = extDXFEX; entExt.extDxf.dxfEnt = ENT_CIRCLE;
                memcpy( &(entExt.extDxf.circle),&(aden->circle),sizeof(aden->circle) );

                if (actEnt.m_pO2W){
                    double *pXyz = entExt.extDxf.circle.pt0;
                    actEnt.m_o2w.CvtO2W( pXyz[0],pXyz[1],pXyz[2],pXyz,pXyz+1,pXyz+2 );
                }
                actEnt.SetExt( sizeof(entExt),(BYTE*)&entExt );
                break;
            case AD_ENT_SHAPE: /* shape */ /* missing name, obl angle and rel x scale */
                break;
            case AD_ENT_MTEXT:
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendVtx( aden->mtext.pt0[0],aden->mtext.pt0[1],aden->mtext.pt0[2] );
                
                memset( &entTxt,0,sizeof(entTxt) );
                strcpy_s( entTxt.strTxt,aden->mtext.textstr );
                entTxt.wid   = float( aden->mtext.boxwid );
                entTxt.hei   = float( aden->mtext.boxht  );

                actEnt.SetTxtPar( entTxt );
                break;
            case AD_ENT_TEXT: /* text */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendVtx( aden->text.pt0[0],aden->text.pt0[1],aden->text.pt0[2] );
                
                memset( &entTxt,0,sizeof(entTxt) );
                strcpy_s( entTxt.strTxt,aden->text.textstr );
                entTxt.angle = float( aden->text.tdata.rotang );
                entTxt.wid   = float( aden->text.tdata.widthfactor );
                entTxt.hei   = float( aden->text.tdata.height );
                
                actEnt.SetTxtPar( entTxt );
                break;
            case AD_ENT_ARC: /* arc */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendArc( aden->arc.pt0[0],aden->arc.pt0[1],aden->arc.pt0[2],aden->arc.radius,aden->arc.stang,aden->arc.endang );
                
                memset( &entExt,0,sizeof(entExt) );
                entExt.extType = extDXFEX; entExt.extDxf.dxfEnt = ENT_ARC;
                memcpy( &(entExt.extDxf.arc),&(aden->arc),sizeof(aden->arc) );
                if (actEnt.m_pO2W){
                    double *pXyz = entExt.extDxf.arc.pt0;
                    actEnt.m_o2w.CvtO2W( pXyz[0],pXyz[1],pXyz[2],pXyz,pXyz+1,pXyz+2 );
                }
                actEnt.SetExt( sizeof(entExt),(BYTE*)&entExt );

                break;
            case AD_ENT_BLOCK: /* block */
                baseX = aden->block.base[0];
                baseY = aden->block.base[1];
                baseZ = aden->block.base[2];
                break;
            case AD_ENT_ENDBLK: /* endblk */
                break;
            case AD_ENT_INSERT: /* insert */
                adSeekBlockheader( dwghandle,aden->insert.blockheaderobjhandle,&(adtb->blkh) );
                if (!adtb->blkh.purgedflag)
                {
                    if ( actEnt.m_pO2W ) 
                        actEnt.m_o2w.CvtO2W( aden->insert.pt0[0],aden->insert.pt0[1],aden->insert.pt0[2],
                                             aden->insert.pt0,aden->insert.pt0+1,aden->insert.pt0+2 );

                    GetEnt4Blk( pDpv,dwghandle,adtb->blkh.objhandle,minX,minY,maxX,maxY,TRUE,
                                aden->insert.pt0[0],aden->insert.pt0[1],aden->insert.pt0[2],
                                aden->insert.xscale,aden->insert.yscale,aden->insert.zscale,aden->insert.rotang );
                }
                break;
            case AD_ENT_ATTDEF: /* attdef */
                break;
            case AD_ENT_ATTRIB: /* attrib */
                break;
            case AD_ENT_SEQEND: /* seqend */
                break;
            case AD_ENT_POLYLINE: /* polyline */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                vhdr.enttype=AD_ENT_POLYLINE;
                while ( vhdr.enttype!=AD_ENT_SEQEND )
                {
                    if ( adGetEntity( entlist,&vhdr,aden )==1 ) 
                    {
                        if ( vhdr.enttype==AD_ENT_VERTEX && aden->vertex.vertexflag )
                            actEnt.AppendVtx( aden->vertex.pt0[0],aden->vertex.pt0[1],aden->vertex.pt0[2] );
                    }else
                        break;
                }
                if ( (aden->pline.polyflag&AD_PLINE_CLOSED)==AD_PLINE_CLOSED ) actEnt.SetClosed();
                break;
            case AD_ENT_VERTEX: /* vertex */
                break;
            case AD_ENT_TRACE:  /* trace */
            case AD_ENT_SOLID:  /* solid */
            case AD_ENT_FACE3D: /* 3dface */
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendVtx( aden->face3d.pt0[0],aden->face3d.pt0[1],aden->face3d.pt0[2] );
                actEnt.AppendVtx( aden->face3d.pt1[0],aden->face3d.pt1[1],aden->face3d.pt1[2] );
                actEnt.AppendVtx( aden->face3d.pt2[0],aden->face3d.pt2[1],aden->face3d.pt2[2] );
                actEnt.AppendVtx( aden->face3d.pt3[0],aden->face3d.pt3[1],aden->face3d.pt3[2] );
                break;
            case AD_ENT_DIMENSION: /* associative dimension */
                break;
            case AD_ENT_VIEWPORT:
                break;
            case AD_ENT_ELLIPSE:
                actEnt.Init( 0,BYTE(adenhd->entcolor) );
                actEnt.AppendElps( aden->ellipse.pt0[0],aden->ellipse.pt0[1],aden->ellipse.pt0[2],aden->ellipse.pt1offset,aden->ellipse.minortomajorratio );

                memset( &entExt,0,sizeof(entExt) );
                entExt.extType = extDXFEX; entExt.extDxf.dxfEnt = ENT_ELLIPSE;
                memcpy( &(entExt.extDxf.ellipse),&(aden->ellipse),sizeof(aden->ellipse) );
                if (actEnt.m_pO2W){
                    double *pXyz = entExt.extDxf.ellipse.pt0;
                    actEnt.m_o2w.CvtO2W( pXyz[0],pXyz[1],pXyz[2],pXyz,pXyz+1,pXyz+2 );
                }
                actEnt.SetExt( sizeof(entExt),(BYTE*)&entExt );

                break;
            case AD_ENT_SPLINE:
                {
                    actEnt.Init( 0,BYTE(adenhd->entcolor) );

                    double fitpt[3],ctlpt[3],knot,weight;
                    PAD_BLOB_CTRL bcptr =adStartBlobRead( aden->spline.ldblob );
                    for ( i=0; i<aden->spline.numknots ; i++ ){ adReadBlobDouble(bcptr,&knot); }
                    for ( i=0; i<aden->spline.numctlpts; i++ ) 
                    { 
                        adReadBlob3Double(bcptr,ctlpt);
                        if (aden->spline.flag & 4) adReadBlobDouble(bcptr,&weight); // rational, has weights 
                    }
                    for ( i=0; i<aden->spline.numfitpts;i++ ) 
                    {
                        adReadBlob3Double( bcptr,fitpt );
                        actEnt.AppendVtx( fitpt[0],fitpt[1],fitpt[2] );                                
                    }
                    adEndBlobRead(bcptr);

                    if ( (aden->spline.flag&AD_SPLINE_CLOSED)==AD_SPLINE_CLOSED ) actEnt.SetClosed();
                }
                break;
            case AD_ENT_REGION:
            case AD_ENT_SOLID3D:
            case AD_ENT_BODY:
                break;       /* no non-longdata to print */
            case AD_ENT_LEADER:
                break;
            case AD_ENT_TOLERANCE:
                break;
            case AD_ENT_MLINE:
                break;
            default: /* proxies */
                if ( adenhd->enttype==adLwplineEnttype(dwghandle) ) 
                {
                    actEnt.Init( 0,BYTE(adenhd->entcolor) );
                    
                    double tempdouble[2],tempbulge,tempwidth[2],bulge; int cd=penLINE;
                    PAD_BLOB_CTRL bcptr=adStartBlobRead( aden->lwpline.ldblob );
                    for ( int il=0; il<aden->lwpline.numpoints; il++ ) 
                    {
                        adReadBlob2Double( bcptr,tempdouble );

                        if ( aden->lwpline.flag & AD_LWPLINE_HAS_BULGES ){ 
                            adReadBlobDouble ( bcptr,&tempbulge ); 
                            if (tempbulge!=0){ bulge=tempbulge;  cd = penCURVE; }
                        }
                        if ( aden->lwpline.flag & AD_LWPLINE_HAS_WIDTHS ){
                            adReadBlob2Double( bcptr,tempwidth  ); 
                        }
                        actEnt.AppendVtx( tempdouble[0],tempdouble[1],aden->lwpline.elevation,cd );    
                    }
                    adEndBlobRead(bcptr);
            
                    memset( &entExt,0,sizeof(entExt) );
                    entExt.extType = extDXFEX; entExt.extDxf.dxfEnt = ENT_LWPLINE;
                    memcpy( &(entExt.extDxf.lwpline),&(aden->lwpline),sizeof(aden->lwpline) );
                    entExt.extDxf.lwpline.bulge = bulge;
                    actEnt.SetExt( sizeof(entExt),(BYTE*)&entExt );
                    
                    
                    if ( aden->lwpline.flag&AD_LWPLINE_IS_CLOSED ) actEnt.SetClosed();                                    
                }else
                if (adenhd->enttype==adOle2frameEnttype(dwghandle)) 
                {

                }else
                if (adenhd->enttype==adHatchEnttype(dwghandle)) 
                {
          
                }else
                if (adenhd->enttype==adImageEnttype(dwghandle)) 
                {
          
                }else /* regular proxy */
                {
                    _cprintf("proxy entity:\n");
                    _cprintf("entclassid: %d\n"  ,aden->proxyent.entclassid );
                    _cprintf("appclassid: %d\n"  ,aden->proxyent.appclassid );
                    _cprintf("entdatabits: %ld\n",aden->proxyent.entdatabits);
                }
                break;
        }
	    if ( actEnt.GetCrdSum() )
	    {
            AD_LAY layer; memset( &layer,0,sizeof(layer) );
		    adSeekLayer( dwghandle,adenhd->entlayerobjhandle,&layer );
            
            actEnt.SetLayIdx( pDpv->QueryLayerIdx( layer.name,TRUE ) );
		    pDpv->AppendEnt( &actEnt );

            int xl=0,yb=0,xr=0,yt=0; actEnt.Get_Rgn( &xl,&yb,&xr,&yt ); 
            if ( minX>xl ) minX = xl; if ( maxX<xr ) maxX = xr; 
            if ( minY>yb ) minY = yb; if ( maxY<yt ) maxY = yt;                    
	    }                
    }while (returnval==1);
    if ( !bBlk ) pDpv->ProgEnd();

    //_cprintf( "RecursiveTime= %d  >>>>>>\n", recursiveTime-- );
    return TRUE;
}

BOOL CSpVzmFile::Import4DWG( LPCSTR lpstrPathName )
{
    AD_DB_HANDLE dwghandle; short initerror=0; 
    if ( !allocateadptrs() ){ AfxMessageBox("couldn't allocate data storage"); return FALSE; }
    
    char initfilepath[512]; ::GetTempPath( 512,initfilepath );
    strcat( initfilepath,"\\~DWGInit.dat"  );
    WIN32_FIND_DATA fd; HANDLE hFind = ::FindFirstFile(initfilepath,&fd);
	if ( hFind == INVALID_HANDLE_VALUE ){
        HMODULE hInstance = ModuleFromAddress(ModuleFromAddress);
        HRSRC	hRes	= ::FindResource( hInstance,MAKEINTRESOURCE(IDR_DWGINIDAT),"BIN" );
        DWORD	dwSize	= ::SizeofResource( hInstance, hRes );
        HGLOBAL hGlob	= ::LoadResource( hInstance, hRes );
		LPBYTE	pBytes	= (LPBYTE)LockResource(hGlob);
		HANDLE  hFile   = ::CreateFile( initfilepath,GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,0,NULL );
		if ( hFile != INVALID_HANDLE_VALUE ){ DWORD rw=0; ::WriteFile( hFile,pBytes,dwSize,&rw,NULL ); ::CloseHandle( hFile ); }
	}
    if ( !adInitAd2( initfilepath,0,&initerror) ){ AfxMessageBox( "Unable to initialize DWG/DXF file." ); freeadptrs(); return FALSE; }
    adSetAd2CriticalErrorFn( criterrhandler );

    adSetupDwgRead(); adSetupDxfRead();
    adSetAd2LoadometerFn( loadMeter ); gs_this=this;
    
    PrintMsg("Load/Scan File Data... \n"); ProgBegin(90);
    if ( (dwghandle=adLoadFile( (void*)lpstrPathName,AD_PRELOAD_ALL,1))==NULL ){
        char strMsg[256]; sprintf( strMsg,"Can not open file: %s \nError code: %d :%s",lpstrPathName,adError(),adErrorStr(adError()) );
        AfxMessageBox( strMsg );  adCloseAd2(); freeadptrs(); return FALSE; 
    }
    ProgEnd();
    
    PrintMsg("Get Section: Tables \n");
    CSpVzmLayer layer; VZMLAYDAT layDat;
    adStartLayerGet( dwghandle ); char on,frozen,vpfrozen,locked;
    for (int i=0; i<(short)adNumLayers(dwghandle); i++ )
    {
        adGetLayerState( dwghandle,adtb->lay.objhandle,&on,&frozen,&vpfrozen,&locked );
        adGetLayer( dwghandle,&adtb->lay );
        if ( !adtb->lay.purgedflag )
        {
            memset( &layDat,0,sizeof(layDat) );
            strcpy_s( layDat.strlayCode, adtb->lay.name );
            memcpy( &(layDat.UsrColor) ,GetColorTable(BYTE(adtb->lay.color)), sizeof(COLORREF));
            if ( !on    ) layDat.layStat |= ST_HID;
            if ( frozen ) layDat.layStat |= ST_FRZ;
            if ( locked ) layDat.layStat |= ST_LCK;
            layer.SetLayDat( layDat );
            AppendLayer( &layer );
        }
    }
        
    double minX= 9999999999999,minY= 9999999999999;
    double maxX=-9999999999999,maxY=-9999999999999;
    PrintMsg("Get Section: Entites \n");
    AD_OBJHANDLE spaceblkobjhandle;
    for (short entset=0; entset<2; entset++ ) 
    {
        if ( adGetBlockHandle( dwghandle, spaceblkobjhandle, entset==0?AD_PAPERSPACE_HANDLE:AD_MODELSPACE_HANDLE ) )
            GetEnt4Blk( this,dwghandle,spaceblkobjhandle,minX,minY,maxX,maxY );
    }
    adCloseFile(dwghandle);
    adCloseAd2();
    freeadptrs();

    // just for remove remote object
    minX= 9999999999999,minY= 9999999999999;
    maxX=-9999999999999,maxY=-9999999999999;
	int i=0;
    double cxd=0,cyd=0; int tx,ty,entSum=0; ENTIDX *p,*pListEnts  = m_pListEnts.GetData( entSum );
    for ( p=pListEnts,i=0;i<entSum;i++,p++ ){ cxd += (p->xl+p->xr)/2; cyd += (p->yb+p->yt)/2; }
    int cx = int(cxd/entSum),cy = int(cyd/entSum); cxd=0; cyd=0;
    for ( p=pListEnts,i=0;i<entSum;i++,p++ ){
        tx = (p->xl+p->xr)/2-cx; if (tx<0) tx=-tx; 
        ty = (p->yb+p->yt)/2-cy; if (ty<0) ty=-ty;
        cxd += tx; cyd += ty;
    }
    int dx = int(cxd/entSum)*16,dy = int(cyd/entSum)*16;
    for ( p=pListEnts,i=0;i<entSum;i++,p++ ){
        tx = (p->xl+p->xr)/2-cx; if (tx<0) tx=-tx; 
        ty = (p->yb+p->yt)/2-cy; if (ty<0) ty=-ty;
        if ( tx > dx || ty > dy ) RemoveEnt(i);
        else{
            if ( minX>p->xl ) minX = p->xl; if ( maxX<p->xr ) maxX=p->xr;
            if ( minY>p->yb ) minY = p->yb; if ( maxY<p->yt ) maxY=p->yt;
        }
    }   
    return TRUE;
}