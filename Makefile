# --- Variables ---
CC = gcc
# Base flags for all builds. This is a very strict set.
# Use '\' for readability.
BASE_CFLAGS = \
    -Wall \
    -Wextra \
    -pedantic \
    -Wshadow \
    -Wconversion \
    -Wsign-compare \
    -Wformat=2 \
    -Wundef \
    -Wunreachable-code \
    -Wlogical-op \
    -Wfloat-equal \
    -Wstrict-prototypes \
    -Wmissing-prototypes \
    -Wmissing-declarations \
    -Wredundant-decls \
    -Wattributes \
    -Wbad-function-cast \
    -Winline \
	-Wdeclaration-after-statement
    # -Wno-unused-parameter # Uncomment if you have functions with intentionally unused parameters (e.g., callbacks)
CPPFLAGS = -Iinclude
LDFLAGS =

# --- Build Configuration (Debug vs Release) ---
ifeq ($(DEBUG), 1)
    CFLAGS = $(BASE_CFLAGS) -g -O0 -std=c11
    BUILD_TYPE = Debug
else
    CFLAGS = $(BASE_CFLAGS) -O2 -std=c11
    BUILD_TYPE = Release
endif

# --- Optional: Treat warnings as errors ---
# Use 'make WERROR=1' to enable this
ifeq ($(WERROR), 1)
    CFLAGS += -Werror
endif

# --- Project Structure ---
SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin
REPORTSDIR = reports
TARGET = loxc

# --- Auto-detection of Files ---
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
DEPS = $(OBJECTS:.o=.d)
CFLAGS += -MMD -MP
EXECUTABLE = $(BINDIR)/$(TARGET)

# --- Rules ---
.PHONY: all clean run check valgrind

# Default goal: build the executable.
all: $(EXECUTABLE)
	@echo "==> Build complete ($(BUILD_TYPE) mode)."

# Rule to link the final executable
$(EXECUTABLE): $(OBJECTS)
	@echo "==> Linking to create $@..."
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $^

# Rule to compile a .c source file into a .o object file
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "==> Compiling $< ($(BUILD_TYPE) mode)..."
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)

# Rule to clean up all obj files
clean:
	@echo "==> Cleaning up obj files..."
	@rm -rf $(OBJDIR)

distclean:
	@echo "==> Cleaning up project..."
	@rm -rf $(OBJDIR) $(BINDIR) $(REPORTSDIR)

# Rule to run the executable
run: all
	@echo "==> Running executable..."
	./$(EXECUTABLE) $(ARGS)

# --- Static Analysis ---
check:
	@echo "==> Running static analysis with Cppcheck..."
	@mkdir -p $(REPORTSDIR)
	cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem -I$(INCDIR) $(SRCDIR) 2> $(REPORTSDIR)/cppcheck.log

# --- Dynamic Analysis ---
valgrind: clean
	@echo "==> Rebuilding in Debug mode for Valgrind analysis..."
	$(MAKE) all DEBUG=1
	@echo "==> Running dynamic analysis with Valgrind..."
	@mkdir -p $(REPORTSDIR)
	valgrind --log-file=$(REPORTSDIR)/valgrind.log --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXECUTABLE) $(ARGS)
