package main

import (
	"fmt"
	"math/rand"
	"os"
	"os/exec"
	"runtime"
	"time"
)

type Point struct {
	X, Y int
}

type Direction int

const (
	Up Direction = iota
	Down
	Left
	Right
)

type Snake struct {
	Body      []Point
	Direction Direction
}

type Game struct {
	Width, Height int
	Snake         Snake
	Food          Point
	Score         int
	GameOver      bool
}

func NewGame(width, height int) *Game {
	snake := Snake{
		Body:      []Point{{width / 2, height / 2}},
		Direction: Right,
	}
	
	game := &Game{
		Width:  width,
		Height: height,
		Snake:  snake,
		Score:  0,
	}
	
	game.generateFood()
	return game
}

func (g *Game) generateFood() {
	for {
		food := Point{
			X: rand.Intn(g.Width),
			Y: rand.Intn(g.Height),
		}
		
		collision := false
		for _, segment := range g.Snake.Body {
			if segment.X == food.X && segment.Y == food.Y {
				collision = true
				break
			}
		}
		
		if !collision {
			g.Food = food
			break
		}
	}
}

func (g *Game) Update() {
	if g.GameOver {
		return
	}
	
	head := g.Snake.Body[0]
	var newHead Point
	
	switch g.Snake.Direction {
	case Up:
		newHead = Point{head.X, head.Y - 1}
	case Down:
		newHead = Point{head.X, head.Y + 1}
	case Left:
		newHead = Point{head.X - 1, head.Y}
	case Right:
		newHead = Point{head.X + 1, head.Y}
	}
	
	if newHead.X < 0 || newHead.X >= g.Width || newHead.Y < 0 || newHead.Y >= g.Height {
		g.GameOver = true
		return
	}
	
	for _, segment := range g.Snake.Body {
		if newHead.X == segment.X && newHead.Y == segment.Y {
			g.GameOver = true
			return
		}
	}
	
	g.Snake.Body = append([]Point{newHead}, g.Snake.Body...)
	
	if newHead.X == g.Food.X && newHead.Y == g.Food.Y {
		g.Score++
		g.generateFood()
	} else {
		g.Snake.Body = g.Snake.Body[:len(g.Snake.Body)-1]
	}
}

func (g *Game) Render() {
	clearScreen()
	
	board := make([][]rune, g.Height)
	for i := range board {
		board[i] = make([]rune, g.Width)
		for j := range board[i] {
			board[i][j] = ' '
		}
	}
	
	for i, segment := range g.Snake.Body {
		if i == 0 {
			board[segment.Y][segment.X] = '●'
		} else {
			board[segment.Y][segment.X] = '○'
		}
	}
	
	board[g.Food.Y][g.Food.X] = '◆'
	
	fmt.Print("╔")
	for i := 0; i < g.Width; i++ {
		fmt.Print("═")
	}
	fmt.Println("╗")
	
	for _, row := range board {
		fmt.Print("║")
		for _, cell := range row {
			fmt.Print(string(cell))
		}
		fmt.Println("║")
	}
	
	fmt.Print("╚")
	for i := 0; i < g.Width; i++ {
		fmt.Print("═")
	}
	fmt.Println("╝")
	
	fmt.Printf("Score: %d\n", g.Score)
	if g.GameOver {
		fmt.Println("Game Over! Press any key to exit...")
	} else {
		fmt.Println("Use arrow keys to move, Q to quit")
	}
}

func clearScreen() {
	var cmd *exec.Cmd
	if runtime.GOOS == "windows" {
		cmd = exec.Command("cmd", "/c", "cls")
	} else {
		cmd = exec.Command("clear")
	}
	cmd.Stdout = os.Stdout
	cmd.Run()
}

func (g *Game) HandleInput(input []byte) {
	if g.GameOver {
		return
	}
	
	// Arrow keys send escape sequences: ESC [ A/B/C/D
	if len(input) == 3 && input[0] == 27 && input[1] == 91 {
		switch input[2] {
		case 65: // Up arrow
			if g.Snake.Direction != Down {
				g.Snake.Direction = Up
			}
		case 66: // Down arrow
			if g.Snake.Direction != Up {
				g.Snake.Direction = Down
			}
		case 67: // Right arrow
			if g.Snake.Direction != Left {
				g.Snake.Direction = Right
			}
		case 68: // Left arrow
			if g.Snake.Direction != Right {
				g.Snake.Direction = Left
			}
		}
	}
}

func enableRawMode() {
	if runtime.GOOS != "windows" {
		exec.Command("stty", "-f", "/dev/tty", "cbreak", "min", "1").Run()
		exec.Command("stty", "-f", "/dev/tty", "-echo").Run()
	}
}

func disableRawMode() {
	if runtime.GOOS != "windows" {
		exec.Command("stty", "-f", "/dev/tty", "echo").Run()
		exec.Command("stty", "-f", "/dev/tty", "-cbreak").Run()
	}
}

func getInput() []byte {
	var buf [3]byte
	n, _ := os.Stdin.Read(buf[:])
	return buf[:n]
}

func main() {
	rand.Seed(time.Now().UnixNano())
	
	enableRawMode()
	defer disableRawMode()
	
	game := NewGame(20, 15)
	
	inputChan := make(chan []byte)
	go func() {
		for {
			input := getInput()
			inputChan <- input
		}
	}()
	
	ticker := time.NewTicker(200 * time.Millisecond)
	defer ticker.Stop()
	
	for {
		select {
		case input := <-inputChan:
			if len(input) == 1 && (input[0] == 'q' || input[0] == 'Q') {
				return
			}
			game.HandleInput(input)
		case <-ticker.C:
			game.Update()
			game.Render()
			if game.GameOver {
				fmt.Println("Press 'q' to quit...")
				for {
					input := <-inputChan
					if len(input) == 1 && (input[0] == 'q' || input[0] == 'Q') {
						return
					}
				}
			}
		}
	}
}