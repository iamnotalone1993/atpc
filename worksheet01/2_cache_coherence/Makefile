# The main file name of your program
OUT = test

# Compiler
CC = mpic++

# The performance flags for the compiler
FLAGS = -std=gnu++17 -O3

.PHONY: all run clean

# Compile your program
all : $(OUT)

$(OUT) : $(OUT).cpp
	$(CC) $(OUT).cpp -o $(OUT) $(FLAGS)

# Run your program
run : $(OUT)
	mpirun -np 4 ./$(OUT)

# Remove your executable
clean :
	rm -f $(OUT)

