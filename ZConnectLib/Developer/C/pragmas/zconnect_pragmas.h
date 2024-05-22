#ifndef PRAGMAS_ZCONNECT_PRAGMAS_H
#define PRAGMAS_ZCONNECT_PRAGMAS_H

#ifndef CLIB_ZCONNECT_PROTOS_H
#include <clib/zconnect_protos.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(AZTEC_C) || defined(__MAXON__) || defined(__STORM__)
#pragma amicall(ZConnectBase,0x01E,ZCon_GetHeader(a1))
#pragma amicall(ZConnectBase,0x024,ZCon_WriteHeader(a0,a1))
#pragma amicall(ZConnectBase,0x02A,ZCon_FreeHeader(a0))
#pragma amicall(ZConnectBase,0x030,ZCon_GetHeaderLineContents(a0,a1,a2))
#pragma amicall(ZConnectBase,0x036,ZCon_AppendHeaderLine(a0,a1,a2))
#pragma amicall(ZConnectBase,0x03C,ZCon_CheckHeader(a0))
#pragma amicall(ZConnectBase,0x042,ZCon_HeaderToISO(a0))
#pragma amicall(ZConnectBase,0x048,ZCon_HeaderToIBM(a0))
#pragma amicall(ZConnectBase,0x04E,ZCon_StringToISO(a1))
#pragma amicall(ZConnectBase,0x054,ZCon_StringToIBM(a1))
#pragma amicall(ZConnectBase,0x05A,ZCon_CreateDate(a1,d0,d1,a2))
#pragma amicall(ZConnectBase,0x060,ZCon_GetDate(a1,a2,a3))
#pragma amicall(ZConnectBase,0x066,ZCon_CreateID(a1))
#pragma amicall(ZConnectBase,0x06C,ZCon_RemHeaderLines(a0,a1,d0))
#pragma amicall(ZConnectBase,0x072,ZCon_RenameHeaderLines(a0,a1,a2,d0))
#pragma amicall(ZConnectBase,0x078,ZCon_GetZConStr(d0))
#pragma amicall(ZConnectBase,0x07E,ZCon_NewCreateID(a1,a2))
#pragma amicall(ZConnectBase,0x084,ZCon_CreateHeader())
#pragma amicall(ZConnectBase,0x08A,ZCon_CheckRoute(a1,a2))
#pragma amicall(ZConnectBase,0x090,ZCon_QP2ASCII(a1,a2))
#pragma amicall(ZConnectBase,0x096,ZCon_ASCII2QP(a1,a2))
#endif
#if defined(_DCC) || defined(__SASC)
#pragma libcall ZConnectBase ZCon_GetHeader       01E 901
#pragma libcall ZConnectBase ZCon_WriteHeader     024 9802
#pragma libcall ZConnectBase ZCon_FreeHeader      02A 801
#pragma libcall ZConnectBase ZCon_GetHeaderLineContents 030 A9803
#pragma libcall ZConnectBase ZCon_AppendHeaderLine 036 A9803
#pragma libcall ZConnectBase ZCon_CheckHeader     03C 801
#pragma libcall ZConnectBase ZCon_HeaderToISO     042 801
#pragma libcall ZConnectBase ZCon_HeaderToIBM     048 801
#pragma libcall ZConnectBase ZCon_StringToISO     04E 901
#pragma libcall ZConnectBase ZCon_StringToIBM     054 901
#pragma libcall ZConnectBase ZCon_CreateDate      05A A10904
#pragma libcall ZConnectBase ZCon_GetDate         060 BA903
#pragma libcall ZConnectBase ZCon_CreateID        066 901
#pragma libcall ZConnectBase ZCon_RemHeaderLines  06C 09803
#pragma libcall ZConnectBase ZCon_RenameHeaderLines 072 0A9804
#pragma libcall ZConnectBase ZCon_GetZConStr      078 001
#pragma libcall ZConnectBase ZCon_NewCreateID     07E A902
#pragma libcall ZConnectBase ZCon_CreateHeader    084 00
#pragma libcall ZConnectBase ZCon_CheckRoute      08A A902
#pragma libcall ZConnectBase ZCon_QP2ASCII        090 A902
#pragma libcall ZConnectBase ZCon_ASCII2QP        096 A902
#endif

#ifdef __cplusplus
}
#endif

#endif	/*  PRAGMAS_ZCONNECT_PRAGMA_H  */