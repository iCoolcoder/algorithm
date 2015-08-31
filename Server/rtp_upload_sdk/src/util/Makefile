include env.mk
USRLIB = ../lib
CFLAGS += $(OPSCFLAGS)
CINCS += -I$(USRLIB)/event -I$(USRLIB)/json
SOURCES_C = $(wildcard *.c)
SOURCES_CPP = $(wildcard *.cpp)
OBJS = $(patsubst %.c,%.o,$(SOURCES_C)) $(patsubst %.cpp,%.o,$(SOURCES_CPP))
UNIT_TESTS_DIR = unit_tests

all:objs

objs:$(OBJS)

%.o:%.c
	$(CC) -c $(CFLAGS) $(CINCS) $^ -o $@

%.o:%.cpp
	$(CC) -c $(CFLAGS) $(CINCS) $^ -o $@

unit_tests:$(OBJS)
	@cd $(UNIT_TESTS_DIR); \
	make unit_tests; \
	make run	

clean:
	rm *.o -f

cleangcov:
	rm -f *.gcno *.gcda

clean_unit_tests:
	@cd $(UNIT_TESTS_DIR); \
	make clean

cleanall:clean cleangcov
