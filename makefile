SRC  := $(addprefix /,$(wildcard *.cpp)) $(addprefix /,$(wildcard *.c)) #$(shell find . -name '*.cpp')
HDR := $(shell find . -name '*.h')
SRC_NAME := $(subst ./,/,$(SRC))
OBJS  := $(addprefix obj, $(SRC_NAME:.cpp=.o)) 
OBJS  := $(OBJS:.c=.co)

DEPS := $(addprefix obj, $(SRC_NAME:.cpp=.d)) 
DEPS := $(DEPS:.c=.d)
INCLUDE := -Iinclude
LIB := -Llib -lfreeglut -lopengl32 -lgdi32 -lwinmm
FLAGS := -g -fpermissive -Wextra -MMD -MP -w -DUNICODE -std=c++20 -static-libgcc -static-libstdc++ #-fcompare-debug-second -Wl,--subsystem,windows
ARGS := $(FLAGS)
TARGET := auto
.PHONY: all clear clear_all

all: build/$(TARGET).exe

rebuild: clear all

build/$(TARGET).exe:  $(OBJS)
	@mkdir -p build
	g++ $(ARGS) $(OBJS) -o build/$(TARGET).exe $(LIB)

obj/%.co: %.c
	@mkdir -p $(dir $@)
	gcc $(ARGS) $(INCLUDE) -c $< -o $@

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	g++ $(ARGS) $(INCLUDE) -c $< -o $@

-include $(DEPS)

clear: 
	rm -f $(OBJS)
	rm -f $(DEPS)

clear_all: clear
	rm -f build/$(TARGET).exe