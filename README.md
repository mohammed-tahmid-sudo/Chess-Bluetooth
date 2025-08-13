# Chess Game - Bluetooth Edition

A modern chess game built with Qt6 and C++ featuring a beautiful graphical interface and complete chess rule implementation.

## Features

- **Complete Chess Implementation**: All standard chess rules including piece movement, capture, and special moves
- **Beautiful GUI**: Clean, modern interface with piece highlighting and move history
- **Interactive Gameplay**: Click to select pieces and move them around the board
- **AI Opponent**: Built-in chess AI with multiple difficulty levels
  - **Easy**: Depth 2 search - Good for beginners
  - **Medium**: Depth 3 search - Balanced gameplay
  - **Hard**: Depth 4 search - Challenging opponent
  - **Expert**: Depth 5 search - Advanced play
- **Configurable AI**: Choose AI color (White/Black) and enable/disable AI opponent
- **Smart AI Engine**: Uses minimax algorithm with alpha-beta pruning for optimal moves
- **Position Evaluation**: AI considers piece values and positional advantages
- **Move History**: Track all moves made during the game with algebraic notation
- **Game Controls**: New game button, AI controls, move history display, and game status indicators
- **Visual Feedback**: Selected pieces are highlighted with a yellow border
- **Status Bar**: Real-time game status and move notifications

## Screenshots

The application features:
- An 8x8 chess board with alternating light and dark squares
- High-quality piece images for both white and black pieces
- Move history panel showing all moves in the current game
- Game control buttons for starting new games
- Status display showing whose turn it is

## Building and Running

### Prerequisites

- Qt6 development libraries
- C++ compiler with C++17 support
- qmake build system

### Building

```bash
# Clean any previous builds
make clean

# Generate Makefile
qmake

# Build the application
make
```

### Running

```bash
# Run the chess game
./Chess-bluetooth
```

## How to Play

1. **Starting a Game**: The game starts automatically with white to move
2. **Making Moves**: 
   - Click on a piece to select it (it will be highlighted in yellow)
   - Click on a destination square to move the piece
   - Only valid moves are allowed according to chess rules
3. **New Game**: Click the "New Game" button or use File → New Game from the menu
4. **Move History**: All moves are recorded in the right panel

### Using the AI Opponent

1. **Enable AI**: Check the "Enable AI Opponent" checkbox in the AI controls panel
2. **Choose AI Color**: Select whether the AI plays as White or Black
3. **Set Difficulty**: Choose from Easy, Medium, Hard, or Expert difficulty levels
4. **Play**: The AI will automatically make moves when it's its turn
5. **AI Status**: Watch the status bar to see when the AI is "thinking"

**Note**: When it's the AI's turn, human input is disabled to prevent interference.

## Game Rules Implemented

- **Pawns**: Forward movement, diagonal captures, double-move from starting position
- **Rooks**: Horizontal and vertical movement
- **Knights**: L-shaped moves (2+1 squares)
- **Bishops**: Diagonal movement
- **Queen**: Combination of rook and bishop moves
- **King**: One square in any direction

## Project Structure

```
Chess-bluetooth/
├── src/
│   ├── main.cpp           # Application entry point
│   ├── mainwindow.h       # Main window header
│   ├── mainwindow.cpp     # Main window implementation
│   ├── chessboard.h       # Chess board header
│   ├── chessboard.cpp     # Chess board and game logic
│   ├── chessai.h          # AI engine header
│   └── chessai.cpp        # AI engine implementation
├── resources/
│   ├── chess.qrc          # Qt resource file
│   └── Asset Images/      # Chess piece images
├── Chess-bluetooth.pro    # Qt project file
├── build.sh              # Build script
└── README.md             # This file
```

## Technical Details

- **Framework**: Qt6 Widgets
- **Language**: C++17
- **Build System**: qmake
- **Architecture**: Model-View pattern with Qt signals/slots
- **Graphics**: QPainter with custom chess board rendering
- **AI Algorithm**: Minimax with alpha-beta pruning
- **AI Features**: Position evaluation tables, move ordering, randomization for equal moves
- **Threading**: AI moves use QTimer for non-blocking UI updates

## Future Enhancements

- Bluetooth multiplayer support (as the name suggests)
- Undo/Redo functionality
- Save/Load games
- Chess engine AI opponent
- Time controls
- Sound effects

## License

This is a personal project created for learning and demonstration purposes.

## Author

Created as a chess game implementation with Qt6 and modern C++.
