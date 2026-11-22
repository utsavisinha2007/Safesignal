CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
INCLUDES = -Iinclude -Iserver

ifeq ($(OS),Windows_NT)
    LDFLAGS = -lws2_32
    TARGET = SafeSignalServer.exe
else
    LDFLAGS = -lpthread
    TARGET = SafeSignalServer
endif

SRCS = server/server_main.cpp \
       server/api_handlers.cpp \
       src/CheckInManager.cpp \
       src/Contact.cpp \
       src/Event.cpp \
       src/Incident.cpp \
       src/JourneyManager.cpp \
       src/ResponseManager.cpp \
       src/RiskAnalyzer.cpp \
       src/SafetySystem.cpp \
       src/VerificationManager.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) $(INCLUDES) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)
