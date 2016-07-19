PROGRAMS=first second

BUILDTYPE?=Debug

os_name=unknown_os
arch_name=ia32
uname_os=$(shell uname)
uname_arch=$(shell uname -m)

ifeq ($(uname_os),Linux)
  os_name=linux
endif

ifeq ($(uname_arch), x86_64)
  arch_name=x64
endif
ifeq ($(uname_arch), amd64)
  arch_name=x64
endif

PAGESPEED_PREFIX=`pkg-config --variable=prefix trafficserver`
PAGESPEED_INCLUDE_DIR=${PAGESPEED_PREFIX}/include/ats_pagespeed/psol

PAGESPEED_INCLUDES =-I$(PAGESPEED_INCLUDE_DIR)\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/chromium/src/\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/google-sparsehash/src\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/google-sparsehash/gen/arch/$(os_name)/$(arch_name)/include\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/protobuf/src\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/re2/src\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/out/$(BUILDTYPE)/obj/gen\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/apr/src/include/\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/aprutil/src/include/\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/apr/gen/arch/$(os_name)/$(arch_name)/include/\
 -I$(PAGESPEED_INCLUDE_DIR)/third_party/aprutil/gen/arch/$(os_name)/$(arch_name)/include/\
 -I$(PAGESPEED_INCLUDE_DIR)/out/$(BUILDTYPE)/obj/gen\
 -I$(PAGESPEED_INCLUDE_DIR)/out/$(BUILDTYPE)/obj/gen/protoc_out/instaweb

PAGESPEED_LIB=${PAGESPEED_PREFIX}/lib/ats_pagespeed/psol/${BUILDTYPE}/${os_name}/${arch_name}/pagespeed_automatic.a


CXXFLAGS=-std=c++11 -g -Wall ${PAGESPEED_INCLUDES} `pkg-config --cflags protobuf`

LDLIBS=${PAGESPEED_LIB} `pkg-config --libs protobuf` -lpthread

all: ${PROGRAMS}

first.SRCS=first.cc
second.SRCS=second.cc

.SECONDEXPANSION:
${PROGRAMS}: % : $${$$@.SRCS}
	${CXX} ${CXXFLAGS} -o $@ $^ ${LDLIBS}

.PHONY: all

clean:
	rm -f ${PROGRAMS} *.o *~
