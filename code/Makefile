SHELL=/bin/bash
RENDERBIN= raydungeon
CXX= g++

SRCDIR= src
OBJDIR= obj
TOOLDIR= tool
IMGUIDIR= imgui
INCLUDE= -I ./include -I ./imgui -I /usr/include -I /usr/local/include

SRC_MODULES= $(wildcard $(SRCDIR)/*.cpp)
TOOL_MODULES= $(wildcard $(TOOLDIR)/*.cpp)
IMGUI_MODULES= $(wildcard $(IMGUIDIR)/*.cpp)

# LIBSDIR= -L /usr/local/lib -L ./lib

LIBS= -lraylib -lnfd
CXXFLAGS= $(INCLUDE) -std=c++20 -DPLATFORM_DESKTOP -g -pthread

SRC_OBJ = $(SRC_MODULES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
TOOL_OBJ = $(TOOL_MODULES:$(TOOLDIR)/%.cpp=$(OBJDIR)/%.o)
IMGUI_OBJ = $(IMGUI_MODULES:$(IMGUIDIR)/%.cpp=$(OBJDIR)/%.o)

PHONY: renderer

renderer: $(SRC_OBJ)
	$(CXX) -o $(RENDERBIN) $(OBJDIR)/*.o $(LIBS) `pkg-config --libs gtk+-3.0` $(LDFLAGS) 
# g++ -std=c++20 ./src/*.cpp $(TOOLDIR)/*.cpp ./imgui/*.cpp -I ./include -I ./imgui -I /usr/include -I /usr/local/include -o $(RENDERBIN) 

basicDun wilsonMaze binarySpace: algorithm
	$(CXX) $(TOOLDIR)/*.cpp ./algorithmsrc/$@.cpp -I ./include -g -o ./algorithm/$@

test: $(TOOL_OBJ)
	echo hola

algorithm:
	mkdir -p ./algorithm

objDir:
	mkdir $(OBJDIR)

$(TOOL_OBJ): $(OBJDIR)/%.o: $(TOOLDIR)/%.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(IMGUI_OBJ): $(OBJDIR)/%.o: $(IMGUIDIR)/%.cpp $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(SRC_OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(TOOL_OBJ) $(IMGUI_OBJ) $(OBJDIR)
	$(CXX) -c $(CXXFLAGS) $< -o $@

$(OBJDIR):
	mkdir -p $@