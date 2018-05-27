TARGET   = dmg

CC       = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I.

LINKER   = g++
LFLAGS   = -Wall -I. -lm

SRCDIR   = src
OBJDIR   = src
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(OBJDIR)/utils.o $(OBJDIR)/mmu.o $(OBJDIR)/cpu.o
DMG_OBJECTS  := $(OBJECTS) $(OBJDIR)/dmg.o
TEST_OBJECTS  := $(OBJECTS) $(OBJDIR)/test.o

all: $(BINDIR)/dmg $(BINDIR)/test

$(BINDIR)/dmg: CXXFLAGS += -DDEBUG
$(BINDIR)/dmg: $(DMG_OBJECTS)
	@$(LINKER) $(DMG_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking dmg complete!"

$(BINDIR)/test: CXXFLAGS += -DTEST
$(BINDIR)/test: $(TEST_OBJECTS)
	@$(LINKER) $(TEST_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking test complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
	@$(rm) $(OBJDIR)/*.o
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/dmg
	@echo "Executable removed!"
