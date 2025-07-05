# how do I separate???
BIN=bin
BUILD=build

.PHONY: all rebuild BUILD

all: dirs rebuild build

rebuild: dirs
	cmake -S . -B $(BUILD)

build: dirs
	cmake --build $(BUILD) --config Release

dirs:
	if not exist $(BIN) mkdir $(BIN)
	if not exist $(BUILD) mkdir $(BUILD)

run:
	$(BIN)/Release/Game.exe

# clean:
# git clean -d -f -x
	