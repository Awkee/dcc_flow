#ifndef DCC_MSG_H_INCLUDED
#define DCC_MSG_H_INCLUDED

#include <arpa/inet.h>
#include <sys/types.h>


#include "dcc_debug.h"
#include "store_info.h"

#define  GET_AVP_NODE_LEN(len)  ((len)%4 == 0 ? (len) :((len) +4 - (len)%4))


#ifdef _LITTLE_ENDIAN_
#include <endian.h>
#else
#define	be64toh(x)    (x)
#endif




#define MAX_OUTBUF_SIZE     6000

/*the attribute struct of message head*/
typedef struct _dcca_msg_head_cmd_t
{
#ifdef _LITTLE_ENDIAN_ 

        unsigned int    rsvd :4;                /**< Reserved */
        unsigned int    t    :1;                /**< Potentially re-transmitted */
        unsigned int    e    :1;                /**< Error */
        unsigned int    p    :1;                /**< Proxiable */
        unsigned int    r    :1;                /**< Request */
#else
        unsigned int    r    :1;                /**< Request */
        unsigned int    p    :1;                /**< Proxiable */
        unsigned int    e    :1;                /**< Error */
        unsigned int    t    :1;                /**< Potentially re-transmitted */
        unsigned int    rsvd :4;                /**< Reserved */

#endif
        unsigned int    uiCmdCode:24;           /**< Command-Code */

} DCCA_MSG_CMD_T;

typedef struct __dcca_msg_head_bit_len_t 
{
        unsigned int    ucVer:8;                /**< Version */
        unsigned int    uiLen:24;               /**< Message Length */
} DCC_BIT_LEN_T ; 

/*the struct of message head*/

typedef struct  _dcca_msg_head_t{
        DCC_BIT_LEN_T   bit_len;                /**< ver:8 + len:24 */
        DCCA_MSG_CMD_T  command;                /**< command flags */
        unsigned int    uiAppID;                /**< Application-ID */
        unsigned int    uiHbh;                  /**< Hop-by-Hop Identifier */
        unsigned int    uiEte;                  /**< End-to-End Identifier */
} DCCA_MSG_HEAD_T ;


#define DCCA_MSG_HEAD_LEN  sizeof( DCCA_MSG_HEAD_T )

/*the attribute struct of message head*/
typedef struct _dcca_avp_node_flag_t
{
#ifdef _LITTLE_ENDIAN_
        unsigned int    rsvd :5;                /**< Reserved **/
        unsigned int    p    :1;                /**< ָ��Ϊ��֤�˵��˰�ȫ��Ҫ���� */
        unsigned int    m    :1;                /**< Must ָ���Ը�AVP��֧���Ƿ��Ǳ���� */
        unsigned int    v    :1;                /**< Vender-ID flag */
#else
        unsigned int    v    :1;                /**< Vender-ID flag */
        unsigned int    m    :1;                /**< Must ָ���Ը�AVP��֧���Ƿ��Ǳ���� */
        unsigned int    p    :1;                /**< ָ��Ϊ��֤�˵��˰�ȫ��Ҫ���� */
        unsigned int    rsvd :5;                /**< Reserved **/
#endif
        unsigned int    avp_length:24;          /**< AVP Length*/

} DCCA_AVP_FLAG_T;



typedef struct  _dcca_avp_node_head_t {
        unsigned int    avp_code;               /**< AVP Code*/
        DCCA_AVP_FLAG_T flag ;                  /**< V M P r r r r r*/
} DCCA_AVP_NODE_HEAD_T;

#define DCCA_AVP_NODE_HEAD_LEN  sizeof( DCCA_AVP_NODE_HEAD_T )

/*****
** DCC AVP�ڵ�ṹ���Ͷ���
***/
typedef struct _dcca_avp_node_t {
    DCCA_AVP_NODE_HEAD_T    avp_node_head;      /**< AVP�ڵ�ͷ����Ϣ*/
    unsigned int            verdor_id;          /**< ������ID*/
    void                    *value;             /**< AVP�洢ֵ����ڵ�ʱָ��ֵΪNULL��Ҷ�ӽڵ�ʱ�洢ʵ��ֵ*/
    struct _dcca_avp_node_t *child;             /**< �ӽڵ�ָ�룬NULL:Ҷ�ӽڵ㣬����Ϊ��ڵ�*/
    struct _dcca_avp_node_t *brother;           /**< ���ظ��ڵ�ָ�룬NULL:����һ���ظ��ڵ�*/
} DCCA_AVP_NODE_T;


/******
** DCC ��Ϣ���ṹ
**/
typedef struct _dcca_msg_packet_t {

    DCCA_MSG_HEAD_T     m_dcca_msg_head;        /**< ��Ϣͷ����Ϣ */
    DCCA_AVP_NODE_T     m_dcca_avp_node_list;   /**< avp�ڵ��б�  */

} DCCA_MSG_PACKET_T;


/***********************************************************************************
*
******/

/**
* ����ͳ����Ϣ�ṹ����
*/
typedef struct _data_flow_info {
    unsigned int    rating_group;               /**< ������     */
    unsigned int    data_flowdn;                /**< ���������� */
    unsigned int    data_flowup;                /**< ���������� */
} DATA_FLOW_INFO_T;

/**
*   ͳ��������Ϣ�ṹ����
*/
typedef struct _DCC_MSG_STS {
    unsigned int    msg_count;                  /**< DCC��Ϣ����*/
    void            *msg_detail;                /**< ͳ����Ϣ��ϸ��Ϣ*/
} DCC_MSG_STS_T;

DCC_MSG_STS_T dcc_msg_sts[ 1024] [2] ;



extern int decode_avp_node_value( const char *szData, int *iPos , int *iLen );
extern int deal_avp_node(const char *szData , int len);
int deal_msg_line( const char * szData ,int time_sec);



#define CE      257
#define RA      258
#define CC      272
#define AS      274
#define DW      280
#define DP      282
#define R       1   /**< Request*/
#define A       0   /**< Answer */

#define REQUEST_INIT    1
#define REQUEST_UPDATE  2
#define REQUEST_TERM    3

#define RAT_TYPE_3G 1
#define RAT_TYPE_2G 2
#define RAT_TYPE_4G 6



#define CER dcc_msg_sts [ CE ] [ R ]
#define RAR dcc_msg_sts [ RA ] [ R ]
#define CCR dcc_msg_sts [ CC ] [ R ]
#define ASR dcc_msg_sts [ AS ] [ R ]
#define DWR dcc_msg_sts [ DW ] [ R ]
#define DPR dcc_msg_sts [ DP ] [ R ]

#define CEA dcc_msg_sts [ CE ] [ A ]
#define RAA dcc_msg_sts [ RA ] [ A ]
#define CCA dcc_msg_sts [ CC ] [ A ]
#define ASA dcc_msg_sts [ AS ] [ A ]
#define DWA dcc_msg_sts [ DW ] [ A ]
#define DPA dcc_msg_sts [ DP ] [ A ]


#define SIMPLE_NODE 0
#define GROUP_NODE  1

/**< ����AVP_CODE �ڵ����� */
#define AVP_SESSION_ID          263
#define AVP_REQUEST_TYPE        416
#define AVP_REQUEST_NUMBER      415
#define AVP_EVENT_TIMESTAMP     55              /**< ��Ϣ�����ϱ�ʱ��� */
#define AVP_TERMINAL_CAUSE      295             /**< ����CCR-Terminate��Ϣ��ָʾDiameter�ͻ��˻Ự��ֹ��ԭ�� */



#define GAVP_SUBSCRIPTION_ID    443
    #define AVP_SUBSCRIPTION_TYPE 450           /**< �ƷѺ���,��86:0,IMSI����:1 */
    #define AVP_SUBSCRIPTION_DATA 444           /**< �ƷѺ���/IMSI���� */
#define GAVP_USER_EQUIPMENT_INFO    458         /**< IMEI ��ڵ� */
    #define AVP_USER_EQUIPMENT_INFO_VALUE 460   /**< IMEI ֵ ,����16λ */

#define GAVP_MSCC               456             /**< Multiple-Services-Credit Control */
    #define GAVP_REQUESTED_SERVICE_UNIT 437     /**< Requested-Service-Unit */
    #define GAVP_USED_SERVICE_UNIT  446         /**< USED-SERVICE-UNIT */
        #define AVP_REPORTING_REASON 872        /**< Reporting-Reason 0:*/
        #define AVP_CC_TOTAL_OCTETS 421         /**< CC-Total-Octets */
        #define AVP_CC_INPUT_OCTETS 412         /**< CC-Input-Octets */
        #define AVP_CC_OUTPUT_OCTETS 414        /**< CC-Output-Octets */
    #define AVP_RATING_GROUP    432             /**< Rating-Group */

#define GAVP_SERVICE_INFORMATION    873         /**< Service-Information */
    #define GAVP_PS_INFORMATION         874     /**< PS-Information */
        #define AVP_3GPP_CHARGING_ID        2   /**< 3GPP-Charging-Id */
        #define AVP_GGSNADDRESS             847 /**< GGSN-Address */
        #define AVP_SGSNADDRESS             1228/**< SGSN-Address */
        #define AVP_3GPP_USER_LOCATION_INFO 22  /**< 3GPP-User-Location-Info ,GGSN�μ�TS29.060��PGW�μ�TS29.274*/
        #define AVP_3GPP_RAT_TYPE           21  /**< 3GPP-RAT-Type */

enum {
END_USER_E164=0,
END_USER_IMSI,
END_USER_SIP_URI,
END_USER_NAI,
END_USER_PRIVATE
} ;

int  g_subscription_flag ;                      /**< ��¼�ն�������Ϣ�ǼƷѺ������IMSI����*/
int  g_usu_service_unit_flag;                   /**< ��������Ƿ�CCR��Я����ʹ�����ϱ���Ϣ */
#define MAX_REC_INFO_BUFF_SIZE  1000

DCC_REC_INFO_T  g_dcc_record_info_list[ MAX_REC_INFO_BUFF_SIZE ];    /**< �洢��Ϣ��¼���������ɴ洢������Ϣ */
int             g_dcc_record_list_count;                             /**< ��¼��ǰ�洢��Ϣ��¼���� */

DCC_REC_INFO_T  g_dcc_record_info;

/** \brief ��ʼ����Ϣ��¼��Ϣ�ṹ
 *
 * \param
 * \return
 *
 */
void init_dcc_record_info();


#endif /* DCC_MSG_H_INCLUDED*/




