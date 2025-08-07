# Chess Game with Bluetooth and AI - Makefile
# Project: Qt-based chess game with Bluetooth connectivity and Python AI

# Compiler and tools
CXX = g++
PYTHON = python3
MOC = moc
UIC = uic
RCC = rcc

# Directories
# CXXFLAGS += -fPIE
# LDFLAGS += -pie
SRC_DIR = src
BUILD_DIR = build
UI_DIR = ui
RESOURCE_DIR = resources
PYTHON_AI_DIR = ai

# Qt and system libraries
QT_MODULES = Qt5Core Qt5Widgets Qt5Network
# Try to add Qt5Bluetooth if available, otherwise skip it
QT_BLUETOOTH_CHECK = $(shell pkg-config --exists Qt5Bluetooth && echo "Qt5Bluetooth" || echo "")
QT_FLAGS = $(shell pkg-config --cflags --libs $(QT_MODULES) $(QT_BLUETOOTH_CHECK))
PYTHON_FLAGS = $(shell $(PYTHON)-config --cflags --ldflags --embed 2>/dev/null || $(PYTHON)-config --cflags --ldflags)

# Compiler flags
# CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2
CXXFLAGS = -std=c++17 -Wall -Wextra -g -O2 -fPIC
INCLUDES = -I$(SRC_DIR) -I$(BUILD_DIR) -I/usr/include/python3.13

# Source files (adjust as you add more files)
# Exclude test.cpp as it has its own main function
SOURCES = $(filter-out $(SRC_DIR)/test.cpp, $(wildcard $(SRC_DIR)/*.cpp))
HEADERS = $(wildcard $(SRC_DIR)/*.h)
UI_FILES = $(wildcard $(UI_DIR)/*.ui)
QRC_FILES = $(wildcard $(RESOURCE_DIR)/*.qrc)

# Generated files
MOC_SOURCES = $(patsubst $(SRC_DIR)/%.h,$(BUILD_DIR)/moc_%.cpp,$(HEADERS))
UI_HEADERS = $(patsubst $(UI_DIR)/%.ui,$(BUILD_DIR)/ui_%.h,$(UI_FILES))
QRC_SOURCES = $(patsubst $(RESOURCE_DIR)/%.qrc,$(BUILD_DIR)/qrc_%.cpp,$(QRC_FILES))

# Object files
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
MOC_OBJECTS = $(patsubst $(BUILD_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MOC_SOURCES))
QRC_OBJECTS = $(patsubst $(BUILD_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(QRC_SOURCES))

ALL_OBJECTS = $(OBJECTS) $(MOC_OBJECTS) $(QRC_OBJECTS)

# Executable name
EXECUTABLE = ChessBluetoothGame

# Default target
.PHONY: all
all: setup $(EXECUTABLE)

# Setup build directory
.PHONY: setup
setup:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(SRC_DIR)
	@mkdir -p $(UI_DIR)
	@mkdir -p $(RESOURCE_DIR)
	@mkdir -p $(PYTHON_AI_DIR)

# Build the main executable
$(EXECUTABLE): $(ALL_OBJECTS)
	@echo "Linking $@..."
	$(CXX) $(ALL_OBJECTS) $(QT_FLAGS) $(PYTHON_FLAGS) -lbluetooth -o $@
	@echo "Build complete: $@"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(UI_HEADERS)
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(QT_FLAGS) -c $< -o $@

# Generate MOC files from headers
$(BUILD_DIR)/moc_%.cpp: $(SRC_DIR)/%.h
	@echo "Generating MOC for $<..."
	$(MOC) $< -o $@

# Compile MOC files
$(BUILD_DIR)/moc_%.o: $(BUILD_DIR)/moc_%.cpp
	@echo "Compiling MOC $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(QT_FLAGS) -c $< -o $@

# Generate UI headers
$(BUILD_DIR)/ui_%.h: $(UI_DIR)/%.ui
	@echo "Generating UI header for $<..."
	$(UIC) $< -o $@

# Generate resource files
$(BUILD_DIR)/qrc_%.cpp: $(RESOURCE_DIR)/%.qrc
	@echo "Generating resource file for $<..."
	$(RCC) $< -o $@

# Compile resource files
$(BUILD_DIR)/qrc_%.o: $(BUILD_DIR)/qrc_%.cpp
	@echo "Compiling resource $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(QT_FLAGS) -c $< -o $@

# Install dependencies (Fedora-specific)
.PHONY: install-deps
install-deps:
	@echo "Installing dependencies for Fedora..."
	sudo dnf install -y qt5-qtbase-devel qt5-qtbluetooth-devel python3-devel bluez-libs-devel
	@echo "Dependencies installed."

# Check Python AI setup
.PHONY: check-python
check-python:
	@echo "Checking Python AI setup..."
	@if [ -f "$(PYTHON_AI_DIR)/chess_ai.py" ]; then \
		echo "Python AI module found."; \
		cd $(PYTHON_AI_DIR) && $(PYTHON) -c "import chess; print('Chess library available')"; \
	else \
		echo "Python AI module not found. Create $(PYTHON_AI_DIR)/chess_ai.py"; \
	fi

# Install Python chess library
.PHONY: install-python-deps
install-python-deps:
	@echo "Installing Python dependencies..."
	pip3 install --user python-chess numpy

# Run the application
.PHONY: run
run: $(EXECUTABLE)
	@echo "Running $(EXECUTABLE)..."
	./$(EXECUTABLE)

# Debug run with gdb
.PHONY: debug
debug: $(EXECUTABLE)
	@echo "Starting debug session..."
	gdb ./$(EXECUTABLE)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)
	rm -f $(EXECUTABLE)
	@echo "Clean complete."

# Create a basic project structure
.PHONY: init-project
init-project: setup
	@echo "Creating basic project structure..."
	@if [ ! -f "$(SRC_DIR)/main.cpp" ]; then \
		echo "Creating main.cpp..."; \
		echo '#include <QApplication>' > $(SRC_DIR)/main.cpp; \
		echo '#include <QMainWindow>' >> $(SRC_DIR)/main.cpp; \
		echo '' >> $(SRC_DIR)/main.cpp; \
		echo 'int main(int argc, char *argv[])' >> $(SRC_DIR)/main.cpp; \
		echo '{' >> $(SRC_DIR)/main.cpp; \
		echo '    QApplication app(argc, argv);' >> $(SRC_DIR)/main.cpp; \
		echo '    QMainWindow window;' >> $(SRC_DIR)/main.cpp; \
		echo '    window.show();' >> $(SRC_DIR)/main.cpp; \
		echo '    return app.exec();' >> $(SRC_DIR)/main.cpp; \
		echo '}' >> $(SRC_DIR)/main.cpp; \
	fi
	@if [ ! -f "$(PYTHON_AI_DIR)/chess_ai.py" ]; then \
		echo "Creating basic chess_ai.py..."; \
		echo '# Chess AI Module' > $(PYTHON_AI_DIR)/chess_ai.py; \
		echo 'import chess' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo 'import chess.engine' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo 'class ChessAI:' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '    def __init__(self):' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '        self.board = chess.Board()' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '    def get_best_move(self):' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '        # Implement AI logic here' >> $(PYTHON_AI_DIR)/chess_ai.py; \
		echo '        return None' >> $(PYTHON_AI_DIR)/chess_ai.py; \
	fi
	@echo "Project structure created!"

# Show help
.PHONY: help
help:
	@echo "Chess Game with Bluetooth and AI - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all              - Build the complete project"
	@echo "  setup            - Create necessary directories"
	@echo "  install-deps     - Install system dependencies (Fedora)"
	@echo "  install-python-deps - Install Python dependencies"
	@echo "  check-python     - Check Python AI setup"
	@echo "  init-project     - Create basic project structure"
	@echo "  run              - Build and run the application"
	@echo "  debug            - Run with gdb debugger"
	@echo "  clean            - Remove build artifacts"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Project structure:"
	@echo "  $(SRC_DIR)/          - C++ source files (.cpp, .h)"
	@echo "  $(UI_DIR)/           - Qt UI files (.ui)"
	@echo "  $(RESOURCE_DIR)/     - Qt resource files (.qrc)"
	@echo "  $(PYTHON_AI_DIR)/    - Python AI modules"
	@echo "  $(BUILD_DIR)/        - Build artifacts (auto-generated)"

# Force rebuild
.PHONY: rebuild
rebuild: clean all

# Package for distribution (basic)
.PHONY: package
package: $(EXECUTABLE)
	@echo "Creating package..."
	@mkdir -p dist
	@cp $(EXECUTABLE) dist/
	@cp -r $(PYTHON_AI_DIR) dist/ 2>/dev/null || true
	@echo "Package created in dist/"
