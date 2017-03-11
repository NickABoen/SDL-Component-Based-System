#OBJS specifies which files to compile as part of the project
OBJS = main.cpp

#CC specifies which compiler we're using
CC = g++ --std=c++14

#COMPILER_FLAGS specifies th eadditional compilation options we're using
#-W SUPPRESSES ALL WARNINGS
COMPILER_FLAGS = -w

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lSDL2 -lSDL2_image

#OBJ_NAME specifies th ename of our executable
OBJ_NAME = program.out

#This is the target that compiles our executable
all: reset $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

reset:
	reset

run: all
	./$(OBJ_NAME)
