
#ifndef STORE_INFO_H_INCLUDED
#define STORE_INFO_H_INCLUDED
/**< 定义存储解析后数据的基础数据结构 */

#define  MAX_SESSION_ID_LEN     256
#define  MAX_MSISDN_LEN         15
#define  MAX_IMSI_LEN           17

#define seperator               ':'


/** \brief GPRS使用量信息结构体
 */
typedef struct _dcc_used_service_unit
{
    int             reporting_reason;               /**< 消息上报原因 ，取值: -1 未赋值状态 ，0-9 正常取值*/
    unsigned int    cc_time;                        /**< 已使用的时间。单位：秒 */
    unsigned int    rating_group;                   /**< Rating组标识。 */
    long            cc_total_octets;                /**< 已使用的上下行总字节数，单位：Byte。*/
    long            cc_input_octets;                /**< 从终端用户收到的已使用字节数，单位：Byte。 */
    long            cc_output_octets;               /**< 发送到终端用户的已使用字节数，单位：Byte。 */
    struct _dcc_used_service_unit *next;            /**< 链表指针 */
} DCC_USU_T;




typedef struct _dcc_rec_info
{
    char        session_id[ MAX_SESSION_ID_LEN];    /**< DCC消息session-ID */
    char        charging_id[9];                     /**< PDN会话ID */
    char        imsi[ MAX_IMSI_LEN ];               /**< 长度为16 */
    char        msisdn[MAX_MSISDN_LEN];             /**< 计费号码 */
    unsigned int    request_type;                   /**< proxy 的请求类型 1,2,3; PA请求类型 10,20,30 */
    unsigned int    request_num;                    /**< 请求消息的序号 */
    unsigned int    up_datetime;                    /**< 上报消息的时间戳信息 */
    int             termination_cause;                /**< CCR请求消息结束原因 取值：-1初始状态，未赋值状态| 0-9 时为正常取值*/
    unsigned int    rat_type;                       /**< 网络类型 */
    DCC_USU_T   *usu_list;                          /**< 使用链表结构存储使用量信息 */
    unsigned int     usu_count;                     /**< 记录使用量节点组数 */
}   DCC_REC_INFO_T;



DCC_USU_T *add_usu_node( );
int free_usu_node( DCC_USU_T *p_usu_node );
int free_usu_list( DCC_USU_T *list );



/** \brief 统计DCC消息的详细信息
 */
typedef struct _dcc_msg_details_t {
    long    count;
    long    ccr_i_count;
    long    ccr_u_count;
    long    ccr_t_count;
    long    cc_input_octets;/**< 从终端用户收到的已使用字节数，单位：Byte。CC-Input-Octets必须和CC-Output-Octets同时出现*/
    long    cc_output_octets;/**< 发送到终端用户的已使用字节数，单位：Byte。CC-Input-Octets必须和CC-Output-Octets同时出现*/
    long    reporting_reason[10]; /**< 统计分析上报原因取值情况 */
    long    termination_cause[10]; /**< 统计分析消息关闭原因 */
} MSG_DETAIL_T ;

/** \brief 统计分析结构体定义
 */
typedef struct _msg_sts_info_t {
    long    msg_count;
    long    msg_err  ;
    long    ccr_count;
    long    cca_count;
    MSG_DETAIL_T    ccr_2g;
    MSG_DETAIL_T    ccr_3g;
    MSG_DETAIL_T    ccr_4g;
} MSG_STS_INFO_T;

MSG_STS_INFO_T g_msg_sts_info[6000];


/** \brief 初始化统计分析全局变量
 *
 * \return void
 *
 */
void init_msg_sts_info();


/** \brief 展现统计分析结果
 *
 * \return void
 *
 */

int add_msg_sts_info( int time_sec );
#endif /* STORE_INFO_H_INCLUDED*/


