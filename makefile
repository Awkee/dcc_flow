

## -D _DEBUG_ : 开启调试模式，输出每行解析信息
## -D _OUT_RECORD_ : 开启输出每条解析字段记录信息 ##
## -D LITTLE_ENDIAN : 开启小端序主机解析方法


DEBUG_FLAGS	=	-D _OUT_RECORD_  
CFLAGS 		= 
LDFLAGS		=  -lpthread -lm

# 支持SDFS 开关 ##
sdfs_flag = 0

ifeq ($(sdfs_flag),1)
	CFLAGS 	+= -I${SDFS_HOME}/include -I${DSC_PATH}/include -D USE_SDFS
	LDFLAGS += -L${SDFS_HOME}/lib -lsdfs -L${DSC_PATH}/lib/ -lddci -ldl
endif

ifeq ($(mode),debug)
	CFLAGS 		+= 	-g -O2 -D LITTLE_ENDIAN ${DEBUG_FLAGS}
	TARGET1 	= 	dcc_flow_d
else
	CFLAGS      +=   -O2 -D LITTLE_ENDIAN
	TARGET1     =   dcc_flow
endif

CC		=	gcc  -m64 





OBJS1		=	dcc_msg.o public.o main.o store_info.o 

all:${TARGET1}

${TARGET1}:${OBJS1}
	${CC} -o $@ ${OBJS1} ${LDFLAGS}

${TARGET1}:${OBJS1}
	${CC} -o $@ ${OBJS1} ${LDFLAGS}

*.o:*.c
	${CC} -c $< 

clean:
	@rm -f *.o core

purge:
	@rm -f ${TARGET1}

