CXXFLAGS = -std=c++11 -O0 -g -Wall -fmessage-length=0

OBJS = mfMain.o mf.o subjectMerge.o generateAlign.o parse.o \
		convert.o classify.o globalSeg.o lenStatistics.o accuracyStatistics.o \
		util.o reads.o queryIndex.o map.o misass.o indel.o sv.o misinfo.o correct.o \
		circosData.o misReg.o

LIBS += -lm -lpthread

TARGET = mf

all: $(TARGET) clean

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) 

clean:
	rm -f $(OBJS)
	
clean-all: clean
	rm -f $(TARGET)
