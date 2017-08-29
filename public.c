#include <time.h>
#include <sys/time.h>
#include "public.h"

void init_ascii_char_maps()
{
    int i ;
    memset( ascii_char_maps , -1, sizeof ascii_char_maps);
    for( i = 'a' ; i < 'g' ; i++)
    {
        ascii_char_maps[ i ] = i - 'a' + 0x0a;
    }
    for( i = 'A' ; i < 'G' ; i++)
    {
        ascii_char_maps[ i ] = i - 'A' + 0x0a;
    }
    for( i = '0' ; i <= '9' ; i++ )
    {
        ascii_char_maps[ i ] = i - '0' ;
    }
}



/** \brief 字符格式转换处理
 *  "A0" ==> 0xA0
 *  两个字节数据 --> 一个字节数值
 * \param
 * \param
 * \return
 *
 */
int  __HexStrToHexInt( const char *szSrc , unsigned char *chDst )
{
    unsigned int tmp_val;
    *chDst = 0;
    tmp_val = ascii_char_maps [ szSrc[0] ] ;
    if( tmp_val == -1 )
       return -1;
    ASSERT( tmp_val != -1 , "unknown character value[%x]\n" , szSrc[0]);
    *chDst = (tmp_val << 4);
    tmp_val = ascii_char_maps [ szSrc[1] ] ;
    if( tmp_val == -1 )
       return -1;
    ASSERT( tmp_val != -1 , "unknown character value[%x]\n" , szSrc[1]);
    *chDst += tmp_val;
    /*debug("[%c,%c]==>[%02X]\n",szSrc[0],szSrc[1], *chDst);*/
    return 0;
}

/** \brief 字符串格式转换处理
 *  "A0B0C0" ==> 0xA0B0C0
 *  2n个字节数据 --> n个字节数值(n >= 0)
 * \param
 * \param
 * \return
 *
 */

int  HexStrToHexInt( const char *szSrc , unsigned char *chDst , int *length )
{
    int i,j,ret ;
    int src_len = strlen(szSrc);
    const char *pSrc;
    char *pDst;
    if( src_len % 2 != 0 )
    {
        fprintf( stderr, "szSrc length[%d] error! \n", src_len);
        return -1;
    }
    if( src_len == 0 )
    {
        chDst[0] = 0;
        *length = 0;
        return 0;
    }

    for( pSrc = szSrc, pDst = chDst ; (pSrc - szSrc) < src_len ; pSrc  += 2, pDst++ )
    {
        ret = __HexStrToHexInt( pSrc , pDst );
        if( ret < 0 )
        {
            fprintf( stderr, "__HexStrToHexInt return [%d] error\n", ret );
            return -2;/**< 内部字符串格式错误，存在非法字符*/
        }
    }
    *length = src_len/2;
    chDst[*length] = 0;

    return 0;
}


void init_dcc_record_info()
{
    memset( g_dcc_record_info_list, 0 , sizeof( g_dcc_record_info_list ) );
    memset( &g_dcc_record_info, 0, sizeof ( g_dcc_record_info ));
    g_dcc_record_info.usu_list = NULL;
    g_dcc_record_info.usu_count = 0;
    g_usu_service_unit_flag = 0;
}


#ifdef _OUT_RECORD_

/** \brief 显示当前正在处理的DCC消息内容
 *
 * \return void
 *
 */
void display_dcc_record_info_old()
{
    DCC_REC_INFO_T *p = &g_dcc_record_info;

    struct tm  *ptm;
    time_t tmv = p->up_datetime ;

    ptm = localtime( &tmv );
    fprintf( stdout, "%s:%s:%s:%s:%d:%d:%d:%04d%02d%02d%02d%02d%02d",
        p->session_id ,p->charging_id ,p->msisdn,p->imsi,p->request_type,p->request_num, p->rat_type, ptm->tm_year + 1830  , ptm->tm_mon+1 , ptm->tm_mday +1 , ptm->tm_hour, ptm->tm_min, ptm->tm_sec) ;
    DCC_USU_T *p_list = p->usu_list;
    while( p_list )
    {
         fprintf( stdout , ":%010d,%ld,%ld,%ld",
                p_list->rating_group, p_list->cc_total_octets, p_list->cc_input_octets, p_list->cc_output_octets );
        p_list = p_list->next;
    }
    fprintf( stdout ,"\n");
}

/** \brief 显示当前正在处理的DCC消息内容
 *  *
 *   * \return void
 *    *
 *     */
void display_dcc_record_info()
{
    DCC_REC_INFO_T *p = &g_dcc_record_info;

    struct tm  *ptm;
    time_t tmv = p->up_datetime ;

    ptm = localtime( &tmv );

    DCC_USU_T *p_list = p->usu_list;
    while( p_list )
    {
        fprintf( stdout, "%s:%d:%s:%d:%d:%04d%02d%02d%02d%02d%02d",
            p->msisdn ,p->request_type, p->charging_id,p->request_num, p->rat_type, \
            ptm->tm_year + 1830  , ptm->tm_mon+1 , ptm->tm_mday +1 , ptm->tm_hour, ptm->tm_min, ptm->tm_sec) ;
         fprintf( stdout , ":%010d,%ld,%ld,%ld\n",
                p_list->rating_group, p_list->cc_total_octets, p_list->cc_input_octets, p_list->cc_output_octets );
        p_list = p_list->next;
    }
}


#endif

