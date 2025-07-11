# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Commands

- **Run the game**: `go run main.go`
- **Build executable**: `go build -o snake main.go`
- **Format code**: `go fmt main.go`
- **Check for issues**: `go vet main.go`

## Architecture

This is a terminal-based Snake game written in Go as a single-file application (`main.go`). The architecture follows a simple game loop pattern:

### Core Components

- **Point struct**: Represents X,Y coordinates for snake segments and food
- **Direction enum**: Defines movement directions (Up, Down, Left, Right)  
- **Snake struct**: Contains the snake body (slice of Points) and current direction
- **Game struct**: Main game state including board dimensions, snake, food position, score, and game over status

### Game Flow

1. **Initialization**: Creates a new game with a centered snake and randomly placed food
2. **Input Handling**: Uses raw terminal mode to capture arrow key escape sequences without requiring Enter
3. **Game Loop**: Runs on a 200ms ticker that updates game state and renders the board
4. **Collision Detection**: Checks for wall collisions and self-collisions
5. **Food System**: Generates new food when eaten, avoiding snake body positions

### Terminal Integration

The game uses platform-specific terminal commands:
- Raw terminal mode for immediate key input (no Enter required)
- Screen clearing using `clear` (Unix/macOS) or `cls` (Windows)
- Arrow key detection via escape sequences (ESC[A/B/C/D)
- Box drawing characters for game border (╔═╗║╚╝)

### Controls

- Arrow keys for movement (prevents reverse direction)
- Q to quit at any time
- Game enforces movement rules (can't move directly opposite to current direction)