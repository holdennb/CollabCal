OBJECTS=calenderServer.o event.o user.o group.o
HEADERS=calenderServer.h event.h user.h group.h
COMPILE=g++ -g -std=c++11 -Wall -c
LINK=g++ -g -std=c++11

all: calenderServer

calenderServer: $(OBJECTS)
	$(LINK) -pthread -o calenderServer $(OBJECTS)

calenderServer.o: calenderServer.cpp $(HEADERS)
	$(COMPILE) -pthread -o calenderServer.o calenderServer.cpp

event.o: event.cpp event.h
	$(COMPILE) -o event.o event.cpp

user.o: user.cpp user.h
	$(COMPILE) -o user.o user.cpp

group.o: group.cpp group.h
	$(COMPILE) -o group.o group.cpp
