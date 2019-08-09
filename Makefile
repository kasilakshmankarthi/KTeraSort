PROG1 = QRandomGenMT.elf
PROG2 = QRandomGenLFSR.elf
PROG3 = QSortRnd.elf
PROG4 = QTeraSort.elf

ifeq ($(ARCH),aarch64)
  ifeq ($(OS), linux)
    COMPILER = /prj/dcg/modeling/encnaa/workloads/share/toolchains/gcc-6.2.1-linaro16.11/aarch64-linux-gnu/bin/aarch64-linux-gnu-
  else
    #For baremetal
    COMPILER = /prj/dcg/modeling/encnaa/workloads/share/toolchains/gcc-6.2.1-linaro16.11/aarch64-linux-gnu/bin/aarch64-elf-
    #LIB_PATH = /prj/dcg/modeling/encnaa/workloads/share/toolchains/gcc-linaro-6.1.1-2016.08-rc2/aarch64-elf/aarch64-elf/libc
  endif
else
  COMPILER=/usr/bin/
endif

CC  = $(COMPILER)gcc
CXX = $(COMPILER)g++

#-ggdb
ifeq ($(OS), linux)
  ifeq ($(ARCH),x86_64)
    CFLAGS = -static -O3 -std=c++11 -m64 -ggdb
    ifeq ($(CPU), ivybridge)
      CFLAGS += -march=ivybridge
    else ifeq ($(CPU), haswell)
      CFLAGS += -march=haswell
    else ifeq ($(CPU), broadwell)
      CFLAGS += -march=broadwell
    endif
  else ifeq ($(ARCH),aarch64)
    CFLAGS = -static -O3 -std=c++11 -ggdb -mcpu=cortex-a57
  endif
else
  #For baremetal
  CFLAGS = -static -specs=aem-ve.specs -O3 -std=c++11 -lrdimon -DBAREMETAL
  #CFLAGS = -L$(LIB_PATH) -lcrt0 -lc -lm -lrdimon
endif

ifeq ($(ARCH),aarch64)
  local_angel=./disk/angel-utils/libangel
  local_angel_include=$(local_angel)/include
  local_angel_lib=$(local_angel)/build

  CFLAGS += -DANGEL -I$(local_angel_include)
  LDFLAGS = -static -L$(local_angel_lib) -langel -flto
else ifeq ($(ARCH),x86_64)
  LDFLAGS = -static -flto
endif

CXXFLAGS = $(CFLAGS)

ifeq ($(CPU), ivybridge)
  all: $(PROG1) $(PROG2) $(PROG3) $(PROG4)

else
  all: $(PROG3) $(PROG4)

endif

SRC1 = QRandomGenMT.cpp
OBJS1 = $(SRC1:.cpp=.o)

SRC2 = QRandomGenLFSR.cpp
OBJS2 = $(SRC2:.cpp=.o)

SRC3 = QSortRnd.cpp
OBJS3 = $(SRC3:.cpp=.o)

SRC4 = QTeraSort.cpp
OBJS4 = $(SRC4:.cpp=.o)

ifeq ($(CPU), ivybridge)
$(PROG1): $(SRC1)
	rm -f $(PROG1) $(OBJS1)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(PROG2): $(SRC2)
	rm -f $(PROG2) $(OBJS2)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
endif

$(PROG3): $(SRC3)
	rm -f $(PROG3) $(OBJS3)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(PROG4): $(SRC4)
	rm -f $(PROG4) $(OBJS4)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(PROG1) $(OBJS1)
	rm -f $(PROG2) $(OBJS2)
	rm -f $(PROG3) $(OBJS3)
	rm -f $(PROG4) $(OBJS4)
