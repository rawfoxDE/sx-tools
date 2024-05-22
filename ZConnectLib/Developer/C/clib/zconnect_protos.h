#ifndef CLIB_ZCONNECT_PROTOS_H
#define CLIB_ZCONNECT_PROTOS_H



/*
$VER: zconnect_protos.h 1.4 (15.2.98) © by Maik "BLiZZeR" Schreiber
*/


#include <exec/types.h>
#include <dos/dos.h>


struct List *ZCon_GetHeader(BPTR filehandle);
void ZCon_WriteHeader(struct List *list, BPTR filehandle);
void ZCon_FreeHeader(struct List *list);
STRPTR ZCon_GetHeaderLineContents(struct List *list, STRPTR id, struct ZConnectHeaderLine **storage);
struct ZConnectHeaderLine *ZCon_AppendHeaderLine(struct List *list, STRPTR id, STRPTR contents);
ULONG ZCon_CheckHeader(struct List *list);
void ZCon_HeaderToISO(struct List *list);
void ZCon_HeaderToIBM(struct List *list);
void ZCon_StringToISO(STRPTR string);
void ZCon_StringToIBM(STRPTR string);
void ZCon_CreateDate(struct ClockData *date, WORD offset, UBYTE summertime, STRPTR buffer);
WORD ZCon_GetDate(STRPTR buffer, struct ClockData *date, UBYTE *summertime);
ULONG ZCon_CreateID(STRPTR string);
void ZCon_RemHeaderLines(struct List *list, STRPTR id, BOOL all);
void ZCon_RenameHeaderLines(struct List *list, STRPTR id, STRPTR newid, BOOL all);
STRPTR ZCon_GetZConStr(ULONG id);
BOOL ZCon_NewCreateID(STRPTR string, STRPTR buffer);
struct List *ZCon_CreateHeader(void);
ULONG ZCon_CheckRoute(STRPTR route, STRPTR system);
ULONG ZCon_QP2ASCII(STRPTR source, STRPTR dest);
ULONG ZCon_ASCII2QP(STRPTR source, STRPTR dest);



#endif   /* CLIB_ZCONNECT_PROTOS_H */
