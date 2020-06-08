# Makefile for building MUSHROOM_EX engine

# Options
LINUX_BUILD ?= 0
MACOS_BUILD ?= 0
WIN64_BUILD ?= 0

DEBUG_BUILD ?= 0
DEBUG_SPECTATOR ?= 0

# Try to automatically select platform if none specified
ifneq ($(filter $(LINUX_BUILD) $(MACOS_BUILD) $(WIN64_BUILD), 1), 1)
	ifeq ($(OS),Windows_NT)
		WIN64_BUILD := 1
	else
		UNAME_S := $(shell uname -s)
		ifeq ($(UNAME_S),Linux)
		    LINUX_BUILD := 1
		endif

		ifeq ($(UNAME_S),Darwin)
		    MACOS_BUILD := 1
		endif
	endif
endif

ifeq ($(WIN64_BUILD), 1)
	$(error Building for Windows not yet supported.)
endif

# OS variables. Building on Linux is assumed default.
DYNAMIC_LIB_EXTENSION := so
STATIC_LIB_EXTENSION  := a

ifeq ($(MACOS_BUILD), 1)
	DYNAMIC_LIB_EXTENSION := dylib
endif

# Commands
CC  := clang
CXX := clang++
LD  := clang++

PYTHON  := python
PREMAKE := premake4

CMAKE  := cmake
MAKE   := make
MKDIR  := mkdir -p
FIND   := find
CP_DIR := cp -r
RM_DIR := rm -r
CP     := cp
CD     := cd
RM     := rm
MV     := mv

# Directories
SRC_DIR     := src
BUILD_DIR   := build
BIN_DIR     := bin
LIB_DIR     := lib
BIN_LIB_DIR := bin/$(LIB_DIR)
INCLUDE_DIR := include

ASSIMP_DIR       := tools/assimp
ASSIMP_LIB_NAME  := libassimp.*
ASSIMP_LIB       := $(ASSIMP_DIR)/bin/$(ASSIMP_LIB_NAME)

GL3W_DIR         := tools/gl3w
GL3W_SRC_NAME    := gl3w.c
GL3W_SRC         := $(GL3W_DIR)/src/$(GL3W_SRC_NAME)

GLM_DIR          := tools/glm

SOIL2_DIR        := tools/SOIL2
SOIL2_LIB_NAME   := libsoil2.$(DYNAMIC_LIB_EXTENSION)
SOIL2_LIB        := $(SOIL2_DIR)/lib/linux/$(SOIL2_LIB_NAME)

SOIL2_MAKE_DIR   := $(SOIL2_DIR)/make/linux
PATCH_SOIL2_LIB_COMMAND :=

YAMLCPP_DIR      := tools/yaml-cpp
YAMLCPP_LIB_NAME := libyaml-cpp*.$(DYNAMIC_LIB_EXTENSION)
YAMLCPP_LIB      := $(YAMLCPP_DIR)/build/$(YAMLCPP_LIB_NAME)

ifeq ($(MACOS_BUILD), 1)
	SOIL2_LIB        := $(SOIL2_DIR)/lib/macosx/$(SOIL2_LIB_NAME)
	SOIL2_MAKE_DIR   := $(SOIL2_DIR)/make/macosx
	PATCH_SOIL2_LIB_COMMAND := install_name_tool -id @rpath/$(SOIL2_LIB_NAME) $(LIB_DIR)/$(SOIL2_LIB_NAME)
endif

# Sources
C_SRCS   := $(shell $(FIND) $(SRC_DIR) -name *.c)
CXX_SRCS := $(shell $(FIND) $(SRC_DIR) -name *.cpp)
OBJS     := $(foreach file, $(CXX_SRCS), $(BUILD_DIR)/$(file).o) \
		    $(foreach file, $(C_SRCS), $(BUILD_DIR)/$(file).o)

# Libraries
LD_FLAGS := -L$(LIB_DIR) -rpath 'lib' -lc -lstdc++ -lsoil2 -lglfw -ldl -lassimp -lyaml-cpp

ifeq ($(MACOS_BUILD), 1)
	LD_FLAGS += -framework CoreVideo -framework OpenGL -framework IOKit -framework Cocoa -framework Carbon
endif

# C Flags
C_FLAGS   := -I$(INCLUDE_DIR) -Wall
CXX_FLAGS := -I$(INCLUDE_DIR) -Wall -std=c++11

# Target exe
TARGET_EXE := mushroom_ex

# Handle options
ifeq ($(DEBUG_BUILD), 1)
	C_FLAGS   += -g -DDEBUG_BUILD
	CXX_FLAGS += -g -DDEBUG_BUILD
	LD_FLAGS  += -g -DDEBUG_BUILD
endif

ifeq ($(DEBUG_SPECTATOR), 1)
	C_FLAGS   += -DDEBUG_SPECTATOR
	CXX_FLAGS += -DDEBUG_SPECTATOR
	LD_FLAGS  += -DDEBUG_SPECTATOR
endif

ifeq ($(WIN64_BUILD), 1)
	TARGET_EXE := mushroom_ex.exe
	C_FLAGS    += -DWIN64_BUILD
	CXX_FLAGS  += -DWIN64_BUILD
endif

ifeq ($(LINUX_BUILD), 1)
	C_FLAGS    += -DLINUX_BUILD
	CXX_FLAGS  += -DLINUX_BUILD
endif

ifeq ($(MACOS_BUILD), 1)
	C_FLAGS    += -DMACOS_BUILD
	CXX_FLAGS  += -DMACOS_BUILD
endif


# Rules for building MushroomEX

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
	$(MKDIR) $(INCLUDE_DIR)
	$(MKDIR) $(INCLUDE_DIR)/assimp
	$(MKDIR) $(INCLUDE_DIR)/glm
	$(MKDIR) $(INCLUDE_DIR)/SOIL2
	$(MKDIR) $(LIB_DIR)
	$(MKDIR) $(BUILD_DIR)
	$(MKDIR) $(BIN_DIR)
	$(MKDIR) $(BIN_LIB_DIR)

clean:
	-$(RM_DIR) $(BUILD_DIR)


# Rules for handling support libraries

build_libs: create_dirs build_assimp build_gl3w build_glm build_SOIL2 build_yamlcpp
	$(CP) $(LIB_DIR)/* $(BIN_LIB_DIR)/

build_assimp:
	$(CD) $(ASSIMP_DIR) && $(CMAKE) CMakeLists.txt
	$(CD) $(ASSIMP_DIR) && $(MAKE)
	$(CP_DIR) $(ASSIMP_DIR)/include/assimp/* $(INCLUDE_DIR)/assimp/
	$(CP_DIR) $(ASSIMP_LIB) $(LIB_DIR)/

build_gl3w:
	$(CD) $(GL3W_DIR) && $(PYTHON) gl3w_gen.py
	$(CP_DIR) $(GL3W_DIR)/include/* $(INCLUDE_DIR)/
	$(CP) $(GL3W_SRC) $(SRC_DIR)/

build_glm:
	$(CP_DIR) $(GLM_DIR)/glm/* $(INCLUDE_DIR)/glm/

build_SOIL2:
	$(CD) $(SOIL2_DIR) && $(PREMAKE) gmake
	$(CD) $(SOIL2_MAKE_DIR) && $(MAKE) config=release
	$(CP_DIR) $(SOIL2_DIR)/src/SOIL2/SOIL2.h $(INCLUDE_DIR)/SOIL2/
	$(CP_DIR) $(SOIL2_LIB) $(LIB_DIR)/
	$(PATCH_SOIL2_LIB_COMMAND)

build_yamlcpp:
	$(MKDIR) $(YAMLCPP_DIR)/build
	$(CD) $(YAMLCPP_DIR)/build && $(CMAKE) -DYAML_BUILD_SHARED_LIBS=ON ..
	$(CD) $(YAMLCPP_DIR)/build && $(MAKE)
	$(CP) $(YAMLCPP_LIB) $(LIB_DIR)
	$(CP_DIR) $(YAMLCPP_DIR)/include/yaml-cpp $(INCLUDE_DIR)


clean_libs: clean_lib_dir clean_assimp clean_gl3w clean_glm clean_SOIL2 clean_yamlcpp

clean_lib_dir:
	-$(RM) $(LIB_DIR)/*
	-$(RM) $(BIN_LIB_DIR)/*

clean_assimp:
	-$(CD) $(ASSIMP_DIR) && $(MAKE) clean
	-$(RM_DIR) $(INCLUDE_DIR)/assimp
	-$(RM) $(LIB_DIR)/$(ASSIMP_LIB_NAME)

clean_gl3w:
	-$(RM_DIR) $(INCLUDE_DIR)/GL
	-$(RM_DIR) $(INCLUDE_DIR)/KHR
	-$(RM) $(SRC_DIR)/$(GL3W_SRC_NAME)

clean_glm:
	-$(RM_DIR) $(INCLUDE_DIR)/glm

clean_SOIL2:
	-$(RM_DIR) $(INCLUDE_DIR)/SOIL2
	-$(RM) $(LIB_DIR)/$(SOIL2_LIB_NAME)

clean_yamlcpp:
	-$(RM_DIR) $(YAMLCPP_DIR)/build
	-$(RM) $(LIB_DIR)/$(YAMLCPP_LIB_NAME)
	-$(RM_DIR) $(INCLUDE_DIR)/yaml-cpp
