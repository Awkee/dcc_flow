#ifndef DCC_DEBUG_H_INCLUDED
#define DCC_DEBUG_H_INCLUDED


#ifdef _DEBUG_

#define show_record(...)  fprintf(stdout, __VA_ARGS__)

#define debug_avp_value(...)    fprintf(stderr, __VA_ARGS__)
#define debug_msg(...)          fprintf(stderr, __VA_ARGS__)
#define debug(...)              fprintf(stderr, __VA_ARGS__)
#define ASSERT(x , ...) do {                            \
        if( !( x ) )                                    \
        {                                               \
            fprintf(stderr, "LINE:%d,FUNC:%s ,TIME:%s\n"\
                , __LINE__, __func__, __TIME__);        \
            fprintf( stderr, __VA_ARGS__);              \
            exit( 1 );                                  \
        }                                               \
} while ( 0 )

#else

#define debug_avp_value(...)
#define debug_msg(...)
#define debug(...)
#define ASSERT(x , ...)

#define show_record(...) 

#endif


#ifdef _OUT_RECORD_

void display_dcc_record_info();

#define display_msg_sts_info()

#else

void display_msg_sts_info();


#define display_dcc_record_info(...)

#endif






#endif /* DCC_DEBUG_H_INCLUDED */


