
// A tetris port by Will Leingang
// Piece rotation as explained by https://www.youtube.com/watch?v=yIpk5TJ_uaI

#include "vex.h"

using namespace vex;
vex::brain Brain;
controller Controller = controller();
constexpr bool isDebugging = false;
int Brain_precision = 0, Console_precision = 0;
constexpr int WIDTH = 10;
constexpr int HEIGHT = 20;
constexpr int CELL = 5;
int grid[HEIGHT][WIDTH] = {{0}};
int score = 0;
int delay = 750; // 750ms is a nice slow starting speed
bool gameOver = false;
constexpr int PIECE_SIZE = 4;
const enum SHAPE { I, O, S, Z, L, J, T };
int level = score < 1000 ? 1 : score/1000;
struct Tetrimino {
    int grid[PIECE_SIZE][PIECE_SIZE];
};
int currentX=3, currentY=0, currentRotation = 0;
Tetrimino currentPiece, nextPiece;
SHAPE currentShape;
SHAPE nextShape;

/*
I locations:						
  {{{0, 0, 0, 0},		  {{{0, 0, 1, 0},		  {{{0, 0, 0, 0},		  {{{0, 1, 0, 0},
    {1, 1, 1, 1},		    {0, 0, 1, 0},		    {0, 0, 0, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 0, 1, 0},		    {1, 1, 1, 1},		    {0, 1, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 1, 0}}}		  {0, 0, 0, 0}}}		  {0, 1, 0, 0}}}
						
L locations:						
  {{{0, 0, 1, 0},		  {{{0, 1, 0, 0},		  {{{0, 0, 0, 0},		  {{{1, 1, 0, 0},
    {1, 1, 1, 0},		    {0, 1, 0, 0},		    {1, 1, 1, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 1, 1, 0},		    {1, 0, 0, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}
						
T locations:						
  {{{0, 1, 0, 0},		  {{{0, 1, 0, 0},		  {{{0, 0, 0, 0},		  {{{0, 1, 0, 0},
    {1, 1, 1, 0},		    {0, 1, 1, 0},		    {1, 1, 1, 0},		    {1, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 1, 0, 0},		    {0, 1, 0, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}
						
Z locations:						
  {{{1, 1, 0, 0},		  {{{0, 0, 1, 0},		  {{{0, 0, 0, 0},		  {{{0, 1, 0, 0},
    {0, 1, 1, 0},		    {0, 1, 1, 0},		    {1, 1, 0, 0},		    {1, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 1, 0, 0},		    {0, 1, 1, 0},		    {1, 0, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}
						
S locations:						
  {{{0, 1, 1, 0},		  {{{0, 1, 0, 0},		  {{{0, 0, 0, 0},		  {{{1, 0, 0, 0},
    {1, 1, 0, 0},		    {0, 1, 1, 0},		    {0, 1, 1, 0},		    {1, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 0, 1, 0},		    {1, 1, 0, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}
						
J locations:						
  {{{1, 0, 0, 0},		  {{{0, 1, 1, 0},		  {{{0, 0, 0, 0},		  {{{0, 1, 0, 0},
    {1, 1, 1, 0},		    {0, 1, 0, 0},		    {1, 1, 1, 0},		    {0, 1, 0, 0},
    {0, 0, 0, 0},		    {0, 1, 0, 0},		    {0, 0, 1, 0},		    {1, 1, 0, 0},
    {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}		  {0, 0, 0, 0}}}
						
O locations:						
  {{{0, 1, 1, 0},						
    {0, 1, 1, 0},						
    {0, 0, 0, 0},						
    {0, 0, 0, 0}}}						
*/


Tetrimino getPiece(SHAPE shape, int rotation) {
  if (shape == SHAPE::I && rotation == 0)
    return Tetrimino{{{0, 0, 0, 0},
                      {1, 1, 1, 1},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::I && rotation == 90)
    return Tetrimino{{{0, 0, 1, 0},
                      {0, 0, 1, 0},
                      {0, 0, 1, 0},
                      {0, 0, 1, 0}}};
  else if (shape == SHAPE::I && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {0, 0, 0, 0},
                      {1, 1, 1, 1},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::I && rotation == 270)
    return Tetrimino{{{0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 1, 0, 0}}};
  else if (shape == SHAPE::L && rotation == 0)
    return Tetrimino{{{0, 0, 1, 0},
                      {1, 1, 1, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::L && rotation == 90)
    return Tetrimino{{{0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 1, 1, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::L && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {1, 1, 1, 0},
                      {1, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::L && rotation == 270)
    return Tetrimino{{{1, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::T && rotation == 0)
    return Tetrimino{{{0, 1, 0, 0},
                      {1, 1, 1, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::T && rotation == 90)
    return Tetrimino{{{0, 1, 0, 0},
                      {0, 1, 1, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::T && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {1, 1, 1, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::T && rotation == 270)
    return Tetrimino{{{0, 1, 0, 0},
                      {1, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::Z && rotation == 0)
    return Tetrimino{{{1, 1, 0, 0},
                      {0, 1, 1, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::Z && rotation == 90)
    return Tetrimino{{{0, 0, 1, 0},
                      {0, 1, 1, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::Z && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {1, 1, 0, 0},
                      {0, 1, 1, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::Z && rotation == 270)
    return Tetrimino{{{0, 1, 0, 0},
                      {1, 1, 0, 0},
                      {1, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::S && rotation == 0)
    return Tetrimino{{{0, 1, 1, 0},
                      {1, 1, 0, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::S && rotation == 90)
    return Tetrimino{{{0, 1, 0, 0},
                      {0, 1, 1, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::S && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {0, 1, 1, 0},
                      {1, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::S && rotation == 270)
    return Tetrimino{{{1, 0, 0, 0},
                      {1, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::J && rotation == 0)
    return Tetrimino{{{1, 0, 0, 0},
                      {1, 1, 1, 0},
                      {0, 0, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::J && rotation == 90)
    return Tetrimino{{{0, 1, 1, 0},
                      {0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::J && rotation == 180)
    return Tetrimino{{{0, 0, 0, 0},
                      {1, 1, 1, 0},
                      {0, 0, 1, 0},
                      {0, 0, 0, 0}}};
  else if (shape == SHAPE::J && rotation == 270)
    return Tetrimino{{{0, 1, 0, 0},
                      {0, 1, 0, 0},
                      {1, 1, 0, 0},
                      {0, 0, 0, 0}}};

  // otherwise just return O
  return Tetrimino {{
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}}};
}

template <typename... Args>
void debug(const Args&... args) {
  if (isDebugging) {
    Brain.Screen.setFont(vex::fontType::mono12);
    Brain.Screen.printAt(60, 80, "                  ");
    Brain.Screen.printAt(60, 80, args...);
  }
}

void drawGameBoard() {
  // Outline
  Brain.Screen.drawRectangle(4, 0, WIDTH * CELL + 1, HEIGHT * CELL + 1);

  // Score
  Brain.Screen.printAt(60, 10, "Level:");

  // Level
  Brain.Screen.printAt(60, 25, "Score:");

  // Next Piece
  Brain.Screen.printAt(60, 40, "Next block:");
}

void renderScore() {
  Brain.Screen.setFont(vex::fontType::mono12);
  Brain.Screen.printAt(97, 25, "%d", score);
}

void renderLevel() {
  Brain.Screen.setFont(vex::fontType::mono12);
  Brain.Screen.printAt(97, 10, "%d", level);
}

void resetClipRegion() {
  Brain.Screen.setClipRegion(0, 0, 160, 100);
}

void renderNextPiece() {
  // Focus & clear the "Next block" region
  Brain.Screen.setClipRegion(60, 45, 20, 20);
  Brain.Screen.clearScreen();
  
  // Render the piece in the "Next block" region
  for (int i = 0; i < PIECE_SIZE;i++) {
    for (int j = 0; j < PIECE_SIZE; j++) {
      if (nextPiece.grid[i][j] == 1)
        Brain.Screen.drawRectangle(60 + j * 5, 45 + i * 5, CELL, CELL);
    }
  }
  
  // Expand the focused area
  resetClipRegion();
}

void render() {
  int offset = 5;
  // Focus & clear the grid boundary
  Brain.Screen.setClipRegion(5, 1, 49, 99);
  Brain.Screen.clearScreen();

  // Render the grid
  for (int iRow = 0; iRow < HEIGHT; iRow++) {
    for (int iCol = 0; iCol < WIDTH; iCol++){
      if (grid[iRow][iCol] > 0) {
        Brain.Screen.drawRectangle(offset + iCol * 5, iRow * 5, CELL, CELL);
      }
    }
  }

  // Expand the focused area
  resetClipRegion();
}

SHAPE getRandomShape() {
  int tmpA = static_cast<int>(SHAPE::I);
  int tmpB = static_cast<int>(SHAPE::T);
  int r = tmpA + rand() / (RAND_MAX / (tmpB - tmpA + 1));
  return static_cast<SHAPE>(r);
}

void debug_fillEntireGrid() {
  for (int iRow = 0; iRow < HEIGHT; iRow++) {
    for (int iCol = 0; iCol < WIDTH; iCol++){
      grid[iRow][iCol] = 1;
    }
  }
}

// Resets temp grid slots
void cleanGrid() {
  for (int iRow = 0; iRow < HEIGHT; iRow++) {
    for (int iCol = 0; iCol < WIDTH; iCol++){
      if (grid[iRow][iCol] == 1)
        grid[iRow][iCol] = 0;
    }
  }
}

bool canMoveDown() {
  for (int iRow = 0; iRow < PIECE_SIZE; iRow++) {
    for (int iCol = 0; iCol < PIECE_SIZE; iCol++) {
      if (currentPiece.grid[iRow][iCol] == 1) {
        int gridX = currentX + iCol;
        int gridY = currentY + iRow + 1; // cell directly below
        // is the piece about to touch the bottom?
        //debug("%d %d", gridY, HEIGHT);
        if (gridY >= HEIGHT) {
          return false;
        }
        
        // is there a piece already at that x/y?
        if (grid[gridY][gridX] > 1) {
          return false;
        }
      }
    }
  }
  return true;
}

bool calculateNewPosition(int newX, int newY, int newRotation) {
  Tetrimino newPiece = newRotation == currentRotation ? currentPiece : getPiece(currentShape, newRotation);
  for (int iRow = 0; iRow < PIECE_SIZE; iRow++) {
    for (int iCol = 0; iCol < PIECE_SIZE; iCol++) {
      if (newPiece.grid[iRow][iCol] == 1) {
        int gridX = newX + iCol;
        int gridY = newY + iRow;
        // is the piece about to touch the bottom?
        if (gridY >= HEIGHT) {
          return false;
        }
        // is the piece going off the grid?
        if (gridX < 0 || gridX >= WIDTH) {
          return false;
        }
        // is there a piece already at that x/y?
        if (grid[gridY][gridX] > 1) {
          return false;
        }
      }
    }
  }
  currentX = newX;
  currentY = newY;
  currentRotation = newRotation;
  currentPiece = newPiece;
  return true;
}

void movePiece() {
  cleanGrid();

  // Place the piece on the grid
  for (int iRow = 0; iRow < PIECE_SIZE; iRow++) {
    for (int iCol = 0; iCol < PIECE_SIZE; iCol++) {
      if (currentPiece.grid[iRow][iCol] == 1)
        grid[currentY + iRow][currentX + iCol] = 1;
    }
  }
  render();
}
void clearRows() {
    int rowsCleared = 0;
    for (int iRow = 0; iRow < HEIGHT; iRow++) {
        bool rowFilled = true;
        for (int iCol = 0; iCol < WIDTH; ++iCol) {
            if (grid[iRow][iCol] != 2) { 
                rowFilled = false;
                break;
            }
        }
        if (rowFilled) {
            rowsCleared++;
            // Shift rows down
            for (int moveRow = iRow; moveRow > 0; moveRow--) {
                for (int iCol = 0; iCol < WIDTH; ++iCol) {
                    grid[moveRow][iCol] = grid[moveRow - 1][iCol];
                }
            }
            // Clear the top row
            for (int iCol = 0; iCol < WIDTH; ++iCol) {
                grid[0][iCol] = 0;
            }
            // Since we just moved all rows down, we need to check the current row again
            iRow--;
        }
    }
    
    score += rowsCleared * 250;
    if (score > 1000)
      level = score / 1000;

    renderScore();
}

void next() {
  if (currentY > 0) {
    // Lock the piece at its current spot
    for (int iRow = 0; iRow < HEIGHT; iRow++) {
      for (int iCol = 0; iCol < WIDTH; iCol++){
        if (grid[iRow][iCol] == 1)
          grid[iRow][iCol] = 2;
      }
    }
    clearRows();
    currentShape = nextShape;
    currentPiece = nextPiece;
  } else {
    currentShape = getRandomShape();
    currentPiece = getPiece(currentShape, 0);
  }
  
  currentX = 3;
  currentY = 0;
  currentRotation = 0;
  nextShape = getRandomShape();
  nextPiece = getPiece(nextShape, 0);
  renderNextPiece();
  renderLevel();
}

void down() {
  if (canMoveDown()){
      currentY++;
      movePiece();
  } else if (currentY == 0) {
    gameOver = true;
  } else
    next();
}

void soundTrack(){
  /*
  Brain.playNote(6, 0, 50); // C
  Brain.playNote(6, 1, 50); // D
  Brain.playNote(6, 2, 50); // E
  Brain.playNote(6, 3, 50); // F
  Brain.playNote(6, 4, 50); // G
  Brain.playNote(6, 5, 50); // A
  Brain.playNote(6, 6, 50); // B
  */
  int speed = 5;
  Brain.playNote(4, 2, speed * 100); // E
  Brain.playNote(4, 6, speed * 25); // B
  Brain.playNote(4, 0, speed * 25); // C
  Brain.playNote(3, 1, speed * 100); // D
  Brain.playNote(4, 0, speed * 25); // C
  Brain.playNote(4, 6, speed * 25); // B
  Brain.playNote(4, 5, speed * 125); // A
  Brain.playNote(4, 0, speed * 25); // C
  Brain.playNote(4, 2, speed * 100); // E
  Brain.playNote(3, 1, speed * 25); // D
  Brain.playNote(4, 0, speed * 25); // C
  Brain.playNote(4, 6, speed * 100); // B
  Brain.playNote(4, 0, speed * 25); // C
  Brain.playNote(3, 1, speed * 100); // D
  Brain.playNote(4, 2, speed * 100); // E
  Brain.playNote(4, 0, speed * 100); // C
  Brain.playNote(4, 5, speed * 25); // A
  Brain.playNote(4, 5, speed * 100); // A
}

int run() {
  drawGameBoard();
  srand(Brain.Timer.system());
  
  // The sound is awful, just don't.
  //thread background = thread(soundTrack);
  //debug_fillEntireGrid();

  next();
  renderScore();
  while (gameOver == false)
  {
    if (Brain.buttonCheck.pressing())
      continue;

    wait(delay - ((level-1) * 15), msec);
    down();
  }

  Brain.Screen.setFont(vex::fontType::mono30);
  Brain.Screen.printAt(15, 60, "Game Over");
  
  return 0;
}

void moveLeft() {
  if (calculateNewPosition(currentX - 1, currentY, currentRotation))
    movePiece();
}

void moveRight() {
  if (calculateNewPosition(currentX + 1, currentY, currentRotation))
    movePiece();
}

void rotateRight(){
  int newRotation = currentRotation == 270 ? 0 : currentRotation + 90;
  if (calculateNewPosition(currentX, currentY, newRotation))
    movePiece();
}

void rotateLeft(){
  int newRotation = currentRotation == 0 ? 270 : currentRotation - 90;
  if (calculateNewPosition(currentX, currentY, newRotation))
    movePiece();
}

void onevent_buttonLeft_pressed_0() {
  moveLeft();
}

void onevent_buttonRight_pressed_0() {
  moveRight();
}

void onEvent_event_timer_0() {
  //debug("timer exec");
  while(Brain.buttonCheck.pressing()) {
    //debug("holding");
    down();
    wait(100, msec);
  }
  //debug("timer complete");
}

void onEvent_check_pressed_0() {
  //debug("chk pressed");
  Brain.Timer.reset();
  Brain.Timer.event(onEvent_event_timer_0, 300);
}

void onEvent_check_released_0() {
  //debug("rlsd %f", Brain.Timer.time(msec));
  // Piece was dropping
  if (Brain.Timer.time(msec) > 300)
    return;
  
  Brain.Timer.clear();
  rotateRight();
}

void onEvent_buttonLDown_pressed_0(){
  down();
}

void onEvent_buttonLUp_pressed_0(){
  currentY--;
  movePiece();
}

void onEvent_buttonFUp_pressed_0(){
  rotateLeft();
}

void onEvent_buttonFDown_pressed_0(){
  rotateRight();
}

void onEvent_buttonRUp_pressed_0(){
  moveLeft();
}
void onEvent_buttonRDown_pressed_0(){
  moveRight();
}

int main() {
  // register event handlers
  Brain.buttonLeft.pressed(onevent_buttonLeft_pressed_0);
  Brain.buttonRight.pressed(onevent_buttonRight_pressed_0);
  Brain.buttonCheck.pressed(onEvent_check_pressed_0);
  Brain.buttonCheck.released(onEvent_check_released_0);
  Controller.ButtonLDown.pressed(onEvent_buttonLDown_pressed_0);
  Controller.ButtonLUp.pressed(onEvent_buttonLUp_pressed_0);
  Controller.ButtonRUp.pressed(onEvent_buttonRUp_pressed_0);
  Controller.ButtonRDown.pressed(onEvent_buttonRDown_pressed_0);
  Controller.ButtonFDown.pressed(onEvent_buttonFDown_pressed_0);
  Controller.ButtonFUp.pressed(onEvent_buttonFUp_pressed_0);

  Brain.Screen.setFont(vex::fontType::mono12);
  
  // initialize the random number system
  srand(Brain.Timer.system());

  run();
}

