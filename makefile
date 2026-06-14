SRC  := $(addprefix /,$(wildcard *.cpp)) $(addprefix /,$(wildcard *.c)) #$(shell find . -name '*.cpp')
HDR := $(shell find . -name '*.h')
SRC_NAME := $(subst ./,/,$(SRC))
OBJS  := $(addprefix obj, $(SRC_NAME:.cpp=.o)) 
OBJS  := $(OBJS:.c=.co)

DEPS := $(addprefix obj, $(SRC_NAME:.cpp=.d)) 
DEPS := $(DEPS:.c=.d)
INCLUDE := -Iinclude
LIB := -L./lib -lfreeglut -lopengl32 -lgdi32 -lwinmm
CFLAGS_COMMON := -O3 -fpermissive -Wextra -MMD -MP -w -DFREEGLUT_STATIC -fopenmp -static -static-libgcc -static-libstdc++
CFLAGS_C := $(CFLAGS_COMMON) -std=c11
CFLAGS_CXX := $(CFLAGS_COMMON) -std=c++17
TARGET := auto
.PHONY: all clear clear_all

all: build/$(TARGET).exe

rebuild: clear all

build/$(TARGET).exe:  $(OBJS)
	@mkdir -p build
	g++ $(CFLAGS_CXX) $(OBJS) -o build/$(TARGET).exe $(LIB)

obj/%.co: %.c
	@mkdir -p $(dir $@)
	gcc $(CFLAGS_C) $(INCLUDE) -c $< -o $@

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	g++ $(CFLAGS_CXX) $(INCLUDE) -c $< -o $@

-include $(DEPS)

clear: 
	rm -f $(OBJS)
	rm -f $(DEPS)

clear_all: clear
	rm -f build/$(TARGET).exe