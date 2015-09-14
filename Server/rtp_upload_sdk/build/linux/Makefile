include env.mk

ifndef release
export OPSCFLAGS = -O0
export OPSLIBS = -lgcov
export OPSDEF = -DDEBUG
else
export OPSCFLAGS = -O2
export OPSLIBS = 
endif

CINCS += -I../../extra/curl/include
CFLAGS += $(OPSCFLAGS) 
LIBS += $(OPSLIBS)
DEFS += $(OPSDEF)
LIBUUID = /usr/lib64/libuuid.a
LIBCURL = ../../extra/curl/lib/libcurl.a
SRC_DIR = ../../src
UTIL_DIR = $(SRC_DIR)/util
COMMON_DIR = $(SRC_DIR)/common
INTERLIVE_DIR = $(SRC_DIR)/interlive
AVCODEC_DIR = $(SRC_DIR)/interlive/avcodec
AVFORMAT_DIR = $(SRC_DIR)/interlive/avformat
RTPTRANS_DIR = $(SRC_DIR)/interlive/rtp_trans

RECEIVER = receiver
BIN = $(RECEIVER)

COMMONOBJS = $(UTIL_DIR)/log.o $(UTIL_DIR)/buffer.o $(UTIL_DIR)/memory.o $(UTIL_DIR)/util.o \
			 $(UTIL_DIR)/md5.o $(UTIL_DIR)/DXThread.o $(UTIL_DIR)/httpclient.o  \
			 $(COMMON_DIR)/proto.o $(COMMON_DIR)/proto_rtp_rtcp.o \
			 $(LIBUUID) $(LIBCURL)

INTERLIVE_OBJS = $(INTERLIVE_DIR)/rtp_upload_sdk.o $(INTERLIVE_DIR)/rtp_info.o $(INTERLIVE_DIR)/rtp_upload.o

AVFORMAT_OBJS = $(AVFORMAT_DIR)/rtcp_dec.o $(AVFORMAT_DIR)/rtcp_enc.o $(AVFORMAT_DIR)/rtcp_helper.o $(AVFORMAT_DIR)/rtp_enc.o \
	$(AVFORMAT_DIR)/rtp_enc_aac.o $(AVFORMAT_DIR)/rtp_enc_h264.o $(AVFORMAT_DIR)/sdp.o 

RTP_TRANS_OBJS = $(RTPTRANS_DIR)/rtp_session.o $(RTPTRANS_DIR)/rtp_trans.o $(RTPTRANS_DIR)/rtp_trans_manager.o

AVCODEC_OBJS = $(AVCODEC_DIR)/aac.o  $(AVCODEC_DIR)/h264.o

RECEIVEROBJS = receiver.o $(INTERLIVE_OBJS) $(AVFORMAT_OBJS) $(RTP_TRANS_OBJS) $(COMMONOBJS) $(AVCODEC_OBJS)

all: $(RECEIVER) 
	@echo -e "\033[32m[INF] Make sdk successfully. \033[0m"
	@echo

receiver.o: $(INTERLIVE_DIR)/testq.cpp 
	$(CC) $(CFLAGS) $(LIBS) $(CINCS) -c -DPROCESS_NAME='"receiver"' $(DEFS) $^ -o $@

$(RECEIVER): $(RECEIVEROBJS)
	$(CC) $(CFLAGS) $(LIBS) $(RECEIVEROBJS)  $(LFLAGS) -o $@
	@echo -e "\033[32m[INF] Make receiver successfully. \033[0m"
	@echo

clean:
	rm -f *.o
	@cd $(INTERLIVE_DIR); rm -f *.o
	@cd $(AVCODEC_DIR); rm -f *.o
	@cd $(AVFORMAT_DIR); rm -f *.o
	@cd $(RTPTRANS_DIR); rm -f *.o
	@cd $(COMMON_DIR); rm -f *.o
	@cd $(UTIL_DIR); rm -f *.o
	@echo -e "\033[32m[INF] Clean up successfully. \033[0m"
	@echo
