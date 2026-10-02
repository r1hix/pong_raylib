CC = clang
SRC = main.c

ifeq ($(OS),Windows_NT)
	TARGET = game.exe
	RAYLIB_PATH ?= C:/raylib
	CFLAGS = -Wall -Wextra -I$(RAYLIB_PATH)/include
	LDFLAGS = -L$(RAYLIB_PATH)/lib -lraylib -lopengl32 -lgdi32 -lwinmm
	RM = -cmd /C del /F /Q
else
	TARGET = game
	CFLAGS = -Wall -Wextra $(shell pkg-config --cflags raylib)
	LDFLAGS = $(shell pkg-config --libs raylib) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	RM = rm -f
endif

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	$(RM) $(TARGET)