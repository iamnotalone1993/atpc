#The input file of your program
IN = barrier_pthread

#The main file name of your program
OUT = out

#The compiler
CC = g++

#The performance flags for the compiler
CFLAGS = -std=c++17 -O3 -pthread -fopenmp

.PHONY: all run clean

#Compile your program
all : $(OUT)

$(OUT) : $(IN).cpp
	$(CC) $(IN).cpp -o $(OUT) $(CFLAGS)

#Run your program
run : $(OUT)
	for i in `seq 10`; do ./$(OUT); done

#Remove your executable
clean :
	rm -f $(OUT)

