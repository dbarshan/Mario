CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread $(shell pkg-config --cflags allegro-5 allegro_image-5 allegro_font-5 allegro_main-5)
LDFLAGS = -pthread $(shell pkg-config --libs allegro-5 allegro_image-5 allegro_font-5 allegro_main-5)

TARGET = mario.bin
SRC_DIR = src
OBJ_DIR = obj

# Find all .cpp files in src and its subdirectories
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
# Generate object file paths
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Rule for compiling .cpp files into .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
