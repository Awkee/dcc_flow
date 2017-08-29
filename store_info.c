#include "public.h"


/** \brief 添加费率组节点
 *
 * \param
 * \param
 * \return  返回创建成功后的节点指针
 *
 */

DCC_USU_T *add_usu_node( )
{
    DCC_USU_T *p_usu_node = (DCC_USU_T *) malloc( sizeof( DCC_USU_T) );
    ASSERT( p_usu_node != NULL , " malloc DCC_USU_T node error! at FILE:[%s],LINE:[%d]\n", __FILE__, __LINE__);
    memset( p_usu_node, 0, sizeof( DCC_USU_T ));
    p_usu_node->reporting_reason = -1;
    p_usu_node->next = NULL;
    return p_usu_node;
}

/** \brief 释放费率组节点存储空间
 *
 * \param DCC_USU_T *p_usu_node : 需要释放的组节点指针
 * \param
 * \return 0 执行成功， 无实际意义，可不判断返回值
 *
 */

int free_usu_node( DCC_USU_T *p_usu_node )
{
    free( p_usu_node );
    p_usu_node = NULL;
    return 0;
}


/** \brief 释放费率组列表存储空间
 *
 * \param list DCC_USU_T*   :费率组列表指针
 * \return int              : 0 返回成功，否则释放失败
 *
 */
int free_usu_list( DCC_USU_T *list )
{
    DCC_USU_T *p_usu_node = list;
    DCC_USU_T *p_next;
    p_next = list;
    ASSERT( p_usu_node != NULL , " malloc DCC_USU_T node error! at FILE:[%s],LINE:[%d]\n", __FILE__, __LINE__);
    while( p_next )
    {
        p_next = p_usu_node->next;
        free(p_usu_node);
        p_usu_node = p_next;
    }
    list = NULL;
    return 0;
}


void display_msg_detail ( MSG_DETAIL_T *ptr )
{
    int i=0;

    fprintf(stdout , "%c%ld%c%ld%c%ld%c%ld%c%ld%c%ld", seperator, ptr->count, seperator, ptr->ccr_i_count, seperator, \
            ptr->ccr_u_count, seperator, ptr->ccr_t_count , seperator ,\
            ptr->cc_input_octets, seperator, ptr->cc_output_octets  );

    while( i < 10 )
    {
        fprintf(stdout , "%c%ld",  seperator, ptr->reporting_reason[i] );
        i++;
    }

    i=0;
    while( i < 10 )
    {
        fprintf(stdout , "%c%ld" , seperator, ptr->termination_cause[i] );
        i++;
    }
}
void init_msg_sts_info()
{
    memset(&g_msg_sts_info, 0 , sizeof( g_msg_sts_info ));
}

#ifndef _OUT_RECORD_

void display_msg_sts_info()
{
    MSG_DETAIL_T *ptr;
    int i = 0;
    ptr = NULL;
    int l_max_buf_size ;        /*记录循环最大数，如果以秒级统计，为6000，若以分钟级别统计，为60*/
    int l_time_len;             /*记录统计时间长度位数，分钟级别2，秒级别4*/
    if ( g_sts_second_flag == 0 )
    {
        l_max_buf_size  = 2460;/*显示内容为小时+分钟*/
        l_time_len      = 4;
    }
    else
    {
        l_max_buf_size  = MAX_OUTBUF_SIZE;/*显示内容为分钟+秒*/
        l_time_len      = 4;
    }
    for( i = 0; i < l_max_buf_size ; i++ )
    {
        if( g_msg_sts_info[i].msg_count == 0 )
            continue;
        fprintf( stdout ,"%04d%c%ld%c%ld%c%ld", i,seperator,g_msg_sts_info[i].msg_count, seperator, g_msg_sts_info[i].ccr_count , seperator, g_msg_sts_info[i].cca_count);
    
        ptr = &g_msg_sts_info[i].ccr_2g;
        display_msg_detail( ptr );
    
        ptr = &g_msg_sts_info[i].ccr_3g;
        display_msg_detail( ptr );
    
        ptr = &g_msg_sts_info[i].ccr_4g;
        display_msg_detail( ptr );
        fprintf(stdout, "\n");
    }
}

#endif

int add_msg_sts_info( int time_sec )
{

    MSG_DETAIL_T    *ptr_msg;
    ptr_msg = NULL;
    switch ( g_dcc_record_info.rat_type )
    {
    case RAT_TYPE_3G:
        ptr_msg = &g_msg_sts_info[time_sec].ccr_3g;
        break;
    case RAT_TYPE_2G:
        ptr_msg = &g_msg_sts_info[time_sec].ccr_2g;
        break;
    case RAT_TYPE_4G:
        ptr_msg = &g_msg_sts_info[time_sec].ccr_4g;
        break;
    default:
        return -1;
        break;
    }

    DCC_USU_T *p_list = g_dcc_record_info.usu_list;
    while( p_list )
    {
        /*p_list->rating_group, p_list->cc_total_octets, p_list->cc_input_octets, p_list->cc_output_octets );*/
        ptr_msg->cc_input_octets += p_list->cc_input_octets;
        ptr_msg->cc_output_octets += p_list->cc_output_octets;
        /**< 记录CCR上报原因 */
        if( p_list->reporting_reason >= 0 && p_list->reporting_reason < 10 )
        {
            if( p_list->reporting_reason == 6) 
                return 0;
            ptr_msg->reporting_reason[ p_list->reporting_reason ] ++;
        }
        p_list = p_list->next;
    }

    ptr_msg->count++;
    if( g_dcc_record_info.request_type == REQUEST_INIT )
    {
        ptr_msg->ccr_i_count ++;
    }
    else if( g_dcc_record_info.request_type == REQUEST_UPDATE )
    {
        ptr_msg->ccr_u_count ++;

    }
    else if( g_dcc_record_info.request_type == REQUEST_TERM )
    {
        ptr_msg->ccr_t_count ++;
        if( g_dcc_record_info.termination_cause >= 0 && g_dcc_record_info.termination_cause < 10 )
            ptr_msg->termination_cause[ g_dcc_record_info.termination_cause ] ++;
    }
#if 0
    DCC_USU_T *p_list = g_dcc_record_info.usu_list;
    while( p_list )
    {
        /*p_list->rating_group, p_list->cc_total_octets, p_list->cc_input_octets, p_list->cc_output_octets );*/
        ptr_msg->cc_input_octets += p_list->cc_input_octets;
        ptr_msg->cc_output_octets += p_list->cc_output_octets;
        /**< 记录CCR上报原因 */
        if( p_list->reporting_reason >= 0 && p_list->reporting_reason < 10 )
            ptr_msg->reporting_reason[ p_list->reporting_reason ] ++;
        p_list = p_list->next;
    }
#endif

    return 0;
}


