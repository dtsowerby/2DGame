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

rebuild_web: dirs
	emcmake cmake -B build-emscripten

build_web: dirs
	cmake --build build-emscripten

run_web: dirs
	cd $(BIN)/web
	python -m http.server 8000
	$(info http://localhost:8000/Game.html)
