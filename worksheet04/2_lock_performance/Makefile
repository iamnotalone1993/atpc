#The main file name of your program
OUT = main

#The performance flags for the compiler
FLAGS = -pthread -std=gnu++17 -O3

.PHONY: all run clean

#Compile your program
all : $(OUT)

$(OUT) : $(OUT).cpp config.h lock.h
	g++ $(OUT).cpp -o $(OUT) $(FLAGS)

#Run your program
run : $(OUT)
	for i in `seq 10`; do ./$(OUT); done

#Remove your executable
clean :
	rm -f $(OUT)

