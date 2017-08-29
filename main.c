
#include <stdio.h>          /* fprintf() */
#include <stdlib.h>         /* malloc(), free() */
#include <string.h>         /* strerror(), strcmp(), strlen(), memcpy() */
#include <errno.h>          /* errno */
#include <fcntl.h>          /* open() */
#include <unistd.h>         /* read(), write(), close(), chown(), unlink() */
#include <sys/types.h>
#include <sys/stat.h>       /* stat(), chmod() */
#include <utime.h>          /* utime() */

#include "public.h"


#ifdef _OUT_RECORD_
#define display_dcc_record_info()
#endif


#define MAX_BUF_SIZE    40960

char *usage_options_CN[] = {
"Usage:",
"	sts_flow_wc  [ --local_dir=<local_dir> | --sdfs_dir=<sdfs_dir> | file_list ] [ -o file ]",
"Parameters:",
"	input options:",
"		-l,--local_dir,  设置数据来源为本地文件系统目录",
"		-s,--sdfs_dir ,  设置数据来源为SDFS系统目录",
"		[file1, file2 ... filen]  ,   设置数据来源文件，默认读取的是本地文件系统文件",
"	output options:",
"		-o output_file , 设置输出的文件名",
NULL
};

char *usage_options_EN[] = {
"Usage:",
"	sts_flow_wc  [ file_list ] [ -o file ]",
"Parameters:",
"	input options:",
"		[file1, file2 ... filen]  ,   set local file list",
"	output options:",
"		-o output_file , set output file path",
NULL
};


void usage( char *app_name )
{
	char **p = usage_options_EN;
	while ( *p != NULL )
	{
		fprintf( stdout , "%s\n", *p++ );
	}
	fprintf( stdout , "-------------------------------\n");
}

int trim_dcc_buff( const char *szSrc, char *szDst  , int *time_sec)
{
    int     l_time_len;/*统计时间级别:秒级别为4，分钟级别为2*/
    int     l_time_pos;/*标记开始统计的时间位置*/
    const char  *pSrc;
    char        *pDst;
    char        t_sec[ 8 ];

    /*去掉前21字节的时间戳信息和冒号(:)*/
    pSrc = szSrc + 21 ;
    pDst = szDst;

    if( strncmp( szSrc, "20", 2 ) != 0 )
        return -1;

    /*用于记录消息处理时间(分秒数)*/
    memset( t_sec , 0, sizeof( t_sec ) );

    if( g_sts_second_flag == 1 )
    {
        l_time_pos = 10;
        l_time_len = 4;
    }
    else
    {
        l_time_pos = 8;
        l_time_len = 4;
    }
    strncpy( t_sec, szSrc + l_time_pos, 4 );
    
    *time_sec = atoi( t_sec );
    if( *time_sec < 0 || *time_sec > 6000 )
        return -2;
//    fprintf(stderr,"[%s],[%d]\n",t_sec, *time_sec);
    
    while( *pSrc != '\0' )
    {
        switch( *pSrc )
        {
        case ' ':
        case '\n':
        case '\r':
            pSrc++;
            break;
        default:
            *pDst++ = *pSrc++;
            break;
        }
    }
    /*szDstLen = pDst - szDst ;*/
    *pDst = '\0';
    return 0;
}

int main( int argc ,char *argv[] )
{
    extern int optind;
    extern char *optarg;

    int i,ret, len,rec_len, test;
    char    *outname;
    int     zfile_flag = 0;
    int     time_sec;

    char szSrc[ MAX_BUF_SIZE ], szTmp[ MAX_BUF_SIZE ] , *pBuff ;
    char szDst[ MAX_BUF_SIZE ];
	char szErrInfo[ MAX_BUF_SIZE];

    DCCA_MSG_HEAD_T dcca_msg_head;
    

    char sid[ 256 ] ,ofile[1024],ifile[1024], g_msisdn[15],out_msisdn_filename[256];
    char charging_id[9];
    int grep_sid_flag,grep_cid_flag, grep_rat_type_flag, grep_msisdn_flag,g_out_msisdn_flag;
    int g_rat_type;
    int binary_output_flag;/**< 控制输出文件内容的格式 , 1:输出为二进制消息格式 , 0:输出为ASCII遛马格式 */
    int  c;
    FILE *ofp, *out_msisdn_fp;

    out_msisdn_fp = NULL;
    ofp = NULL;
    fp_in = NULL;

    g_out_msisdn_flag = grep_msisdn_flag = grep_sid_flag = grep_cid_flag = grep_rat_type_flag = 0;
    binary_output_flag = 0;

    g_sts_second_flag = 0;
    init_msg_sts_info();

    if( argc == 1 )
    {
        usage( argv[0] );
        exit(0 );
    }

    while ( (c = getopt(argc, argv, "hHtsbz:g:r:m:o:c:M:") ) != -1 )
    {
        switch( c )
        {
        case 's':
            g_sts_second_flag = 1;
            break;
        case 'b':
            binary_output_flag = 1;
            break;
        case 't':
            test = 1;
            break;
        case 'g':
            grep_sid_flag = 1;
            memset( sid, 0, sizeof( sid ) );
            strcpy(sid ,optarg);
            break;

        case 'm':
            grep_msisdn_flag = 1;
            memset( g_msisdn, 0, sizeof( g_msisdn) );
            strcpy( g_msisdn , optarg );
            break;

        case 'M':/**< 指定输出Msisdn字段到该文件中 */
            g_out_msisdn_flag = 1;
            memset( out_msisdn_filename, 0 ,sizeof( out_msisdn_filename ) );
            strcpy( out_msisdn_filename , optarg );
            break;

        case 'r':
            grep_rat_type_flag = 1;
            g_rat_type=atoi(optarg);
            break;

        case 'o':
            memset( ofile , 0 , sizeof( ofile ) );
            strcpy(ofile , optarg);
            ofp = fopen( ofile , "wb+");
            ASSERT( ofp != NULL, "open file[%s] error!\n", ofile);
            break;

        case 'c':
           grep_cid_flag = 1;
           char szCid[256], szCidTmp[256];
           int cid_len;
           memset( charging_id, 0 ,sizeof( charging_id ) );
           strcpy( charging_id , optarg);
           debug_msg("%s\n", charging_id);
           break;
        case 'h':
        case 'H':
           usage(argv[0]);
           exit(0);
        case '?':
                break;
        }
    }

#ifdef USE_SDFS
    if ( sinit("h", &g_sdfs , NULL) )
	{
		fprintf(stderr, "sdfs init error[%s]", szErrInfo);
		return -1;
	}
#endif
    init_ascii_char_maps();
    memset( &dcca_msg_head, 0, sizeof ( DCCA_MSG_HEAD_T ));
    rec_len = 0;
    memset( szDst, 0, sizeof( szDst ) );

    if( g_out_msisdn_flag == 1 )
    {
        out_msisdn_fp = fopen( out_msisdn_filename, "a");
        ASSERT( out_msisdn_fp != NULL , "open msisdn_file [%s] error!error info:[%s]\n",out_msisdn_filename,strerror( errno ) );
    }

    for( i = optind ; i < argc ; i++)
    {
        len = (int)strlen(argv[i]);

        if (strcmp(argv[i] + len - 3, ".gz") == 0 ||
            strcmp(argv[i] + len - 3, "-gz") == 0)
            zfile_flag = 1;
        else if (strcmp(argv[i] + len - 2, ".z") == 0 ||
            strcmp(argv[i] + len - 2, "-z") == 0 ||
            strcmp(argv[i] + len - 2, "_z") == 0 ||
            strcmp(argv[i] + len - 2, ".Z") == 0)
            zfile_flag = 1;
        else{
            zfile_flag = 0;
        } 

#ifdef USE_SDFS
        if( saccess( argv[i] , R_OK , &g_sdfs ) == -1 )
#else
        if( access( argv[i] , R_OK ) == -1 )
#endif
        {
             debug_msg( "file[%s] doesn't exits !\n", argv[i] );
             continue;
        }
#ifdef USE_SDFS
        fp_in = sopen( argv[i] , "r", &g_sdfs );
#else
        fp_in = fopen( argv[i], "r");
#endif


        ASSERT( fp_in != NULL, " fopen file error![%s]\n", argv[i] );
        memset( szSrc, 0 , sizeof( szSrc ));
#ifdef USE_SDFS
        while( sgets( szSrc, MAX_BUF_SIZE , fp_in) != NULL )
#else
        while( fgets( szSrc, MAX_BUF_SIZE , fp_in) != NULL )
#endif
        {

            memset( szTmp , 0 , sizeof ( szTmp ));
            trim_dcc_buff(szSrc, szTmp , &time_sec );
            ret = HexStrToHexInt(szTmp, szDst,&rec_len);
            if( ret == -1 )
            {

                fprintf( stderr, "file [%s]  record format  error! szSrc:[%s]\n", argv[i] , szSrc );
                break;
            }
            if( ret == -2 )
            {
                debug_msg("FILE[%s],func:[%s],current LINE:[%d] msg is error[%s]\n", __FILE__, __func__, __LINE__, szSrc );
                continue;
            }
            g_msg_sts_info[time_sec].msg_count ++;
            if( -1 == deal_msg_line( szDst , time_sec ) )
            {
                memset(szSrc,0, sizeof( szSrc));
                g_msg_sts_info[time_sec].msg_err++;
                continue;
            }

            if( grep_sid_flag == 1 )
            {
                if( strncmp( g_dcc_record_info.session_id , sid , strlen( sid ) ) == 0 )
                    if( ofp != NULL && binary_output_flag == 1) 
                       fwrite( szDst, rec_len , 1 ,ofp );  
                    else if( ofp != NULL && binary_output_flag == 0 )
                       fprintf( ofp , "%s", szSrc);
                    else
                        fprintf( stdout, "%s", szSrc);
            }
            else if( grep_cid_flag == 1 )
            {
                if(  strcmp( g_dcc_record_info.charging_id , charging_id ) == 0 )
                    if( ofp != NULL && binary_output_flag == 1)
                       fwrite( szDst, rec_len , 1 ,ofp );
                    else if( ofp != NULL && binary_output_flag == 0 )
                       fprintf( ofp , "%s", szSrc);
                    else
                        fprintf( stdout, "%s", szSrc);
            }
            else if( grep_rat_type_flag == 1 )
            {
                if( g_dcc_record_info.rat_type == g_rat_type )
                    if( ofp != NULL && binary_output_flag == 1)
                       fwrite( szDst, rec_len , 1 ,ofp );
                    else if( ofp != NULL && binary_output_flag == 0 )
                       fprintf( ofp , "%s", szSrc);
                    else
                        fprintf( stdout, "%s", szSrc);
            }
            else if ( grep_msisdn_flag == 1 )
            {

                if(  strcmp( g_dcc_record_info.msisdn , g_msisdn ) == 0 )
                    if( ofp != NULL && binary_output_flag == 1)
                       fwrite( szDst, rec_len , 1 ,ofp );
                    else if( ofp != NULL && binary_output_flag == 0 )
                       fprintf( stdout, "%s", szSrc);
                    else
                        fprintf( stdout, "%s", szSrc);
            }
            if( g_out_msisdn_flag == 1 && out_msisdn_fp != NULL)
                fprintf(out_msisdn_fp, "%s %02d\n",g_dcc_record_info.msisdn , g_dcc_record_info.rat_type );

            memset( szSrc, 0 , sizeof( szSrc ));
        }
#ifdef USE_SDFS
        sclose( fp_in);
#else
        fclose( fp_in);
#endif
        fp_in = NULL;
    }
    display_msg_sts_info();
    if( ofp != NULL )
        fclose(ofp);

    if( g_out_msisdn_flag == 1 &&  out_msisdn_fp != NULL )
        fclose( out_msisdn_fp );

    return 0;
}

