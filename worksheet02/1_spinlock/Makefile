# The main file name of your program
OUT = spinlock

# The compiler
CC = g++

# The performance flags for the compiler
FLAGS = -pthread -std=gnu++17 -O3

.PHONY: all run clean

# Compile your program
all : $(OUT)

$(OUT) : $(OUT).cpp
	$(CC) $(OUT).cpp -o $(OUT) $(FLAGS)

# Run your program
run : $(OUT)
	for i in `seq 1`; do ./$(OUT); done

# Remove your executable
clean :
	rm -f $(OUT)

