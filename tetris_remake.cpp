#include <SDL.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int WINDOW_WIDTH = 500;
const int WINDOW_HEIGHT = 600;
const int GRID_COLS = 10;
const int GRID_ROWS = 20;
const int CELL_SIZE = 30; // pixels of each cell

bool paused = false;
bool gameOver = false;
bool onStartScreen = true;

Uint32 lockDelay = 300;// milliseconds to lock piece
Uint32 groundStart = 0;
int touchingGround = 0;
int grid[GRID_ROWS][GRID_COLS] = { 0 };
int currentPieceID = -1;
int currentPiece[4][4];
int tetX = 3;   // piece spawn X
int tetY = 0;   // spawn at top
int rotationState = 0;
int SIDEBAR_X = GRID_COLS * CELL_SIZE + 50;
int totalScore = 0;
int linesCleared = 0;
int holdPieceID = -1;            // no piece held yet
int canHold = 1;                 // can only hold once per piece
int nextPieceID = -1;
int ghostX, ghostY;

// Standard SRS kicks for JLTSZ pieces
int kicksNormal[4][5][2] =
{
    // 0 -> R
    { {0,0}, {-1,0}, {-1,1}, {0,-2}, {-1,-2} },

    // R -> 2
    { {0,0}, {1,0},  {1,1},  {0,-2}, {1,-2} },

    // 2 -> 3
    { {0,0}, {1,0},  {1,-1}, {0,2},  {1,2} },

    // 3 -> 0
    { {0,0}, {-1,0}, {-1,-1},{0,2},  {-1,2} }
};


// SRS kicks for I piece
int kicksI[4][5][2] =
{
    // 0 -> R
    { {0,0}, {-2,0}, {1,0},  {-2,-1}, {1,2} },

    // R -> 2
    { {0,0}, {-1,0}, {2,0},  {-1,2},  {2,-1} },

    // 2 -> 3
    { {0,0}, {2,0},  {-1,0}, {2,1},   {-1,-2} },

    // 3 -> 0
    { {0,0}, {1,0},  {-2,0}, {1,-2},  {-2,1} }
};

// 7 Tetris shapes (4x4)
int TETROMINOS[7][4][4] = {
    // I
    {
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0},
        {0,1,0,0}
    },
    // O
    {
        {1,1,0,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // T
    {
        {0,1,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // L
    {
        {0,0,1,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // J
    {
        {1,0,0,0},
        {1,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // S
    {
        {0,1,1,0},
        {1,1,0,0},
        {0,0,0,0},
        {0,0,0,0}
    },
    // Z
    {
        {1,1,0,0},
        {0,1,1,0},
        {0,0,0,0},
        {0,0,0,0}
    }
};

SDL_Color pieceColors[8] = {
    {0, 0, 0, 255},        // 0: empty
    {0, 255, 255, 255},    // 1: I (cyan)
    {255, 255, 0, 255},    // 2: O (yellow)
    {128, 0, 128, 255},    // 3: T (purple)
    {255, 165, 0, 255},    // 4: L (orange)
    {0, 0, 255, 255},      // 5: J (blue)
    {0, 255, 0, 255},      // 6: S (green)
    {255, 0, 0, 255}       // 7: Z (red)
};

int CheckCollision(int newX, int newY);

void spawnPiece() {

    // If this is the first piece of the game:
    if (nextPieceID == -1)
        nextPieceID = rand() % 7;

    currentPieceID = nextPieceID;
    nextPieceID = rand() % 7;    // generate next piece

    rotationState = 0;

    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            currentPiece[r][c] = TETROMINOS[currentPieceID][r][c];

    // SRS STANDARD SPAWN POSITIONS
    if (currentPieceID == 0 || currentPieceID == 1) {  // I or O
        tetX = 4;  // Center in 10-wide field (columns 4-7)
    }
    else {  // T,L,J,S,Z (3-wide, left-handed spawn)
        tetX = 3;  // Columns 3-5 (standard left-center)
    }

    tetY = 0;  // Row 0 (invisible spawn above visible grid)
    canHold = 1;
}

int CheckCollision(int newX, int newY)
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (currentPiece[r][c]) {

                int gx = newX + c;
                int gy = newY + r;

                // Wall collision
                if (gx < 0 || gx >= GRID_COLS || gy >= GRID_ROWS)
                    return 1;

                // Collision with placed blocks
                if (gy >= 0 && grid[gy][gx] != 0)
                    return 1;
            }
        }
    }

    return 0; // No collision
}

// ADD THIS NEW FUNCTION (after CheckCollision):
int CheckGhostCollision(int newX, int newY)
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (currentPiece[r][c]) {  // Uses same currentPiece shape
                int gx = newX + c;
                int gy = newY + r;

                // Wall collision
                if (gx < 0 || gx >= GRID_COLS || gy >= GRID_ROWS)
                    return 1;

                // ONLY collide with placed blocks (ignores active piece)
                if (gy >= 0 && grid[gy][gx] != 0)
                    return 1;
            }
        }
    }
    return 0;
}


void UpdateGhost()
{
    ghostX = tetX;
    ghostY = tetY;

    while (!CheckGhostCollision(ghostX, ghostY + 1))
    {
        ghostY++;
    }
}

void LockPiece()
{
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (currentPiece[r][c]) {
                int gx = tetX + c;
                int gy = tetY + r;
                if (gy >= 0 && gy < GRID_ROWS && gx >= 0 && gx < GRID_COLS)
                    grid[gy][gx] = currentPieceID + 1; // store color/index later
            }
        }
    }
}

int ClearLines() {
    int cleared = 0;
    for (int r = GRID_ROWS - 1; r >= 0; r--) { // bottom to top
        int full = 1;
        for (int c = 0; c < GRID_COLS; c++) {
            if (grid[r][c] == 0) { full = 0; break; }
        }

        if (full) {
            cleared++;
            // shift all rows above down
            for (int row = r; row > 0; row--)
                for (int col = 0; col < GRID_COLS; col++)
                    grid[row][col] = grid[row - 1][col];

            // clear top row
            for (int col = 0; col < GRID_COLS; col++)
                grid[0][col] = 0;

            r++; // check same row again after shift
        }
    }

    if (cleared > 0) {
        int points[] = { 0,40,100,300,1200 };
        if (cleared > 4) cleared = 4;
        totalScore += points[cleared];
        linesCleared += cleared;
    }

    return cleared;
}


void RotatePiece()
{
    int backup[4][4];
    memcpy(backup, currentPiece, sizeof(backup));

    int oldRot = rotationState;
    int newRot = (rotationState + 1) % 4;

    if (currentPieceID == 1)   // O-piece doesn't rotate
        return;

    // Rotate piece (4×4 clockwise)
    int rotated[4][4] = { 0 };
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < 4; c++)
            rotated[c][3 - r] = currentPiece[r][c];

    memcpy(currentPiece, rotated, sizeof(rotated));

    // pick correct kick table row (I-piece or normal)
    int (*kickTable)[2] = (currentPieceID == 0)
        ? kicksI[oldRot]
        : kicksNormal[oldRot];

    // Try kicks
    for (int i = 0; i < 5; i++)
    {
        int dx = kickTable[i][0];
        int dy = kickTable[i][1];

        if (!CheckCollision(tetX + dx, tetY + dy))
        {
            tetX += dx;
            tetY += dy;
            rotationState = newRot;
            return;
        }
    }

    // All failed -> revert
    memcpy(currentPiece, backup, sizeof(backup));
}

void RestartGame()
{
    gameOver = false;
    paused = false;

    // Reset score & stats
    totalScore = 0;
    linesCleared = 0;

    // Reset hold mechanic
    holdPieceID = -1;
    canHold = 1;

    // Clear grid
    memset(grid, 0, sizeof(grid));

    // Reset piece position/state
    rotationState = 0;

    // Generate next piece again
    nextPieceID = rand() % 7;

    // Spawn current piece
    spawnPiece();
	UpdateGhost();

    // Reset lock-delay mechanics
    touchingGround = 0;
    groundStart = 0;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());         // check for error initializing SDL
        return 1;
    }

    if (TTF_Init() == -1) {
        printf("TTF_Init Error: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("C:\\Windows\\Fonts\\MAIAN.TTF", 24);
    if (!font) {
        printf("Failed to load font: %s\n", TTF_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Tetris",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    spawnPiece();
    UpdateGhost();

    int running = 1;
    bool pieceJustLocked = false;
    SDL_Event event; // Declare the event variable here
    Uint32 lastFallTime = 0; // Move this variable outside the loop
    Uint32 fallDelay = 500; // drop piece every 500 ms


	SDL_Rect restartButton = {
		(WINDOW_WIDTH - 150) / 2,
		WINDOW_HEIGHT / 2 + 10,
		150,
		50
	};

    SDL_Rect startButton = { 
        WINDOW_WIDTH / 2 - 100, 
        WINDOW_HEIGHT / 2 + 50,
        200,
        50
    };

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;

            if (event.type == SDL_KEYDOWN) {

                switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                case SDLK_a:
                    if (!paused && !pieceJustLocked && !CheckCollision(tetX - 1, tetY)) {
                        tetX--;
                        UpdateGhost();
                    }
                    break;

                case SDLK_RIGHT:
                case SDLK_d:
                    if (!paused && !pieceJustLocked && !CheckCollision(tetX + 1, tetY)) {
                        tetX++;
                        UpdateGhost();
                    }
                    break;

                case SDLK_DOWN:
                case SDLK_s:
                    if (!paused && !pieceJustLocked && !CheckCollision(tetX, tetY + 1)) {
                        tetY++;
                        lastFallTime = SDL_GetTicks();  // Reset gravity timer for faster fall
                        touchingGround = 0;             // Reset lock timer (standard soft drop behavior)
                        groundStart = 0;
                    }
                    UpdateGhost();
                    break;

                case SDLK_UP:
                case SDLK_w:
                    if (!paused && !pieceJustLocked && !event.key.repeat) {
                        RotatePiece();
                        UpdateGhost();
                    }
                    break;

                case SDLK_c:   // Hold piece
                    if (!paused && !pieceJustLocked && canHold) {
                        int temp = holdPieceID;
                        holdPieceID = currentPieceID;

                        if (temp == -1) {
                            spawnPiece();
							pieceJustLocked = 1; // trigger 100ms delay before next input
                        }
                        else {
                            currentPieceID = temp;
                            for (int r = 0; r < 4; r++)
                                for (int c = 0; c < 4; c++)
                                    currentPiece[r][c] = TETROMINOS[currentPieceID][r][c];
                            
                            // spawn position logic
                            if (currentPieceID == 0 || currentPieceID == 1) tetX = 4;
                            else tetX = 3;
                            tetY = 0;

							pieceJustLocked = 1; // trigger 100ms delay again
                        }

                        canHold = 0;
                    }
                    break;

                case SDLK_SPACE:
                    if (!paused && !pieceJustLocked) {
                        while (!CheckCollision(tetX, tetY + 1))
                            tetY++;
                    }
                    LockPiece();
					ClearLines();
                    spawnPiece();
                    canHold = 1;
					pieceJustLocked = 1;
                    break;

                case SDLK_p:   // Pause toggle (always allowed)
                    if (!gameOver)
                        paused = !paused;
                    break;

                case SDLK_r:   // Restart
                    if (!event.key.repeat)
                        RestartGame();
					    UpdateGhost();
                    break;
                }

                if (gameOver && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r) {
                    RestartGame();
                }

            }

            // Restart button click (allowed during game over)
            if (gameOver && event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;
                if (mx >= restartButton.x && mx <= restartButton.x + restartButton.w &&
                    my >= restartButton.y && my <= restartButton.y + restartButton.h) {
                    RestartGame();
                }
            }

            // START BUTTON (mouse)
            if (onStartScreen && event.type == SDL_MOUSEBUTTONDOWN) {
                int mx = event.button.x;
                int my = event.button.y;

                if (mx >= startButton.x && mx <= startButton.x + startButton.w &&
                    my >= startButton.y && my <= startButton.y + startButton.h)
                {
                    onStartScreen = false;
                    RestartGame(); // initializes board, pieces, score
                }
            }

            // START BUTTON (keyboard)
            if (onStartScreen && event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                onStartScreen = false;
                RestartGame();
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (!paused && !pieceJustLocked && SDL_GetTicks() - lastFallTime >= fallDelay) {
            if (CheckCollision(tetX, tetY + 1)) {
                if (!touchingGround) {
                    touchingGround = 1;
                    groundStart = SDL_GetTicks();
                }
                if (SDL_GetTicks() - groundStart >= lockDelay) {
                    LockPiece();
                    int cleared = ClearLines();
                    spawnPiece();
                    canHold = 1;
                    pieceJustLocked = 1;
                    touchingGround = 0;
                    groundStart = 0;
                }
            }
            else {
                tetY++;
                touchingGround = 0;
                groundStart = 0;
            }
            lastFallTime = SDL_GetTicks();
        }

        //OUTSIDE of gravity block
        if (pieceJustLocked) {
            static Uint32 lockResetTime = 0;
            if (lockResetTime == 0)
                lockResetTime = SDL_GetTicks();

            if (SDL_GetTicks() - lockResetTime > 100 ||
                (!CheckGhostCollision(tetX - 1, tetY) || !CheckGhostCollision(tetX + 1, tetY))) {
                UpdateGhost();
                if (CheckCollision(tetX, tetY))
                    gameOver = true;
                pieceJustLocked = false;
                lockResetTime = 0;
            }
        }

        // Draw grid lines
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // white lines
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                SDL_Rect cell;
                cell.x = c * CELL_SIZE;
                cell.y = r * CELL_SIZE;
                cell.w = CELL_SIZE;
                cell.h = CELL_SIZE;
                SDL_RenderDrawRect(renderer, &cell); // draw only border

            }
        }

		// Draw ghost piece
        for (int r = 0; r < 4; r++)
        {
            for (int c = 0; c < 4; c++)
            {
                if (currentPiece[r][c])
                {
                    SDL_Rect gRect = {
                        (ghostX + c) * CELL_SIZE,
                        (ghostY + r) * CELL_SIZE,
                        CELL_SIZE,
                        CELL_SIZE
                    };

                    SDL_SetRenderDrawColor(renderer, 250, 250, 250, 80);  // translucent gray
                    SDL_RenderFillRect(renderer, &gRect);
                }
            }
        }

        // Draw the blocks
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (currentPiece[r][c]) {
                    SDL_Rect cell;
                    cell.x = (tetX + c) * CELL_SIZE;
                    cell.y = (tetY + r) * CELL_SIZE;
                    cell.w = CELL_SIZE;
                    cell.h = CELL_SIZE;

                    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 150); // cyan piece with transparency
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }

        // Draw placed blocks (filled cells)
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                int id = grid[r][c];
                if (id) {
                    SDL_Rect cell;
                    cell.x = c * CELL_SIZE;
                    cell.y = r * CELL_SIZE;
                    cell.w = CELL_SIZE;
                    cell.h = CELL_SIZE;
                    SDL_Color color = pieceColors[id];
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 150);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }

        // Render Sidebar Text
        SDL_Color white = { 255, 255, 255, 255 };

        // ---------- Score ----------
        char scoreText[64];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", totalScore);

        SDL_Surface* surfScore = TTF_RenderText_Solid(font, scoreText, white);
        if (surfScore) {
            SDL_Texture* texScore = SDL_CreateTextureFromSurface(renderer, surfScore);

            SDL_Rect rectScore = { SIDEBAR_X, 20, surfScore->w, surfScore->h };
            SDL_RenderCopy(renderer, texScore, NULL, &rectScore);

            SDL_FreeSurface(surfScore);
            SDL_DestroyTexture(texScore);
        }

        // ---------- Lines ----------
        char linesText[64];
        snprintf(linesText, sizeof(linesText), "Lines: %d", linesCleared);

        SDL_Surface* surfLines = TTF_RenderText_Solid(font, linesText, white);
        if (surfLines) {
            SDL_Texture* texLines = SDL_CreateTextureFromSurface(renderer, surfLines);

            SDL_Rect rectLines = { SIDEBAR_X, 60, surfLines->w, surfLines->h };
            SDL_RenderCopy(renderer, texLines, NULL, &rectLines);

            SDL_FreeSurface(surfLines);
            SDL_DestroyTexture(texLines);
        }

        // ---------- Controls ----------
        char controlsText[128];
        snprintf(controlsText, sizeof(controlsText),
            "Controls:\n-a d- move\n-w- rotate\n-s- soft drop\n-space- hard drop\n-c- hold\n-p- pause\n-r- restart");

        SDL_Surface* surfControls = TTF_RenderText_Blended_Wrapped(font, controlsText, white, 150);
        if (surfControls) {
            SDL_Texture* texControls = SDL_CreateTextureFromSurface(renderer, surfControls);

            SDL_Rect rectControls = {
                SIDEBAR_X,
                WINDOW_HEIGHT - surfControls->h - 40,
                surfControls->w,
                surfControls->h
            };

            SDL_RenderCopy(renderer, texControls, NULL, &rectControls);

            SDL_FreeSurface(surfControls);
            SDL_DestroyTexture(texControls);
        }

        // ---------------- NEXT PIECE ----------------
        SDL_Surface* nextLabel = TTF_RenderText_Solid(font, "Next piece:", white);
        SDL_Texture* nextLabelTex = SDL_CreateTextureFromSurface(renderer, nextLabel);

        SDL_Rect nlRect = { SIDEBAR_X, 120, nextLabel->w, nextLabel->h };
        SDL_RenderCopy(renderer, nextLabelTex, NULL, &nlRect);

        SDL_FreeSurface(nextLabel);
        SDL_DestroyTexture(nextLabelTex);

        // Draw next tetromino (in a 4x4 preview box)
        for (int r = 0; r < 4; r++) {
            for (int c = 0; c < 4; c++) {
                if (TETROMINOS[nextPieceID][r][c]) {
                    SDL_Rect cell = {
                        SIDEBAR_X + c * 20,
                        160 + r * 20,
                        20, 20
                    };
                    SDL_Color color = pieceColors[nextPieceID + 1];
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 150);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }
        
        // ---------------- HOLD PIECE ----------------
        SDL_Surface* holdLabel = TTF_RenderText_Solid(font, "Hold:", white);
        SDL_Texture* holdLabelTex = SDL_CreateTextureFromSurface(renderer, holdLabel);

        SDL_Rect hlRect = { SIDEBAR_X, 260, holdLabel->w, holdLabel->h };
        SDL_RenderCopy(renderer, holdLabelTex, NULL, &hlRect);

        SDL_FreeSurface(holdLabel);
        SDL_DestroyTexture(holdLabelTex);

        if (holdPieceID != -1) {
            for (int r = 0; r < 4; r++) {
                for (int c = 0; c < 4; c++) {
                    if (TETROMINOS[holdPieceID][r][c]) {
                        SDL_Rect cell = {
                            SIDEBAR_X + c * 20,
                            300 + r * 20,
                            20, 20
                        };
                        SDL_Color color = pieceColors[holdPieceID + 1];
                        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 150);
                        SDL_RenderFillRect(renderer, &cell);
                    }
                }
            }
        }

        // ---- PAUSE: skip updates but still render frame ----
        if (paused) {
            // Draw paused text in the middle
                // Semi-transparent black overlay
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // black with alpha
            SDL_Rect overlay = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
            SDL_RenderFillRect(renderer, &overlay);

            // PAUSED text
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Surface* pausedSurface = TTF_RenderText_Solid(font, "PAUSED", white);
            SDL_Texture* pausedTexture = SDL_CreateTextureFromSurface(renderer, pausedSurface);

            SDL_Rect pausedRect = {
                (WINDOW_WIDTH - pausedSurface->w) / 2,
                (WINDOW_HEIGHT - pausedSurface->h) / 2,
                pausedSurface->w,
                pausedSurface->h
            };

            SDL_RenderCopy(renderer, pausedTexture, NULL, &pausedRect);
            SDL_RenderPresent(renderer);

            SDL_FreeSurface(pausedSurface);
            SDL_DestroyTexture(pausedTexture);

            continue;    // <- SKIPS ALL MOVEMENT & GRAVITY
        }

        if (gameOver) {
            // Fill background black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, NULL);

            // Draw "GAME OVER" text
            SDL_Color red = { 255, 0, 0, 255 };
            SDL_Surface* goSurface = TTF_RenderText_Solid(font, "GAME OVER", red);
            SDL_Texture* goTex = SDL_CreateTextureFromSurface(renderer, goSurface);
            SDL_Rect goRect = {
                (WINDOW_WIDTH - goSurface->w) / 2,
                WINDOW_HEIGHT / 2 - 50,
                goSurface->w,
                goSurface->h
            };
            SDL_RenderCopy(renderer, goTex, NULL, &goRect);
            SDL_FreeSurface(goSurface);
            SDL_DestroyTexture(goTex);

            // Draw button background
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderFillRect(renderer, &restartButton);

            // Draw text
            SDL_Color white = { 255, 255, 255, 255 };
            SDL_Surface* rsurf = TTF_RenderText_Solid(font, "Restart (R)", white);
            SDL_Texture* rtex = SDL_CreateTextureFromSurface(renderer, rsurf);
            SDL_Rect rrect = { restartButton.x + (restartButton.w - rsurf->w) / 2,
                               restartButton.y + (restartButton.h - rsurf->h) / 2,
                               rsurf->w, rsurf->h };
            SDL_RenderCopy(renderer, rtex, NULL, &rrect);
            SDL_FreeSurface(rsurf);
            SDL_DestroyTexture(rtex);
        }

        if (onStartScreen) {
            // Fill screen black
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderFillRect(renderer, NULL);

            // Draw title
            SDL_Color white = { 255,255,255,255 };
            SDL_Surface* titleSurf = TTF_RenderText_Solid(font, "TETRIS", white);
            SDL_Texture* titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
            SDL_Rect titleRect = {
                (WINDOW_WIDTH - titleSurf->w) / 2,
                WINDOW_HEIGHT / 2 - 100,
                titleSurf->w,
                titleSurf->h
            };
            SDL_RenderCopy(renderer, titleTex, NULL, &titleRect);
            SDL_FreeSurface(titleSurf);
            SDL_DestroyTexture(titleTex);

            // Draw Start button
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
            SDL_RenderFillRect(renderer, &startButton);

            SDL_Surface* startSurf = TTF_RenderText_Solid(font, "START", white);
            SDL_Texture* startTex = SDL_CreateTextureFromSurface(renderer, startSurf);
            SDL_Rect startRect = {
                startButton.x + (startButton.w - startSurf->w) / 2,
                startButton.y + (startButton.h - startSurf->h) / 2,
                startSurf->w,
                startSurf->h
            };
            SDL_RenderCopy(renderer, startTex, NULL, &startRect);
            SDL_FreeSurface(startSurf);
            SDL_DestroyTexture(startTex);

            SDL_RenderPresent(renderer);
            continue;  // IMPORTANT: skip drawing the game
        }

        // Present renderer
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

