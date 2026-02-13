/**
 * @file maze.c
 * @brief Console-based maze game with three modes: manual play, multiple possible paths, and shortest path using BFS.
 * @authors Sava Akbari
 *
 * This program loads a maze from a text file and provides:
 * 1. Manual navigation using WASD keys
 * 2. Display of multiple possible paths from start to exit (using randomized DFS)
 * 3. Computation and visualization of the shortest path using Breadth-First Search (BFS)
 *
 * Features colored console output (Windows) and cross-platform compatibility.
 */

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>           // for srand() and rand()

#ifdef _WIN32
#include <windows.h>    // for SetConsoleTextAttribute and Sleep
#else
#include <unistd.h>     // for sleep() on Linux/macOS
#endif

 /**
  * @defgroup Colors Console color codes (Windows only)
  * @{
  */
#define CYAN    3
#define GREEN   10
#define RED     12
#define YELLOW  14
#define WHITE   15
  /** @} */

  /**
   * @defgroup Constants Global compile-time constants
   * @{
   */
#define MAXR                105     /**< Maximum number of rows the maze can have */
#define MAXC                105     /**< Maximum number of columns the maze can have */
#define QSIZE               (MAXR * MAXC)   /**< Maximum size of BFS queue arrays */
#define MAX_PATHS_TO_SHOW   20      /**< Maximum number of possible paths to display in mode 2 */
   /** @} */

   /**
    * @defgroup Globals Global state variables
    * @{
    */
char maze[MAXR][MAXC];              /**< 2D grid storing the loaded maze */
int rows = 0, cols = 0;             /**< Actual dimensions after loading the maze file */
int sr, sc;                         /**< Start position coordinates ('S') */
int er, ec;                         /**< Exit position coordinates ('E') */
int pr, pc;                         /**< Current player position in manual mode */
int current_path_r[MAXR * MAXC];     /**< Row indices of the current DFS path */
int current_path_c[MAXR * MAXC];     /**< Column indices of the current DFS path */
int path_len;                       /**< Length (number of cells) of the current path */
int qr[QSIZE];                      /**< Row coordinates for BFS queue */
int qc[QSIZE];                      /**< Column coordinates for BFS queue */
int front, rear;                    /**< Front and rear pointers of the circular queue */
int dr[] = { -1, 1, 0, 0 };           /**< Delta row for 4 directions: up, down, left, right */
int dc[] = { 0, 0, -1, 1 };           /**< Delta column for 4 directions */
const char* filename = "maze.txt";  /**< Path to the maze input file */
/** @} */

/**
 * @defgroup Color Color control functions (Windows only)
 * @{
 */

 /**
  * @brief Sets the console foreground text color.
  * @param color One of the predefined color constants (RED, GREEN, YELLOW, etc.)
  * @note Only effective on Windows; no-op on other platforms.
  */
void set_color(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

/** @} */

/**
 * @defgroup Queue BFS Queue Management
 * @{
 */

 /**
  * @brief Initializes the circular queue used by BFS.
  * @details Sets both front and rear pointers to 0 (empty state).
  */
void queue_init(void) {
    front = rear = 0;
}

/**
 * @brief Checks whether the BFS queue is empty.
 * @return 1 if the queue is empty, 0 otherwise
 */
int queue_empty(void) {
    return front == rear;
}

/**
 * @brief Enqueues a new cell position at the rear of the BFS queue.
 * @param r Row coordinate of the cell
 * @param c Column coordinate of the cell
 */
void queue_push(int r, int c) {
    qr[rear] = r;
    qc[rear] = c;
    rear = (rear + 1) % QSIZE;
}

/**
 * @brief Dequeues and returns the cell at the front of the BFS queue.
 * @param r Pointer to store the dequeued row coordinate
 * @param c Pointer to store the dequeued column coordinate
 */
void queue_pop(int* r, int* c) {
    *r = qr[front];
    *c = qc[front];
    front = (front + 1) % QSIZE;
}

/** @} */

/**
 * @defgroup MazeIO Maze File Loading & Validation
 * @{
 */

 /**
  * @brief Loads and validates the maze from the input text file.
  * @details Reads line by line, removes trailing newline, ensures uniform row length,
  *          and locates exactly one 'S' and one 'E'.
  * @return 1 on success, 0 on failure (error message is printed)
  */
int load_maze(void) {
    FILE* f = fopen(filename, "r");
    if (f == NULL) {
        set_color(RED);
        printf("Error: maze.txt not found or cannot be opened!\n");
        set_color(WHITE);
        return 0;
    }

    rows = 0;
    char line[MAXC];
    while (fgets(line, MAXC, f) != NULL) {
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;

        strcpy(maze[rows], line);

        if (rows == 0) {
            cols = (int)len;
        }
        else if ((int)len != cols) {
            set_color(RED);
            printf("Error: All rows must have the same length!\n");
            set_color(WHITE);
            fclose(f);
            return 0;
        }

        rows++;
        if (rows >= MAXR) break;
    }
    fclose(f);

    if (rows == 0 || cols == 0) {
        set_color(RED);
        printf("Maze is empty!\n");
        set_color(WHITE);
        return 0;
    }

    sr = sc = er = ec = -1;
    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            if (maze[i][j] == 'S') { sr = i; sc = j; }
            if (maze[i][j] == 'E') { er = i; ec = j; }
        }
    }

    if (sr == -1 || er == -1) {
        set_color(RED);
        printf("Maze must contain 'S' and 'E'!\n");
        set_color(WHITE);
        return 0;
    }

    return 1;
}

/** @} */

/**
 * @defgroup Display Maze Rendering
 * @{
 */

 /**
  * @brief Clears the terminal screen and renders the maze grid with colored characters.
  * @param grid The maze grid to display (can be original or modified copy)
  * @param show_player If non-zero, renders the player position as red '^'
  */
void print_maze(const char grid[MAXR][MAXC], int show_player) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif  // Windows only; consider "clear" for Unix-like systems

    int i, j;
    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols; j++) {
            char ch = grid[i][j];

            if (show_player && i == pr && j == pc) {
                set_color(RED);
                printf("^");
                set_color(WHITE);
            }
            else if (ch == 'S' || ch == 'E') {
                set_color(9);   // Bright blue
                printf("%c", ch);
                set_color(WHITE);
            }
            else if (ch == '^') {
                set_color(RED); // Path markers
                printf("^");
                set_color(WHITE);
            }
            else if (ch == '#') {
                set_color(YELLOW); // Walls
                printf("%c", ch);
                set_color(WHITE);
            }
            else if (ch == 'b') {
                set_color(GREEN); // Shortest path cells
                printf("%c", ch);
                set_color(WHITE);
            }
            else {
                printf("%c", ch);
            }
        }
        printf("\n");
    }
    printf("\n");
}

/** @} */

/**
 * @defgroup Movement Player Movement Logic
 * @{
 */

 /**
  * @brief Validates whether a cell is inside the maze and passable.
  * @param r Row index
  * @param c Column index
  * @return 1 if the position is valid and not a wall, 0 otherwise
  */
int is_valid(int r, int c) {
    if (r < 0 || r >= rows || c < 0 || c >= cols) return 0;
    if (maze[r][c] == '#') return 0;
    return 1;
}

/**
 * @brief Handles player movement based on keyboard input.
 * @param ch Input character representing direction ('w','a','s','d') or other
 */
void move_player(char ch) {
    int nr = pr, nc = pc;

    if (ch == 'w' || ch == 'W') nr--;
    else if (ch == 's' || ch == 'S') nr++;
    else if (ch == 'a' || ch == 'A') nc--;
    else if (ch == 'd' || ch == 'D') nc++;
    else {
        set_color(RED);
        printf("Invalid movement! Use w, a, s, d or q to quit.\n");
        set_color(WHITE);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
        return;
    }

    if (is_valid(nr, nc)) {
        pr = nr;
        pc = nc;
    }
    else {
        set_color(RED);
        printf("Invalid movement! Cannot go through walls or out of bounds.\n");
        set_color(WHITE);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }
}

/** @} */

/**
 * @defgroup Manual Manual Gameplay Mode
 * @{
 */

 /**
  * @brief Interactive loop for manual maze navigation using WASD keys.
  */
void play_manual(void) {
    pr = sr;
    pc = sc;

    while (1) {
        print_maze(maze, 1);

        if (pr == er && pc == ec) {
            set_color(GREEN);
            printf("Congratulations! You reached the exit!\n\n");
            set_color(WHITE);
            break;
        }

        printf("Move (w a s d) or q to quit: ");
        char ch;
        scanf(" %c", &ch);

        if (ch == 'q' || ch == 'Q') {
            set_color(RED);
            printf("You quit the game.\n");
            set_color(WHITE);
            return;
        }

        move_player(ch);
    }
}

/** @} */

/**
 * @defgroup BFS Shortest Path Computation (BFS)
 * @{
 */

 /**
  * @brief Reconstructs and marks the shortest path on the maze using parent information.
  * @param parent_r 2D array of parent row indices from BFS
  * @param parent_c 2D array of parent column indices from BFS
  */
void mark_shortest_path(int parent_r[MAXR][MAXC], int parent_c[MAXR][MAXC]) {
    int cr = er, cc = ec;
    int length = 0;

    while (cr != sr || cc != sc) {
        int tempr = parent_r[cr][cc];
        int tempc = parent_c[cr][cc];
        if (maze[cr][cc] != 'S' && maze[cr][cc] != 'E') {
            maze[cr][cc] = 'b';
        }
        cr = tempr;
        cc = tempc;
        length++;
    }

    set_color(YELLOW);
    printf("Shortest path (length: %d steps):\n", length);
    set_color(WHITE);
#ifdef _WIN32
    Sleep(1300);
#else
    sleep(1);
#endif
}

/**
 * @brief Computes the shortest path from 'S' to 'E' using Breadth-First Search.
 * @details Uses a queue and parent tracking to reconstruct the path.
 */
void bfs_shortest(void) {
    int visited[MAXR][MAXC] = { 0 };
    int parent_r[MAXR][MAXC];
    int parent_c[MAXR][MAXC];
    int found = 0;

    queue_init();
    queue_push(sr, sc);
    visited[sr][sc] = 1;
    parent_r[sr][sc] = -1;
    parent_c[sr][sc] = -1;

    while (!queue_empty() && !found) {
        int cr, cc;
        queue_pop(&cr, &cc);

        int d;
        for (d = 0; d < 4; d++) {
            int nr = cr + dr[d];
            int nc = cc + dc[d];

            if (!is_valid(nr, nc)) continue;
            if (visited[nr][nc]) continue;

            visited[nr][nc] = 1;
            parent_r[nr][nc] = cr;
            parent_c[nr][nc] = cc;
            queue_push(nr, nc);

            if (nr == er && nc == ec) {
                found = 1;
                break;
            }
        }
    }

    if (!found) {
        set_color(RED);
        printf("No path exists!\n");
        set_color(WHITE);
        return;
    }

    mark_shortest_path(parent_r, parent_c);
    print_maze(maze, 0);
}

/** @} */

/**
 * @defgroup DFS Possible Paths via Randomized DFS
 * @{
 */

 /**
  * @brief Finds one path from the current cell to the exit using randomized DFS.
  * @param r Current row
  * @param c Current column
  * @param visited 2D visited matrix to avoid revisiting cells
  * @return 1 if a path to the exit was found, 0 otherwise
  */
int dfs_find_one_path(int r, int c, int visited[MAXR][MAXC]) {
    current_path_r[path_len] = r;
    current_path_c[path_len] = c;
    path_len++;

    if (r == er && c == ec) {
        return 1;
    }

    visited[r][c] = 1;

    // Randomize direction order to generate different paths
    int dirs[4] = { 0, 1, 2, 3 };
    int i;
    for (i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = dirs[i];
        dirs[i] = dirs[j];
        dirs[j] = temp;
    }

    for (i = 0; i < 4; i++) {
        int dir_idx = dirs[i];
        int nr = r + dr[dir_idx];
        int nc = c + dc[dir_idx];

        if (is_valid(nr, nc) && !visited[nr][nc]) {
            if (dfs_find_one_path(nr, nc, visited)) {
                return 1;
            }
        }
    }

    visited[r][c] = 0;
    path_len--;
    return 0;
}

/**
 * @brief Displays multiple possible paths from start to exit one by one.
 * @details Uses DFS with randomized direction order and asks user if they want more paths.
 */
void show_some_solutions(void) {
    int count = 0;
    char user_answer;

    set_color(YELLOW);
    printf("Searching for possible paths...\n\n");
    set_color(WHITE);
#ifdef _WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    while (count < MAX_PATHS_TO_SHOW) {
        int visited[MAXR][MAXC] = { 0 };
        path_len = 0;

        int found = dfs_find_one_path(sr, sc, visited);

        if (!found) {
            set_color(RED);
            printf("No more paths found.\n");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            set_color(WHITE);
            break;
        }

        count++;

        char temp_maze[MAXR][MAXC];
        int i;
        for (i = 0; i < rows; i++) {
            strcpy(temp_maze[i], maze[i]);
        }

        // Mark path excluding S and E
        for (i = 1; i < path_len - 1; i++) {
            temp_maze[current_path_r[i]][current_path_c[i]] = '^';
        }

        set_color(YELLOW);
        printf("\n--- Possible Path #%d (length: %d steps) ---\n", count, path_len - 1);
        set_color(WHITE);
#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif

        print_maze(temp_maze, 0);

        if (count >= MAX_PATHS_TO_SHOW) {
            printf("\nMaximum number of paths reached.\n");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
            break;
        }

        set_color(CYAN);
        printf("\nDo you want to see another path? (y/n): ");
        set_color(WHITE);
        scanf(" %c", &user_answer);

        if (user_answer != 'y' && user_answer != 'Y') {
            break;
        }
    }
}

/** @} */

/**
 * @defgroup Menu Program Menu & Control Flow
 * @{
 */

 /**
  * @brief Displays the main menu and reads the user's selection.
  * @return The selected option (1–4)
  */
int show_menu(void) {
    int choice;

    set_color(CYAN);
    printf("\n=== Maze Game Menu ===\n");
    printf("1 - Play manually (WASD)\n");
    printf("2 - Show some possible solutions (up to %d paths)\n", MAX_PATHS_TO_SHOW);
    printf("3 - Show shortest path (BFS)\n");
    printf("4 - Exit\n");
    printf("Your choice: ");
    set_color(WHITE);
    scanf("%d", &choice);
    return choice;
}

/**
 * @brief Program entry point and main control loop.
 * @return 0 on normal termination
 */
int main(void) {
    srand((unsigned int)time(NULL));

    if (!load_maze()) {
        set_color(RED);
        printf("Program terminated.\n");
        set_color(WHITE);
        return 1;
    }

    while (1) {
        int opt = show_menu();

        if (opt == 1) {
            play_manual();
        }
        else if (opt == 2) {
            show_some_solutions();
        }
        else if (opt == 3) {
            bfs_shortest();
        }
        else if (opt == 4) {
            set_color(YELLOW);
            printf("Goodbye!\n");
            set_color(WHITE);
            return 0;
        }
        else {
            set_color(RED);
            printf("Invalid option!\n");
            set_color(WHITE);
            continue;
        }

#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif

        set_color(CYAN);
        printf("\n1 - Return to menu\n");
        printf("2 - Exit program\n");
        printf("Your choice: ");
        set_color(WHITE);
        int again;
        scanf("%d", &again);

        if (again != 1) {
            set_color(YELLOW);
            printf("Goodbye!\n");
            set_color(WHITE);
            return 0;
        }

        load_maze();  // Reset maze to original state after each mode
    }

    return 0;
}

/** @} */
