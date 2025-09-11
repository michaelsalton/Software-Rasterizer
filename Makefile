CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -Ithird_party/stb -Ithird_party/imgui -Ithird_party/imgui/backends `pkg-config --cflags sdl3`
LDFLAGS = `pkg-config --libs sdl3`

# Directories
SRC_DIR = src
BUILD_DIR = build
TEST_BUILD_DIR = build/tests

# Source files
SRCS = $(SRC_DIR)/core/main.cpp \
       $(SRC_DIR)/core/transform.cpp \
       $(SRC_DIR)/core/camera.cpp \
       $(SRC_DIR)/core/camera_controller.cpp \
       $(SRC_DIR)/pipeline/vertex_processor.cpp \
       $(SRC_DIR)/pipeline/vertex_shader.cpp \
       $(SRC_DIR)/pipeline/primitive_assembler.cpp \
       $(SRC_DIR)/pipeline/clipper.cpp \
       $(SRC_DIR)/pipeline/rasterizer.cpp \
       $(SRC_DIR)/pipeline/fragment_shader.cpp \
       $(SRC_DIR)/rendering/renderer.cpp \
       $(SRC_DIR)/rendering/texture.cpp \
       $(SRC_DIR)/graphics/graphics_context.cpp \
       $(SRC_DIR)/graphics/frame_buffer.cpp \
       $(SRC_DIR)/graphics/bitmap_font.cpp \
       $(SRC_DIR)/graphics/texture_generator.cpp \
       $(SRC_DIR)/lighting/light.cpp \
       $(SRC_DIR)/scene/entity.cpp \
       $(SRC_DIR)/game/game_manager.cpp \
       $(SRC_DIR)/utils/timer.cpp \
       $(SRC_DIR)/utils/input_manager.cpp \
       $(SRC_DIR)/ui/ui_manager.cpp \
       $(SRC_DIR)/ui/imgui.cpp \
       $(SRC_DIR)/ui/imgui_draw.cpp \
       $(SRC_DIR)/ui/imgui_tables.cpp \
       $(SRC_DIR)/ui/imgui_widgets.cpp \
       $(SRC_DIR)/ui/imgui_impl_sdl3.cpp \
       $(SRC_DIR)/ui/imgui_impl_sdlrenderer3.cpp

# Object files with build directory prefix
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = SoftwareRasterizer

# Test files
TEST_SOURCES = tests/Math/vec2_test.cpp tests/Math/vec3_test.cpp tests/Math/vec4_test.cpp tests/Math/mat4_test.cpp
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