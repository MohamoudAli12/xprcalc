CC = gcc
CFLAGS =-Wall -Wextra -Wpedantic -Wshadow -Wconversion \
         -Wsign-conversion -Wformat=2 -Wundef \
         -Wnull-dereference -Wdouble-promotion \
         -fsanitize=address,undefined -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address,undefined -lm
LDFLAGS_TEST = -lcriterion -lm

# Files
APP_SRCS = xprcalc.c main.c 
TEST_SRCS = xprcalc.c test.c  

# Output binaries
APP_TARGET = xprcalc 
TEST_TARGET = xprcalc_test

# Default target
all: $(APP_TARGET)
app: $(APP_TARGET)
	./$(APP_TARGET)

# Build the application
$(APP_TARGET): $(APP_SRCS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ -O3 -march=native

# Build the test binary
$(TEST_TARGET): $(TEST_SRCS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS_TEST)

# Run tests
test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Clean build artifacts
clean:
	rm -f $(APP_TARGET) $(TEST_TARGET)

