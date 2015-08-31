include env.mk

ifndef release
export OPSCFLAGS = -O0
export OPSLIBS = -lgcov
export OPSDEF = -DDEBUG
else
export OPSCFLAGS = -O2
export OPSLIBS = 
endif

CFLAGS += $(OPSCFLAGS) -lpthread -lrt 
LIBS += $(OPSLIBS)
DEFS += $(OPSDEF)
LIBUUID = /usr/lib64/libuuid.a
UTIL_DIR = ../util
COMMON_DIR = ../common
AVCODEC_DIR = avcodec
AVFORMAT_DIR = avformat
RTPTRANS_DIR = rtp_trans

RECEIVER = receiver
BIN = $(RECEIVER)

COMMONOBJS = DXThread.o rtp_download_sdk.o \
	$(UTIL_DIR)/log.o $(UTIL_DIR)/statfile.o $(UTIL_DIR)/buffer.o $(UTIL_DIR)/memory.o $(UTIL_DIR)/util.o \
	$(UTIL_DIR)/md5.o \
	$(COMMON_DIR)/proto.o $(COMMON_DIR)/proto_rtp_rtcp.o \
	$(LIBUUID)

AVFORMAT_OBJS = $(AVFORMAT_DIR)/rtcp_dec.o $(AVFORMAT_DIR)/rtcp_enc.o $(AVFORMAT_DIR)/rtcp_helper.o $(AVFORMAT_DIR)/rtp_enc.o \
	$(AVFORMAT_DIR)/rtp_enc_aac.o $(AVFORMAT_DIR)/rtp_enc_h264.o $(AVFORMAT_DIR)/sdp.o 

RTP_TRANS_OBJS = $(RTPTRANS_DIR)/rtp_session.o $(RTPTRANS_DIR)/rtp_trans.o $(RTPTRANS_DIR)/rtp_trans_manager.o

AVCODEC_OBJS = $(AVCODEC_DIR)/aac.o  $(AVCODEC_DIR)/h264.o

RECEIVEROBJS = receiver.o $(AVFORMAT_OBJS) $(RTP_TRANS_OBJS) $(COMMONOBJS) $(AVCODEC_OBJS)

all: $(RECEIVER) 
	@echo -e "\033[32m[INF] Make interlive successfully. \033[0m"
	@echo

receiver.o:testq.cpp 
	$(CC) $(CFLAGS) $(CINCS) -c -DPROCESS_NAME='"receiver"' $(DEFS) $^ -o $@

$(RECEIVER): $(RECEIVEROBJS)
	$(CC) $(CFLAGS) $(RECEIVEROBJS)  $(LFLAGS) -o $@
	@echo -e "\033[32m[INF] Make receiver successfully. \033[0m"
	@echo

clean:
	rm -f *.o
	@cd rtp_trans; rm -f *.o
	@cd avformat; rm -f *.o
	@cd avcodec; rm -f *.o
