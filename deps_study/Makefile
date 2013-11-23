CXX      = g++
CXXFLAGS = -Wall -O3
LDFLAGS  =

TARGET = testcpp
SRCS   = main.cc x.cc foo.cc
OBJS   = $(SRCS:.cc=.o)
DEPS   = $(SRCS:.cc=.depends)


.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

.cc.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.depends: %.cc
	$(CXX) -M $(CXXFLAGS) $< > $@

clean:
	 rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)