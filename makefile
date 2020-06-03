# Makefile for building MUSHROOM_EX engine

# Options
DEBUG_BUILD ?= 0

# Commands
CC  := clang
CXX := clang++
LD  := clang++

PYTHON := python

CMAKE  := cmake
make   := make
MKDIR  := mkdir -p
CP_DIR := cp -r
RM_DIR := rm -r
CP     := cp
CD     := cd
RM     := rm

# Directories
SRC_DIR   := src
OBJ_DIR   := obj
BUILD_DIR := build
BIN_DIR   := bin

ASSIMP_DIR := tools/assimp
ASSIMP_LIB := $(ASSIMP_DIR)/bin/libassimp.so

GL3W_DIR := tools/gl3w
GL3W_SRC := $(GL3W_DIR)/src/gl3w.c

GLM_DIR := tools/glm

# Sources
C_SRCS   := $(shell find $(SRC_DIR) -name *.c)
CXX_SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS     := $(foreach file, $(CXX_SRCS), $(BUILD_DIR)/$(file).o) \
		    $(foreach file, $(C_SRCS), $(BUILD_DIR)/$(file).o)

# Libraries
LD_FLAGS := -Llib
LD_FLAGS += -lc -lstdc++ -lglfw -ldl

# C Flags
C_FLAGS   := -Iinclude -Wall
CXX_FLAGS := -Iinclude -Wall

ifeq ($(DEBUG_BUILD), 1)
	C_FLAGS   += -g
	CXX_FLAGS += -g
	LD_FLAGS  += -g
endif

# Target exe
TARGET_EXE = mushroom_ex

all: create_dirs $(TARGET_EXE)

$(TARGET_EXE): $(OBJS)
	$(LD) $(LD_FLAGS) $(OBJS) -o $(BUILD_DIR)/$(TARGET_EXE)
	$(CP) $(BUILD_DIR)/$(TARGET_EXE) $(BIN_DIR)/$(TARGET_EXE)

$(BUILD_DIR)/%.cpp.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CXX_FLAGS) -c $< -o $@

$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR) $(dir $@)
	$(CC) $(C_FLAGS) -c $< -o $@

create_dirs:
	$(MKDIR) include
	$(MKDIR) include/assimp
	$(MKDIR) include/glm
	$(MKDIR) lib
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(BIN_DIR)

clean:
	-$(RM_DIR) $(BUILD_DIR)

build_libs: create_dirs build_assimp build_gl3w build_glm

build_assimp:
	$(CD) $(ASSIMP_DIR) && $(CMAKE) CMakeLists.txt
	$(CD) $(ASSIMP_DIR) && $(MAKE)
	$(CP_DIR) $(ASSIMP_DIR)/include/assimp/* include/assimp/
	$(CP) $(ASSIMP_LIB) lib/

build_gl3w:
	$(CD) $(GL3W_DIR) && $(PYTHON) gl3w_gen.py
	$(CP_DIR) $(GL3W_DIR)/include/* include/
	$(CP) $(GL3W_SRC) src/

build_glm:
	$(CP_DIR) $(GLM_DIR)/glm/* include/glm/

clean_libs: clean_assimp clean_gl3w clean_glm

clean_assimp:
	-$(CD) tools/assimp && $(MAKE) clean
	-$(RM_DIR) include/assimp
	-$(RM) $(ASSIMP_LIB)

clean_gl3w:
	-$(RM_DIR) include/GL
	-$(RM_DIR) include/KHR
	-$(RM) src/gl3w.c

clean_glm:
	-$(RM_DIR) include/glm