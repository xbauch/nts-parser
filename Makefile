CXX=clang++
#CXX=g++
#CFLAGS=-Wall -pedantic -g -ferror-limit=5 -DDEBUG
CFLAGSDEBUG=-g -ferror-limit=5 -DDEBUG
CFLAGSRELEASE=-O3
#CFLAGSDEBUG=-Wall -pedantic -g -fmax-errors=5 -DDEBUG
CXXFLAGS:=$(CFLAGSDEBUG) -I. -std=c++1z
#CXXFLAGS:=$(CFLAGSRELEASE) -I. -std=c++1z
TARGET=parser
TOOLSOBJS=$(shell find tools -name '*.cpp' | sed s\/.cpp\$\/.o\/)
TOOLSHEDS=$(shell find tools -name '*.h')
PARSERSOBJS=$(shell find parsers -name '*.cpp' | sed s\/.cpp\$\/.o\/)
PARSERSHEDS=$(shell find parsers -name '*.h')
PCHEDS=$(shell find Parser-Combinators -name '*.h')

.PHONY: all clean

all:		$(TARGET)

$(TARGET):	$(TARGET).o $(TOOLSOBJS) $(PARSEROBJS)
	$(CXX) $(TARGET).o $(TOOLSOBJS) $(PARSEROBJS) $(CXXFLAGS) -o $(TARGET)

$(TARGET).o:	$(TARGET).cpp $(TOOLSHEDS) $(PARSERSHEDS) $(PCHEDS)
	$(CXX) $(TARGET).cpp $(CXXFLAGS) -c -o $(TARGET).o

clean:
	-rm $(shell find tools -name '*.o') $(TARGET) $(TARGET).o
	-rm $(shell find parsers -name '*.o')
