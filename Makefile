CXX=g++
RM=rm -f

EXTL=m

SRCS=fast_diff.cpp
SRCSC=uni_corr.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: mainSim

mainSim: $(SRCS)
	@mkdir -p bin
	$(CXX) -std=c++11 $(SRCS) -O2 -Wall -o bin/fast_diff
	$(CXX) -std=c++11 $(SRCSC) -O2 -Wall -o bin/uni_corr

clean:
	rm -f bin/*
	rmdir bin
