# Maze Game - C Console Application

A console-based maze exploration game developed in C for the Basic Programming course. Navigate the maze manually, explore multiple possible paths, or find the shortest path using BFS — with colorful console output!

![Demo GIF](demo.gif)

## Features
- **Manual Play**: Move from 'S' (start) to 'E' (exit) using WASD keys with real-time feedback.
- **Multiple Possible Paths**: View up to 20 different paths using randomized DFS (user can request more).
- **Shortest Path**: Computes and visually marks the shortest path using BFS (cells marked with 'b').
- Colorful console output: Red for errors, green for success, yellow for paths, blue for S/E, etc.

**Note**: Colored output uses Windows Console API and works perfectly on Windows.  
This version is **Windows-only** due to the color features.  
It will **not** display colors properly (or may have formatting issues) on Linux or macOS.

## Demo
A short video demonstration (recorded on Windows with colors) is included in the repository as `demo_video.mp4`.  
It shows all three modes in action with one of the sample mazes.

![Demo GIF](demo.gif)  
*(Animated GIF showing gameplay, path display, and shortest path mode)*

## How to Run (Windows only)
1. **Compile**:
   gcc Maze.c -o Maze
   
2. **Run**:
    Maze.exe

3. The program looks for `maze.txt` in the same directory.  
Three sample mazes are included:
- `Maze[0].txt` → Simple maze for basic testing
- `Maze[1].txt` → Medium complexity maze
- `Maze[2].txt` → Challenging maze

**To use a sample**: Rename the desired file to `maze.txt`, or edit line **65** in `Maze.c`:
```c
const char* filename = "Maze[2].txt";  // Change to any sample file
```

### Maze Format
- "#" → Wall
- S → Start (exactly one)
- E → Exit (exactly one)
- * or space → Open path
- All rows must have equal length.

### Requirements
- Windows OS (for colored console output)
- C compiler (gcc, MinGW, or Visual Studio)
- No external libraries required

### Why Windows-only?
This version uses the Windows-specific console API (SetConsoleTextAttribute) for colored text output.
On Linux or macOS, colors will not display correctly (or may cause formatting issues).
If you need a cross-platform version without colors, let me know — a Linux/macOS friendly version exists but is not included in this repository.

### Acknowledgments
Developed as part of the Basic Programming course at Shahid Beheshti University — Bahman 1404 (February 2026).
Instructor: Dr. Seyed Mohammad Hajiseyyedjavadi 

Thank you for checking out the project!
Feedback and stars are welcome 😊
