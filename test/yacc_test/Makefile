NAME = mini
TEST_FILE = test.gc

CXXFLAGS = -std=c++14 $(NO_WARNING)

all: ${NAME}

parser.cpp: ${NAME}.y
	bison -d -o parser.cpp ${NAME}.y --report=all

parser.hpp: parser.cpp

tokenizer.cpp: ${NAME}.l
	flex -o tokenizer.cpp ${NAME}.l

%.o: %.cpp
	g++ -c $(CXXFLAGS) -g -o $@ $<

${NAME}: parser.o tokenizer.o
	g++ -o $@ parser.o tokenizer.o

clean:
	rm -f parser.cpp parser.hpp tokenizer.cpp
	rm -f parser.output
	rm -f *.o
	rm ${NAME}
run:
	./${NAME} ${TEST_FILE}