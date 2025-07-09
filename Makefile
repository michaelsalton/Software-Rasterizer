CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Isrc `pkg-config --cflags sdl3`
LDFLAGS = `pkg-config --libs sdl3`

# Source files
SRC_DIR = src
SRCS = $(SRC_DIR)/Core/Main.cpp \
       $(SRC_DIR)/Graphics/Graphics.cpp \
       $(SRC_DIR)/Graphics/Renderer.cpp \
       $(SRC_DIR)/Game/GameManager.cpp \
       $(SRC_DIR)/Game/Entity.cpp \
       $(SRC_DIR)/Utils/Timer.cpp \
       $(SRC_DIR)/Utils/InputManager.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = SoftwareRasterizer

# Test files
TEST_SOURCES = tests/Math/Vec2Test.cpp tests/Math/Vec3Test.cpp tests/Math/Vec4Test.cpp tests/Math/Mat4Test.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)
TEST_RUNNER = tests/RunAllTests

.PHONY: all clean run test test-verbose

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f $(TEST_OBJECTS) tests/RunAllTests.o $(TEST_RUNNER)
	find . -name "*.o" -type f -delete

# Test targets
test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

test-verbose: $(TEST_RUNNER)
	./$(TEST_RUNNER) -v

$(TEST_RUNNER): tests/RunAllTests.o $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Source pattern rule needs to handle nested directories
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@