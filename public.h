
#ifndef PUBLIC_H_INCLUDED
#define PUBLIC_H_INCLUDED


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dcc_debug.h"

#include "dcc_msg.h"

#ifdef USE_SDFS
#include "sdfs.h"

/*SDFS客户端句柄*/
SDFS g_sdfs;

/*输入文件句柄*/
SFILE *fp_in; 

#else

FILE *fp_in;

#endif


/*统计结果是否为秒级别，1:是，0:否 */
int g_sts_second_flag;

int ascii_char_maps[ 256 ] ;
void init_ascii_char_maps();
int  __HexStrToHexInt( const char *szSrc , unsigned char *chDst );
int  HexStrToHexInt( const char *szSrc , unsigned char *chDst , int *length );



#endif

