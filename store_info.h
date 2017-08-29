
#ifndef STORE_INFO_H_INCLUDED
#define STORE_INFO_H_INCLUDED
/**< ����洢���������ݵĻ������ݽṹ */

#define  MAX_SESSION_ID_LEN     256
#define  MAX_MSISDN_LEN         15
#define  MAX_IMSI_LEN           17

#define seperator               ':'


/** \brief GPRSʹ������Ϣ�ṹ��
 */
typedef struct _dcc_used_service_unit
{
    int             reporting_reason;               /**< ��Ϣ�ϱ�ԭ�� ��ȡֵ: -1 δ��ֵ״̬ ��0-9 ����ȡֵ*/
    unsigned int    cc_time;                        /**< ��ʹ�õ�ʱ�䡣��λ���� */
    unsigned int    rating_group;                   /**< Rating���ʶ�� */
    long            cc_total_octets;                /**< ��ʹ�õ����������ֽ�������λ��Byte��*/
    long            cc_input_octets;                /**< ���ն��û��յ�����ʹ���ֽ�������λ��Byte�� */
    long            cc_output_octets;               /**< ���͵��ն��û�����ʹ���ֽ�������λ��Byte�� */
    struct _dcc_used_service_unit *next;            /**< ����ָ�� */
} DCC_USU_T;




typedef struct _dcc_rec_info
{
    char        session_id[ MAX_SESSION_ID_LEN];    /**< DCC��Ϣsession-ID */
    char        charging_id[9];                     /**< PDN�ỰID */
    char        imsi[ MAX_IMSI_LEN ];               /**< ����Ϊ16 */
    char        msisdn[MAX_MSISDN_LEN];             /**< �ƷѺ��� */
    unsigned int    request_type;                   /**< proxy ���������� 1,2,3; PA�������� 10,20,30 */
    unsigned int    request_num;                    /**< ������Ϣ����� */
    unsigned int    up_datetime;                    /**< �ϱ���Ϣ��ʱ�����Ϣ */
    int             termination_cause;                /**< CCR������Ϣ����ԭ�� ȡֵ��-1��ʼ״̬��δ��ֵ״̬| 0-9 ʱΪ����ȡֵ*/
    unsigned int    rat_type;                       /**< �������� */
    DCC_USU_T   *usu_list;                          /**< ʹ������ṹ�洢ʹ������Ϣ */
    unsigned int     usu_count;                     /**< ��¼ʹ�����ڵ����� */
}   DCC_REC_INFO_T;



DCC_USU_T *add_usu_node( );
int free_usu_node( DCC_USU_T *p_usu_node );
int free_usu_list( DCC_USU_T *list );



/** \brief ͳ��DCC��Ϣ����ϸ��Ϣ
 */
typedef struct _dcc_msg_details_t {
    long    count;
    long    ccr_i_count;
    long    ccr_u_count;
    long    ccr_t_count;
    long    cc_input_octets;/**< ���ն��û��յ�����ʹ���ֽ�������λ��Byte��CC-Input-Octets�����CC-Output-Octetsͬʱ����*/
    long    cc_output_octets;/**< ���͵��ն��û�����ʹ���ֽ�������λ��Byte��CC-Input-Octets�����CC-Output-Octetsͬʱ����*/
    long    reporting_reason[10]; /**< ͳ�Ʒ����ϱ�ԭ��ȡֵ��� */
    long    termination_cause[10]; /**< ͳ�Ʒ�����Ϣ�ر�ԭ�� */
} MSG_DETAIL_T ;

/** \brief ͳ�Ʒ����ṹ�嶨��
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


/** \brief ��ʼ��ͳ�Ʒ���ȫ�ֱ���
 *
 * \return void
 *
 */
void init_msg_sts_info();


/** \brief չ��ͳ�Ʒ������
 *
 * \return void
 *
 */

int add_msg_sts_info( int time_sec );
#endif /* STORE_INFO_H_INCLUDED*/


