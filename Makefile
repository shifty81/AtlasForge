# Atlas Engine — Makefile Convenience Targets
#
# Wraps build.sh and cmake for quick developer workflows.
#
# Usage:
#   make              # Build all targets (Release)
#   make debug        # Build all targets (Debug)
#   make tests        # Build and run tests
#   make server       # Build the headless server
#   make editor       # Build the editor
#   make clean        # Remove build artifacts
#   make help         # Show all targets
#
# The underlying build is driven by CMake; this Makefile provides
# short aliases that mirror NovaForge's Makefile conventions.

.PHONY: all debug release development server client editor runtime engine \
        tests test clean distclean configure configure-debug configure-ci \
        install sdk run-server run-editor run-client run-runtime help

# Default build type
BUILD_TYPE ?= Release
JOBS       ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
BUILD_DIR  := build
DIST_DIR   := dist

# ─── Primary targets ──────────────────────────────────────────────────

## Build all targets in Release mode (default)
all:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) all

## Build all targets in Debug mode
debug:
	@./build.sh -b Debug -j $(JOBS) all

## Build all targets in Release mode
release:
	@./build.sh -b Release -j $(JOBS) all

## Build all targets in Development mode (RelWithDebInfo)
development:
	@./build.sh -b Development -j $(JOBS) all

# ─── Individual targets ──────────────────────────────────────────────

## Build headless server
server:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) server

## Build player client
client:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) client

## Build developer editor
editor:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) editor

## Build runtime
runtime:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) runtime

## Build engine and gameplay libraries only
engine:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) engine

# ─── Testing ──────────────────────────────────────────────────────────

## Build and run all tests
tests:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) tests

## Alias for 'tests'
test: tests

## Build tests in Debug mode and run
test-debug:
	@./build.sh -b Debug -j $(JOBS) tests

## Quick-rebuild and run tests (no full configure cycle)
test-quick:
	@cmake --build $(BUILD_DIR) --target AtlasTests -j $(JOBS)
	@./$(BUILD_DIR)/tests/AtlasTests

# ─── CMake configure shortcuts ────────────────────────────────────────

## Configure with CMake (Release)
configure:
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

## Configure with CMake (Debug)
configure-debug:
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

## Configure for CI
configure-ci:
	@mkdir -p $(BUILD_DIR)
	@cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Release -DATLAS_DETERMINISM_STRICT=ON -DATLAS_SIMULATION_BUILD=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# ─── Install & SDK ───────────────────────────────────────────────────

## Install SDK (headers + libraries) to dist/sdk
install:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) -i all

## Alias for 'install'
sdk: install

# ─── Run targets ─────────────────────────────────────────────────────

## Build and run the server
run-server:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) --run server

## Build and run the editor
run-editor:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) --run editor

## Build and run the client
run-client:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) --run client

## Build and run the runtime
run-runtime:
	@./build.sh -b $(BUILD_TYPE) -j $(JOBS) --run runtime

# ─── Cleanup ─────────────────────────────────────────────────────────

## Remove build directory
clean:
	@echo "Removing build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Done."

## Remove build directory AND dist directory
distclean: clean
	@echo "Removing dist directory..."
	@rm -rf $(DIST_DIR)
	@echo "Done."

# ─── Help ────────────────────────────────────────────────────────────

## Show this help
help:
	@echo "Atlas Engine — Makefile Targets"
	@echo ""
	@echo "  Build:"
	@echo "    make              Build all (Release)"
	@echo "    make debug        Build all (Debug)"
	@echo "    make release      Build all (Release)"
	@echo "    make development  Build all (RelWithDebInfo)"
	@echo "    make server       Build headless server"
	@echo "    make client       Build player client"
	@echo "    make editor       Build developer editor"
	@echo "    make runtime      Build runtime"
	@echo "    make engine       Build engine libraries"
	@echo ""
	@echo "  Test:"
	@echo "    make tests        Build and run all tests"
	@echo "    make test-debug   Build and run tests (Debug)"
	@echo "    make test-quick   Quick rebuild and run tests"
	@echo ""
	@echo "  Configure:"
	@echo "    make configure       CMake configure (Release)"
	@echo "    make configure-debug CMake configure (Debug)"
	@echo "    make configure-ci    CMake configure (CI)"
	@echo ""
	@echo "  Install:"
	@echo "    make install      Install SDK to dist/sdk"
	@echo ""
	@echo "  Run:"
	@echo "    make run-server   Build and run server"
	@echo "    make run-editor   Build and run editor"
	@echo "    make run-client   Build and run client"
	@echo "    make run-runtime  Build and run runtime"
	@echo ""
	@echo "  Clean:"
	@echo "    make clean        Remove build directory"
	@echo "    make distclean    Remove build + dist directories"
	@echo ""
	@echo "  Variables:"
	@echo "    BUILD_TYPE=Debug|Release|Development (default: Release)"
	@echo "    JOBS=N            Parallel jobs (default: auto)"
