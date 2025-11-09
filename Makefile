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
OBJDIR = obj
BINDIR = bin
LIBSDIR = libs
REPORTSDIR = reports
TARGET = loxc

# --- Library Configuration ---
LIB_NAME = libmonstera
LIB_SUBMODULE_DIR = external/$(LIB_NAME)
# ライブラリのヘッダファイルがあるディレクトリを指定
LIB_INCDIR = $(LIB_SUBMODULE_DIR)/src

# --- Auto-detection of Files ---
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
DEPS = $(OBJECTS:.o=.d)
EXECUTABLE = $(BINDIR)/$(TARGET)
SHARED_LIB_PATH = $(LIBSDIR)/lib$(LIB_NAME).so

# --- Compiler & Linker Flags ---
# Add library's include path
CPPFLAGS = -I$(SRCDIR) -I$(LIB_INCDIR)
CFLAGS += -MMD -MP

# Add library path and name for linker.
# The rpath option embeds the library path in the executable for easy running.
LDFLAGS = -L$(LIBSDIR) -l$(LIB_NAME) -Wl,-rpath,'$$ORIGIN/../$(LIBSDIR)'

# --- Rules ---
.PHONY: all clean distclean run check valgrind library

# Default goal: build the executable. This depends on the shared library being ready.
all: $(EXECUTABLE)
	@echo "==> Build complete ($(BUILD_TYPE) mode)."

# Rule to build just the library by calling its own Makefile
# This is a prerequisite for the executable.
$(SHARED_LIB_PATH):
	@echo "==> Building dependent library: $(LIB_NAME)..."
	@$(MAKE) -C $(LIB_SUBMODULE_DIR) all \
		DEBUG=$(DEBUG) \
		WERROR=$(WERROR) \
		LIBSDIR_OUT=$(abspath $(LIBSDIR))

# Rule to link the final executable, depends on application objects and the shared library.
$(EXECUTABLE): $(OBJECTS) $(SHARED_LIB_PATH)
	@echo "==> Linking to create $@..."
	@mkdir -p $(BINDIR)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

# Rule to compile a .c source file into a .o object file
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo "==> Compiling $< ($(BUILD_TYPE) mode)..."
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

-include $(DEPS)

# Rule to clean up all obj files and call the library's clean target
clean:
	@echo "==> Cleaning up application object files..."
	@rm -rf $(OBJDIR)
	@$(MAKE) -C $(LIB_SUBMODULE_DIR) clean

# Rule to clean up everything
distclean:
	@echo "==> Cleaning up project..."
	@rm -rf $(OBJDIR) $(BINDIR) $(REPORTSDIR) $(LIBSDIR)
	@$(MAKE) -C $(LIB_SUBMODULE_DIR) distclean

# Rule to run the executable
run: all
	@echo "==> Running executable..."
	./$(EXECUTABLE) $(ARGS)

# --- Static Analysis ---
check:
	@echo "==> Running static analysis with Cppcheck..."
	@mkdir -p $(REPORTSDIR)
	cppcheck --enable=all --inconclusive --std=c11 --suppress=missingIncludeSystem -I$(SRCDIR) -I$(LIB_INCDIR) $(SRCDIR) 2> $(REPORTSDIR)/cppcheck.log

# --- Dynamic Analysis ---
valgrind:
	@echo "==> Rebuilding in Debug mode for Valgrind analysis..."
	$(MAKE) distclean
	$(MAKE) all DEBUG=1
	@echo "==> Running dynamic analysis with Valgrind..."
	@mkdir -p $(REPORTSDIR)
	valgrind --log-file=$(REPORTSDIR)/valgrind.log --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(EXECUTABLE) $(ARGS)