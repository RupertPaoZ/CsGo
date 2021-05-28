<<<<<<< HEAD
NAME = mini

LLVM_CONFIG = /usr/lib/llvm-10/bin/llvm-config

NO_WARNING =  -Wno-return-type \
	-Wno-switch \

CXXFLAGS =  -std=c++14 $(NO_WARNING)

OBJS = parser.o tokenizer.o ast.o

all : $(NAME)

parser.cpp: ${NAME}.y
	bison -d -o parser.cpp ${NAME}.y

parser.hpp: parser.cpp

tokenizer.cpp: ${NAME}.l
	flex -o tokenizer.cpp ${NAME}.l

%.o: %.cpp ast.h
	g++ -c $(CXXFLAGS) -g -o $@ $< 

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS)

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f parser.hpp parser.cpp tokenizer.cpp mini
	-rm -f *.json
	-rm -f *.s
	-rm -f *.ll
=======
NAME = mini

LLVM_CONFIG = /usr/lib/llvm-10/bin/llvm-config

NO_WARNING =  -Wno-return-type \
	-Wno-switch \

CXXFLAGS =  -std=c++14 $(NO_WARNING)

OBJS = parser.o tokenizer.o ast.o

all : $(NAME)

parser.cpp: ${NAME}.y
	bison -d -o parser.cpp ${NAME}.y

parser.hpp: parser.cpp

tokenizer.cpp: ${NAME}.l
	flex -o tokenizer.cpp ${NAME}.l

%.o: %.cpp ast.h
	g++ -c $(CXXFLAGS) -g -o $@ $< 

$(NAME): $(OBJS)
	g++ -o $@ $(OBJS)

.PHONY: clean
clean:
	-rm -f *.o
	-rm -f parser.hpp parser.cpp tokenizer.cpp mini
	-rm -f *.json
	-rm -f *.s
	-rm -f *.ll
>>>>>>> main
