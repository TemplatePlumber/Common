BIN = bin
TARGET = $(BIN)/RdUtlCommon

# Define the source files
SOURCES = RdUtlCommon.cpp

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
	g++ -o $(TARGET) $(OBJECTS) -std=c++23

# Rule to compile a cpp files into .o files
%.o: %.cpp
	g++ -c $< -o $@ -std=c++23

# Phony target for cleaning up generated files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)