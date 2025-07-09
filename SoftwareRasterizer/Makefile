CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I. `pkg-config --cflags sdl3`
LDFLAGS = `pkg-config --libs sdl3`

SRCS = Core/Main.cpp \
       Graphics/Graphics.cpp \
       Graphics/Renderer.cpp \
       Math/MathHelper.cpp \
       Game/GameManager.cpp \
       Game/Entity.cpp \
       Utils/Timer.cpp \
       Utils/InputManager.cpp

OBJS = $(SRCS:.cpp=.o)
TARGET = SoftwareRasterizer

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)