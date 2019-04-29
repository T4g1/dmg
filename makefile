CC       = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -ggdb

LINKER   = g++
LFLAGS   = -Wall -I. -lm

SRCDIR   = src
OBJDIR   = src

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(OBJDIR)/utils.o $(OBJDIR)/mmu.o $(OBJDIR)/cpu.o
DMG_OBJECTS  := $(OBJECTS) $(OBJDIR)/dmg.o
TEST_OBJECTS  := $(OBJECTS) $(OBJDIR)/test.o

all: dmg test

dmg: CXXFLAGS += -DDEBUG
dmg: $(DMG_OBJECTS)
	@$(LINKER) $(DMG_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking dmg complete!"

test: CXXFLAGS += -DTEST
test: $(TEST_OBJECTS)
	@$(LINKER) $(TEST_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking test complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
ifneq (,$(wildcard $(OBJDIR)/*.o))
	@rm $(OBJDIR)/*.o
endif
	@echo "Cleanup complete!"

.PHONY: remove
remove:
ifneq (,$(wildcard dmg))
	@rm dmg
endif
ifneq (,$(wildcard test))
	@rm test
endif
	@echo "Executable removed!"
