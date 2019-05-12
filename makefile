CC       = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -ggdb

LINKER   = g++
LFLAGS   = -Wall -I. -lm -lSDL2

SRCDIR   = src
OBJDIR   = src

SOURCES  := $(wildcard $(SRCDIR)/*.cpp) \
            $(wildcard $(SRCDIR)/mbc/*.cpp)
SOURCES  := $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
SOURCES  := $(filter-out $(SRCDIR)/test.cpp, $(SOURCES))
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(patsubst %.cpp, %.o, $(SOURCES))
DMG_OBJECTS  := $(OBJECTS) $(OBJDIR)/main.o
TEST_OBJECTS  := $(OBJECTS) $(OBJDIR)/test.o

BUILD := rel
ifeq ($(BUILD),devel)
CXXFLAGS += -DDEBUG
endif

TEST := no
ifeq ($(BUILD),yes)
CXXFLAGS += -DTEST
endif

all: dmg test

dmg: CXXFLAGS +=
dmg: $(DMG_OBJECTS)
	$(LINKER) $(DMG_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking dmg complete!"

test: CXXFLAGS +=
test: $(TEST_OBJECTS)
	$(LINKER) $(TEST_OBJECTS) $(LFLAGS) -o $@
	@echo "Linking test complete!"

$(OBJECTS): %.o : %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
ifneq (,$(wildcard $(OBJDIR)/*.o))
	@rm $(OBJDIR)/*.o
	@rm $(OBJDIR)/mbc/*.o
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
