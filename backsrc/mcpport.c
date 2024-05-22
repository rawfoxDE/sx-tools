//static const char __version[] = "\0$VER: Node Control v1.0 ("__DATE__")";


#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
//#include <sxstructs.h>

struct MCPMessage
{
        struct Message Msg;
        UWORD command;
        UWORD nodenum;
        long data1;
        long data2;
        long data3;
        long data4;
        long data5;
        long data6;
} themsg;
struct MsgPort *MyPort, *MCPPort;

void main(void)
{

        MCPPort = FindPort("SX-MCP");
        if(MCPPort)
        {
                MyPort = CreateMsgPort();
                if(MyPort)
                {
                        themsg.Msg.mn_Length = sizeof(struct MCPMessage);
                        themsg.Msg.mn_ReplyPort = MyPort;
                        themsg.nodenum = 0;
                        themsg.command = 10;    /* 10 = QUERY SYSOP */
                        PutMsg(MCPPort, (struct Message *)&themsg);
                        WaitPort(MyPort);
                        DeleteMsgPort(MyPort);
                        if(themsg.data1)
                        {
                                PutStr("The Sysop is away because:nn");
                                PutStr((char *)themsg.data1);
                                PutStr("n");
                        } else {
                                PutStr("The Sysop is available!n");
                        }
                }
        }
}
