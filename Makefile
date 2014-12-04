OBJECTS=calenderServer.o event.o user.o group.o objectCache.o serverFiles.o serverActions.o persistentState.o
COMPILE=g++ -g -std=c++11 -Wall -c
LINK=g++ -g -std=c++11

all: calenderServer

calenderServer: $(OBJECTS)
	$(LINK) -o calenderServer $(OBJECTS)

calenderServer.o: calenderServer.cpp calenderServer.h serverFiles.h serverActions.h persistentState.h
	$(COMPILE) -o calenderServer.o calenderServer.cpp

event.o: event.cpp event.h
	$(COMPILE) -o event.o event.cpp

user.o: user.cpp user.h
	$(COMPILE) -o user.o user.cpp

group.o: group.cpp group.h
	$(COMPILE) -o group.o group.cpp

objectCache.o: objectCache.cpp objectCache.h serverFiles.h
	$(COMPILE) -o objectCache.o objectCache.cpp

serverFiles.o: serverFiles.cpp serverFiles.h
	$(COMPILE) -o serverFiles.o serverFiles.cpp

serverActions.o: serverActions.cpp serverActions.h group.h event.h user.h
	$(COMPILE) -o serverActions.o serverActions.cpp

persistentState.o: persistentState.cpp persistentState.h objectCache.h serverFiles.h group.h event.h user.h
	$(COMPILE) -o persistentState.o persistentState.cpp
