#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dcc_msg.h"

#ifdef _DEBUG_

void  display_msg_head(  DCCA_MSG_HEAD_T *p_msg_head )
{
    DCCA_MSG_HEAD_T *p;
    p = p_msg_head;
    debug_msg( "ver:%d,msg_len:%d\nflag{r:%d,p:%d,e:%d,t:%d,rsvd:%d},command_code:%d\n",
        p->bit_len.ucVer,p->bit_len.uiLen ,
        p->command.r,p->command.p,p->command.e, p->command.t, p->command.rsvd, p->command.uiCmdCode
        );
}

void  display_avp_head(  DCCA_AVP_NODE_HEAD_T *p_avp_head )
{
    DCCA_AVP_NODE_HEAD_T *p;
    p = p_avp_head;
    debug_msg( "avp_code:%d,avp_length:%d,flags: [ v:%d, m:%d, p:%d, rsvd:%d ]\n",
        p->avp_code,p->flag.avp_length , p->flag.v, p->flag.m, p->flag.p, p->flag.rsvd
        );
}
#else

#define display_msg_head(...)
#define display_avp_head(...)

#endif /*_DEBUG_*/

/** \brief 解析AVP_NODE数据信息(获取 值的偏移位置和数据长度)
 *
 * \param const char *szData 节点起始位置指针
 * \param int *iPos     数据值的指针偏移量
 * \param int *iLen     数据值的实际长度
 * \return 0 表示成功， 非0 表示失败
 *
 */
int decode_avp_node_value( const char *szData, int *iPos , int *iLen )
{

    DCCA_AVP_NODE_HEAD_T    *pnode, l_avp_node;     /* Avp节点头结构指针 */
    const char              *pData;

	int     tmp_val;
    int     avp_value_pos;              /* 当前AVP节点的值偏移位置 */
    int     avp_value_len;              /* 当前AVP节点的值的实际长度 */
    pData   = szData;

	memset( &l_avp_node , 0 , sizeof(DCCA_AVP_NODE_HEAD_T));
    pnode   = (DCCA_AVP_NODE_HEAD_T *) pData;
	l_avp_node.avp_code = ntohl( pnode->avp_code );
	memcpy( & l_avp_node.flag ,  &pnode->flag , sizeof( DCCA_AVP_FLAG_T ) );
#ifdef _LITTLE_ENDIAN_
	l_avp_node.flag.avp_length = ntohl( pnode->flag.avp_length << 8);
#else
	l_avp_node.flag.avp_length = ntohl( pnode->flag.avp_length );
#endif

    /* avp_value_pos   = ( pnode->flag.v == 1 ? 12 : 8 );*/
    avp_value_pos   = ( l_avp_node.flag.v == 1 ? (DCCA_AVP_NODE_HEAD_LEN + 4) : DCCA_AVP_NODE_HEAD_LEN );
    avp_value_len   = GET_AVP_NODE_LEN( l_avp_node.flag.avp_length ) - avp_value_pos;

    if( avp_value_len < 0 )
    {
        debug_msg("avp_value_len < 0 error! avp_len:[%d], avp_value_pos:[%d]\n", l_avp_node.flag.avp_length ,avp_value_pos );
        return -1;
    }

    if( avp_value_pos < 0 )
    {
        debug_msg("avp_value_pos < 0 error!,value[%d]\n", avp_value_pos);
        return -2;
    }
    else if(  avp_value_pos > l_avp_node.flag.avp_length )
    {
        debug_msg("avp_value_pos > l_avp_node.flag.avp_length[%d] error!,value[%d]\n", l_avp_node.flag.avp_length ,avp_value_pos );
        return -3;
    }
    *iLen           = avp_value_len;
    *iPos           = avp_value_pos;
    return 0;
}

/** \brief 解析本次交互消息中的所有AVP节点
 *
 * \param const char *szData    : AVP节点的消息数据指针
 * \param int len               : 消息的实际长度值
 * \return 0 解析成功，否则解析错误
 *
 */

int deal_avp_node(const char *szData , int len )
{
    int     i,j,ret;
    int     avp_value_len;
    int     avp_value_pos;
	int     tmp_val ;

    /*进行实际值解析指针*/
    unsigned int     *iNum;
    unsigned long *ui64Num;
    unsigned char cNum;

    /* 指向实际消息数据的指针*/
    const char  *pData ;
    const char  *pTmpData;
    DCCA_AVP_NODE_HEAD_T *p,*pnext , l_avp_node;
    DCC_USU_T *p_usu_node;
    int     szBuffLen;
    char    szBuff[ 4096 ]; /* 数据缓冲区 */

    pData   = szData;

    i = 0;
    while( i < len )
    {
        p       = (DCCA_AVP_NODE_HEAD_T *) pData ;
	    l_avp_node.avp_code = ntohl(p->avp_code)  ;
	    memcpy( & l_avp_node.flag , & p->flag , sizeof(DCCA_AVP_FLAG_T) );
#ifdef _LITTLE_ENDIAN_
		l_avp_node.flag.avp_length = ntohl( p->flag.avp_length << 8);
#else
		l_avp_node.flag.avp_length = ntohl( p->flag.avp_length );
#endif

        ret = decode_avp_node_value(pData, &avp_value_pos , &avp_value_len );
        ASSERT( ret == 0 , "FILE:%s,FUNC:[%s],LINE:%d , return value:[%d]\n", __FILE__, __func__, __LINE__, ret );
                if( ret != 0 )
                    return -1;

        switch( l_avp_node.avp_code )
        {

        case AVP_TERMINAL_CAUSE:
            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );

            iNum = (unsigned int *) szBuff;
            g_dcc_record_info.termination_cause = ntohl(*iNum);
            debug_avp_value("avp_code:[%d], type:[HexInt32]:%08X\n", l_avp_node.avp_code, ntohl(*iNum) );
            break;

        case AVP_SESSION_ID:
        case AVP_REQUEST_TYPE:
        case AVP_REQUEST_NUMBER:
        case AVP_RATING_GROUP:
        case AVP_CC_OUTPUT_OCTETS:
        case AVP_CC_INPUT_OCTETS:
        case AVP_CC_TOTAL_OCTETS:
        case AVP_EVENT_TIMESTAMP:
                case AVP_REPORTING_REASON:

            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );

            if( l_avp_node.avp_code == AVP_SESSION_ID )
            {
                strncpy( g_dcc_record_info.session_id, szBuff , avp_value_len );
                debug_avp_value("avp_code:[%d], type:[string]:%s\n", l_avp_node.avp_code, szBuff );
            }
            else if ( l_avp_node.avp_code == AVP_REQUEST_TYPE )
            {
                iNum = (unsigned int *) szBuff;
                g_dcc_record_info.request_type = ntohl(*iNum);
            }
            else if ( l_avp_node.avp_code == AVP_REQUEST_NUMBER )
            {
                iNum = (unsigned int *) szBuff;
                g_dcc_record_info.request_num = ntohl(*iNum);
            }
            else if( l_avp_node.avp_code == AVP_RATING_GROUP && g_usu_service_unit_flag == 1 )
            {
                iNum = (unsigned int *) szBuff;
                g_dcc_record_info.usu_list->rating_group = ntohl(*iNum);
            }
            else if( l_avp_node.avp_code == AVP_CC_TOTAL_OCTETS && g_usu_service_unit_flag == 1 )
            {
                ui64Num = (unsigned long *) szBuff;
                g_dcc_record_info.usu_list->cc_total_octets = be64toh(*ui64Num);

debug_avp_value("avp_code:[%d], type:[uint64]:%ld\n", l_avp_node.avp_code, g_dcc_record_info.usu_list->cc_total_octets ); 

            }
            else if( l_avp_node.avp_code == AVP_CC_OUTPUT_OCTETS && g_usu_service_unit_flag == 1 )
            {
                ui64Num = (unsigned long *) szBuff;
                g_dcc_record_info.usu_list->cc_output_octets = be64toh(*ui64Num);
debug_avp_value("avp_code:[%d], type:[uint64]:%ld\n", l_avp_node.avp_code,  g_dcc_record_info.usu_list->cc_output_octets );
            }
            else if( l_avp_node.avp_code == AVP_CC_INPUT_OCTETS && g_usu_service_unit_flag == 1 )
            {
                ui64Num = (unsigned long *) szBuff;
                g_dcc_record_info.usu_list->cc_input_octets = be64toh(*ui64Num);
debug_avp_value("avp_code:[%d], type:[uint32]:%d\n", l_avp_node.avp_code,  be64toh(*ui64Num) );
            }
            else if ( l_avp_node.avp_code == AVP_REPORTING_REASON && g_usu_service_unit_flag == 1 )
            {
                iNum = (unsigned int *) szBuff;
                g_dcc_record_info.usu_list->reporting_reason = ntohl(*iNum);
            }
            else if ( l_avp_node.avp_code == AVP_EVENT_TIMESTAMP )
            {
                iNum = (unsigned int *) szBuff;
                g_dcc_record_info.up_datetime = ntohl(*iNum);
            }
            break;

        case AVP_3GPP_RAT_TYPE:
            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );

            cNum = szBuff[ 0 ];
            g_dcc_record_info.rat_type = cNum;
            debug_avp_value("avp_code:[%d], type:[uint32]:%d\n", l_avp_node.avp_code, cNum );
            break;

        case AVP_SUBSCRIPTION_DATA:
            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );
            if( g_subscription_flag == END_USER_E164 )
            {
                g_subscription_flag = -1;
                strcpy( g_dcc_record_info.msisdn, szBuff);
                debug_avp_value("avp_code:[%d], type:[string],msisdn:%s\n", l_avp_node.avp_code, szBuff );
            }
            else if( g_subscription_flag == END_USER_IMSI )
            {
                g_subscription_flag = -1;
                strcpy( g_dcc_record_info.imsi , szBuff);
                debug_avp_value("avp_code:[%d], type:[string],imsi:%s\n", l_avp_node.avp_code, szBuff );
            }
            break;
        case AVP_SUBSCRIPTION_TYPE:
            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );

            iNum = (unsigned int *) szBuff;
            g_subscription_flag = ntohl(*iNum);
            debug_avp_value("avp_code:[%d], type:[uint32]:%d\n", l_avp_node.avp_code, g_subscription_flag );
            break;

        case AVP_3GPP_CHARGING_ID:
            memset( szBuff, 0 , sizeof( szBuff ));
            memcpy( szBuff , pData + avp_value_pos , avp_value_len );

            iNum = (unsigned int *) szBuff;
            sprintf( g_dcc_record_info.charging_id ,"%08X", ntohl(*iNum ) );
            debug_avp_value("avp_code:[%d], type:[HexInt32]:%08X\n", l_avp_node.avp_code, ntohl(*iNum ) );
            break;

        case GAVP_MSCC:
        case GAVP_SERVICE_INFORMATION:
        case GAVP_PS_INFORMATION:
        case GAVP_SUBSCRIPTION_ID:
                case GAVP_REQUESTED_SERVICE_UNIT:
                        debug_avp_value("avp_code:[%d]\n", l_avp_node.avp_code);
                        debug_avp_value(" ");
            if( 0 != ( ret = deal_avp_node( pData + avp_value_pos , avp_value_len ) ) )
            {
                debug_msg( "FILE:[%s],FUNC:[%s],LINE:[%d] error occur! avp_code:[%d]\n",  __FILE__, __func__, __LINE__, l_avp_node.avp_code );
                return ret;
            }
            break;
        case GAVP_USED_SERVICE_UNIT:
            /**< 新增费率组节点*/
            g_usu_service_unit_flag = 1;
            p_usu_node = add_usu_node(  );
            p_usu_node->next = g_dcc_record_info.usu_list;
            g_dcc_record_info.usu_list = p_usu_node ;
                        debug_avp_value("avp_code:[%d],avp_len:[%d]\n", l_avp_node.avp_code, avp_value_len);
                        debug_avp_value(" ");
            if( 0 != ( ret = deal_avp_node( pData + avp_value_pos , avp_value_len ) ) )
            {
                debug_msg( "FILE:[%s],FUNC:[%s],LINE:[%d] error occur! avp_code:[%d]\n",  __FILE__, __func__, __LINE__, l_avp_node.avp_code );
                return ret;
            }
            break;
        default:
            display_avp_head( &l_avp_node );
            break;
        } /* END of switch( l_avp_node.avp_code ) */
        i += GET_AVP_NODE_LEN( l_avp_node.flag.avp_length  );     /**< 记录已经处理完毕的AVP节点的长度*/
        pData += GET_AVP_NODE_LEN( l_avp_node.flag.avp_length  ); /**< 跳跃到下一个AVP节点起始位置*/

    } /* END of while( i < len ) */

    return 0;
}


/** \brief 解析DCC消息数据流
 *
 * \param const char *szData
 * \return 0 success , -1 error
 *
 */

int deal_msg_line( const char * szData , int time_sec )
{
    /*
    TODO:
        1.解析DCC消息头部格式 DCCA_MSG_HEAD_T
        2.根据消息头部分析出消息类型(CCR/CCA)
        3.读取第一个AVP头部信息 DCCA_AVP_NODE_HEAD_T (session_id字段)
        4.依次读取每一个AVP节点，分别获取(avp_code,length,flags,value)
    */
    int                     msg_len,avp_len;/*消息长度,AVP节点长度*/
    int                     tmp_val;/*临时变量*/
    const char              *pData;/*数据指针，指向需要处理的消息起始位置*/
    DCCA_MSG_HEAD_T         dcca_msg_head, *p_msg_head;
    DCCA_AVP_NODE_HEAD_T    dcca_avp_node_head;
    int                     ret;/**< 记录解析消息返回值 */

    pData = szData;
    p_msg_head = (DCCA_MSG_HEAD_T *) szData;
    memset( & dcca_msg_head , 0 , sizeof( DCCA_MSG_HEAD_T ));
    memcpy( & dcca_msg_head.bit_len , & p_msg_head->bit_len , sizeof(DCC_BIT_LEN_T) );
    memcpy( & dcca_msg_head.command , & p_msg_head->command , sizeof(DCCA_MSG_CMD_T) );
#ifdef _LITTLE_ENDIAN_
    dcca_msg_head.command.uiCmdCode = ntohl(p_msg_head->command.uiCmdCode << 8);
    dcca_msg_head.bit_len.uiLen = ntohl(p_msg_head->bit_len.uiLen << 8);
#else
    dcca_msg_head.command.uiCmdCode = ntohl(p_msg_head->command.uiCmdCode );
    dcca_msg_head.bit_len.uiLen = ntohl(p_msg_head->bit_len.uiLen );
#endif

    display_msg_head( & dcca_msg_head );
    if( dcca_msg_head.bit_len.ucVer == 1 )
    {
        switch( dcca_msg_head.command.uiCmdCode )
        {
        case CC:/*CCR CCA */
            break;
        default:
            return -1;
        }
    }

    pData   += DCCA_MSG_HEAD_LEN;
    msg_len =  dcca_msg_head.bit_len.uiLen - DCCA_MSG_HEAD_LEN;
    init_dcc_record_info();

    ret = deal_avp_node(pData , msg_len );
    if( ret != 0 )
    {
          debug_msg( "FILE:%s,FUNC:%s,LINE:%d, error code:%d, err_msg:[%s]\n",__FILE__, __func__, __LINE__ , ret,  szData );
          return -1;
    }

    if( dcca_msg_head.bit_len.ucVer == 1 )
    {
        switch( dcca_msg_head.command.uiCmdCode )
        {
        case CC:/*CCR CCA */
            if( dcca_msg_head.command.r == 0 )
            {/*CCA*/
                g_msg_sts_info[time_sec].cca_count++;
            }
            else
            {/**CCR*/
                g_msg_sts_info[time_sec].ccr_count++;
                add_msg_sts_info(time_sec );
            }
            break;
        default:
            return -1;
        }
    }

    display_dcc_record_info();

    return 0;
}
