# Makefile for building MUSHROOM_EX engine

# Options
DEBUG_BUILD ?= 0

# Commands
CC  := clang
CXX := clang++
LD  := clang++

PYTHON  := python
PREMAKE := premake4

CMAKE  := cmake
MAKE   := make
MKDIR  := mkdir -p
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
INCLUDE_DIR := include

ASSIMP_DIR := tools/assimp
ASSIMP_LIB := $(ASSIMP_DIR)/bin/libassimp.*

GL3W_DIR := tools/gl3w
GL3W_SRC := $(GL3W_DIR)/src/gl3w.c

GLM_DIR := tools/glm

SOIL2_DIR      := tools/SOIL2
SOIL2_LIB      := $(SOIL2_DIR)/lib/linux/libsoil2
SOIL2_MAKE_DIR := $(SOIL2_DIR)/make/linux

INSTALL_LIB_PATH := /usr/lib/

# Sources
C_SRCS   := $(shell find $(SRC_DIR) -name *.c)
CXX_SRCS := $(shell find $(SRC_DIR) -name *.cpp)
OBJS     := $(foreach file, $(CXX_SRCS), $(BUILD_DIR)/$(file).o) \
		    $(foreach file, $(C_SRCS), $(BUILD_DIR)/$(file).o)

# Libraries
LD_FLAGS := -L$(LIB_DIR) -lc -lstdc++ -lsoil2 -lglfw -ldl -lassimp

# C Flags
C_FLAGS   := -I$(INCLUDE_DIR) -Wall
CXX_FLAGS := -I$(INCLUDE_DIR) -Wall

ifeq ($(DEBUG_BUILD), 1)
	C_FLAGS   += -g
	CXX_FLAGS += -g
	LD_FLAGS  += -g
endif

# Target exe
TARGET_EXE = mushroom_ex


# Rules
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

clean:
	-$(RM_DIR) $(BUILD_DIR)

build_libs: create_dirs build_assimp build_gl3w build_glm build_SOIL2

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
	$(CP_DIR) $(SOIL2_LIB).a $(LIB_DIR)/
	$(CP_DIR) $(SOIL2_LIB).so $(LIB_DIR)/

install_libs:
	$(CP) $(LIB_DIR)/* $(INSTALL_LIB_PATH)

clean_libs: clean_assimp clean_gl3w clean_glm clean_SOIL2

clean_assimp:
	-$(CD) $(ASSIMP_DIR) && $(MAKE) clean
	-$(RM_DIR) $(INCLUDE_DIR)/assimp
	-$(RM) $(LIB_DIR)/libassimp.*

clean_gl3w:
	-$(RM_DIR) $(INCLUDE_DIR)/GL
	-$(RM_DIR) $(INCLUDE_DIR)/KHR
	-$(RM) $(SRC_DIR)/gl3w.c

clean_glm:
	-$(RM_DIR) $(INCLUDE_DIR)/glm

clean_SOIL2:
	-$(RM_DIR) $(INCLUDE_DIR)/SOIL2
	-$(RM) $(LIB_DIR)/libsoil2.a
	-$(RM) $(LIB_DIR)/libsoil2.so