CC        = g++
CXXFLAGS  = -std=c++11 -Wall -Wextra -I. -g -ggdb

LINKER    = g++
LFLAGS    = -Wall -I. -lm -lSDL2 -lGL -ldl

SRCDIR    = src
OBJDIR    = src

SOURCES  := $(wildcard $(SRCDIR)/*.cpp) \
            $(wildcard $(SRCDIR)/mbc/*.cpp) \
            $(wildcard $(SRCDIR)/gui/*.cpp) \
            $(wildcard lib/imgui/*.cpp) \
            lib/imgui/examples/imgui_impl_sdl.cpp \
            lib/imgui/examples/imgui_impl_opengl3.cpp
SOURCES  := $(filter-out $(SRCDIR)/lib/imgui//imgui_demo.cpp, $(SOURCES))
SOURCES  := $(filter-out $(SRCDIR)/main.cpp, $(SOURCES))
SOURCES  := $(filter-out $(SRCDIR)/test.cpp, $(SOURCES))

INCLUDES := -Ilib/imgui \
            -Ilib/imgui_club/ \
            -Ilib/imgui/examples \
            -Ilib/imgui/examples/libs/gl3w \
            -I/usr/include/SDL2

CXXFLAGS += $(INCLUDES)

OBJECTS       := $(patsubst %.cpp, %.o, $(SOURCES))
OBJECTS_C     := lib/imgui/examples/libs/gl3w/GL/gl3w.o
DMG_OBJECTS   := $(OBJECTS) $(OBJECTS_C) $(OBJDIR)/main.o
TEST_OBJECTS  := $(OBJECTS) $(OBJECTS_C) $(OBJDIR)/test.o

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

$(OBJECTS_C): %.o : %.c
	$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "Compiled "$<" successfully!"

.PHONY: clean
clean:
ifneq (,$(wildcard $(OBJDIR)/*.o))
	@rm $(OBJDIR)/*.o
	@rm $(OBJDIR)/gui/*.o
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
