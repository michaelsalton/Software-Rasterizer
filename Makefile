CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Ithird_party/stb -Ithird_party/imgui -Ithird_party/imgui/backends `pkg-config --cflags sdl3`
LDFLAGS = `pkg-config --libs sdl3`

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_BUILD_DIR = build/tests

# Source files
SRCS = $(SRC_DIR)/Core/Main.cpp \
       $(SRC_DIR)/Core/Transform.cpp \
       $(SRC_DIR)/Core/Camera.cpp \
       $(SRC_DIR)/Core/CameraController.cpp \
       $(SRC_DIR)/Graphics/Graphics.cpp \
       $(SRC_DIR)/Graphics/Renderer.cpp \
       $(SRC_DIR)/Graphics/Framebuffer.cpp \
       $(SRC_DIR)/Graphics/Vertex.cpp \
       $(SRC_DIR)/Graphics/VertexShader.cpp \
       $(SRC_DIR)/Graphics/Clipper.cpp \
       $(SRC_DIR)/Graphics/PrimitiveAssembler.cpp \
       $(SRC_DIR)/Graphics/Rasterizer.cpp \
       $(SRC_DIR)/Graphics/BitmapFont.cpp \
       $(SRC_DIR)/Graphics/Texture.cpp \
       $(SRC_DIR)/Graphics/FragmentShader.cpp \
       $(SRC_DIR)/Graphics/TextureGenerator.cpp \
       $(SRC_DIR)/Game/GameManager.cpp \
       $(SRC_DIR)/Game/Entity.cpp \
       $(SRC_DIR)/Utils/Timer.cpp \
       $(SRC_DIR)/Utils/InputManager.cpp \
       $(SRC_DIR)/GUI/GUIManager.cpp \
       $(SRC_DIR)/GUI/imgui.cpp \
       $(SRC_DIR)/GUI/imgui_draw.cpp \
       $(SRC_DIR)/GUI/imgui_tables.cpp \
       $(SRC_DIR)/GUI/imgui_widgets.cpp \
       $(SRC_DIR)/GUI/imgui_impl_sdl3.cpp \
       $(SRC_DIR)/GUI/imgui_impl_sdlrenderer3.cpp

# Object files with build directory prefix
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = SoftwareRasterizer

# Test files
TEST_SOURCES = tests/Math/Vec2Test.cpp tests/Math/Vec3Test.cpp tests/Math/Vec4Test.cpp tests/Math/Mat4Test.cpp
TEST_OBJECTS = $(patsubst tests/%.cpp,$(TEST_BUILD_DIR)/%.o,$(TEST_SOURCES))
TEST_RUNNER = tests/RunAllTests

.PHONY: all clean run test test-verbose

all: $(TARGET)

# Create build directories
$(BUILD_DIR) $(TEST_BUILD_DIR):
	mkdir -p $@

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

# Pattern rule for building object files in build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET) $(TEST_RUNNER)
	find . -name "*.o" -type f -delete

# Test targets
test: $(TEST_RUNNER)
	./$(TEST_RUNNER)

test-verbose: $(TEST_RUNNER)
	./$(TEST_RUNNER) -v

$(TEST_RUNNER): $(TEST_BUILD_DIR)/RunAllTests.o $(TEST_OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

# Pattern rule for test object files
$(TEST_BUILD_DIR)/%.o: tests/%.cpp | $(TEST_BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@