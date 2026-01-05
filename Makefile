BIN = bin
TARGET = $(BIN)/RdUtlCommon

# Define the source files
SOURCES = TpUtlCommon.cpp TpUtlIdPool.cpp
INCLUDE = -I../../ThirdParty/boost_1_90_0 -I../../ThirdParty/libbacktrace
LIBS = -ldbghelp -lbacktrace -ldbgeng -lole32
LIB_PATHS = -L../../ThirdParty/libbacktrace/.libs 

# Define the object files (derived from source files)
OBJECTS = $(SOURCES:.c=.o)

# Default target: build the executable
all: $(TARGET)

ifeq ($(OS),Windows_NT)
MKD = if not exist $(BIN) mkdir
else
MKD = mkdir -p $(BIN)
endif


# Rule to link the executable from object files
$(TARGET): $(OBJECTS)
	@$(MKD) $(BIN)
	g++ -ggdb3 -O0 -o $(TARGET) $(OBJECTS) -std=c++23 $(LIBS) $(LIB_PATHS) $(INCLUDE)

# Rule to compile a cpp files into .o files
%.o: %.cpp
	g++ -ggdb3 -O0 -c $< -o $@ -std=c++23 $(INCLUDE)

# Phony target for cleaning up generated files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)