SOURCES = main.cpp client.cpp utils.cpp
EXECUTABLE = ipkcpc

ifdef OS
	# If the project is being compiled on windows, add getopt.cpp
	SOURCES += getopt.cpp
endif

build:
	g++ $(SOURCES) -o $(EXECUTABLE)

clean:
	rm $(EXECUTABLE)
