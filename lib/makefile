ÑXX = g++
OBJECTS_DIR = ../bin
SOURCES = 	$(wildcard *.cpp)

FLAGS = - -std=c++11 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs \
		-Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security \
		-Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo \
		-Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE

OBJECT_FILES = $(addsuffix .o,$(basename $(SOURCES)))


all: $(OBJECT_FILES)



%.o: %.cpp
	$(CXX) -c $< -o $(OBJECTS_DIR)/$@

clean:
	rm -rf *.exe
	rm -rf *.o