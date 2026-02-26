#include "raylib.h"
#include <stdio.h>

#define SCREEN_SIZE 900
#define UI_HEIGHT 70
#define CELL_SIZE 100
#define EMPTY '.'

Color thinLine  = (Color){170, 170, 170, 255};
Color thickLine = (Color){60, 60, 60, 255};

Color xColor    = (Color){200, 50, 60, 255};
Color oColor    = (Color){40, 110, 220, 255};
Color ruleColor = (Color){70,70,90,255};

/* ========= GAME STATES ========= */
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_SWAP 2

/* ========= MODES ========= */
#define MODE_NORMAL 0
#define MODE_MISERE 1

/* ========= GLOBAL DATA ========= */
char sb[3][3][3][3];
char bb[3][3];

char currentPlayer = 'X';
int nextRow = -1, nextCol = -1;
int freeMove = 1;

int gameState = STATE_MENU;
int gameMode = MODE_NORMAL;

int gameOver = 0;
char winner = EMPTY;

/* Swap system */
int xSwapUsed = 0, oSwapUsed = 0;
int swapSourceRow = -1, swapSourceCol = -1;
int swapTargetRow = -1, swapTargetCol = -1;
int swapAnimating = 0;
float swapTimer = 0.0f;
float swapDuration = 0.35f;

/* Big win tracking */
int winLineType = -1;  // 0=row,1=col,2=diag1,3=diag2
int winLineIndex = -1;

/* ========= SWAP POPUP ========= */
int showSwapPopup = 0;
float swapPopupTimer = 0.0f;
float swapPopupDuration = 2.5f;
/* ========= INITIALIZE ========= */

void initialize()
{
    for(int br=0;br<3;br++)
        for(int bc=0;bc<3;bc++)
        {
            bb[br][bc] = EMPTY;
            for(int cr=0;cr<3;cr++)
                for(int cc=0;cc<3;cc++)
                    sb[br][bc][cr][cc] = EMPTY;
        }
   
    currentPlayer = 'X';
    nextRow = nextCol = -1;
    freeMove = 1;
    gameOver = 0;
    winner = EMPTY;

    xSwapUsed = oSwapUsed = 0;

    swapSourceRow = swapSourceCol = -1;
    swapTargetRow = swapTargetCol = -1;
    swapAnimating = 0;

    winLineType = -1;
    winLineIndex = -1;

}

/* ========= WIN CHECKS ========= */

int checkSmallWin(int sr,int sc)
{
    char (*b)[3] = sb[sr][sc];

    for(int i=0;i<3;i++)
        if(b[i][0]!=EMPTY && b[i][0]==b[i][1] && b[i][1]==b[i][2])
            return 1;

    for(int i=0;i<3;i++)
        if(b[0][i]!=EMPTY && b[0][i]==b[1][i] && b[1][i]==b[2][i])
            return 1;

    if(b[0][0]!=EMPTY && b[0][0]==b[1][1] && b[1][1]==b[2][2])
        return 1;

    if(b[0][2]!=EMPTY && b[0][2]==b[1][1] && b[1][1]==b[2][0])
        return 1;

    return 0;
}

int checkBigWin()
{
    for(int i=0;i<3;i++)
    {
        if(bb[i][0]!=EMPTY && bb[i][0]!='D' &&
           bb[i][0]==bb[i][1] && bb[i][1]==bb[i][2])
        {
            winLineType = 0;
            winLineIndex = i;
            return 1;
        }
    }

    for(int i=0;i<3;i++)
    {
        if(bb[0][i]!=EMPTY && bb[0][i]!='D' &&
           bb[0][i]==bb[1][i] && bb[1][i]==bb[2][i])
        {
            winLineType = 1;
            winLineIndex = i;
            return 1;
        }
    }

    if(bb[0][0]!=EMPTY && bb[0][0]!='D' &&
       bb[0][0]==bb[1][1] && bb[1][1]==bb[2][2])
    {
        winLineType = 2;
        return 1;
    }

    if(bb[0][2]!=EMPTY && bb[0][2]!='D' &&
       bb[0][2]==bb[1][1] && bb[1][1]==bb[2][0])
    {
        winLineType = 3;
        return 1;
    }

    return 0;
}

int checkBigFull()
{
    for(int r = 0; r < 3; r++)
        for(int c = 0; c < 3; c++)
            if(bb[r][c] == EMPTY)
                return 0;

    return 1;
}

int checkSmallFull(int sr, int sc)
{
    for(int r=0;r<3;r++)
        for(int c=0;c<3;c++)
            if(sb[sr][sc][r][c] == EMPTY)
                return 0;

    return 1;
}


/* ========= DRAWING ========= */

void drawCentered(const char* text,int y,int size,Color color)
{
    int w = MeasureText(text,size);
    DrawText(text,(SCREEN_SIZE-w)/2,y,size,color);
}

void drawMenu()
{
    ClearBackground((Color){235, 238, 245, 255});

    drawCentered("STRATEGIC TIC TAC TOE", 120, 60, DARKPURPLE);

    drawCentered("HOW TO PLAY", 230, 42, PURPLE);

drawCentered("i. Win a small board to claim it", 290, 30, ruleColor);
drawCentered("ii. Your move decides the next board", 330, 30, ruleColor);
drawCentered("iii. Each player gets ONE swap", 370, 30, ruleColor);
drawCentered("iv. Misere Mode: 3 in a row loses", 410, 30, ruleColor);

DrawLine(200, 450, 700, 450, (Color){180,180,200,255});

   DrawRectangle(170, 470, 560, 150,
              (Color){40,42,55,255});

drawCentered("Press 1 - NORMAL MODE", 515, 34, (Color){90,230,130,255});
drawCentered("Press 2 - MISERE MODE", 565, 34, (Color){230,90,90,255});
}

void drawUI()
{
    DrawRectangle(0,0,SCREEN_SIZE,UI_HEIGHT,LIGHTGRAY);

    DrawText(TextFormat("Turn: %c",currentPlayer),
             20,20,20,BLACK);

    DrawText(TextFormat("X Swap: %s",xSwapUsed?"Used":"Ready"),
             200,20,20,RED);

    DrawText(TextFormat("O Swap: %s",oSwapUsed?"Used":"Ready"),
             400,20,20,BLUE);
}

void drawBoard()
{
    /* ---- Swap Source Highlight ---- */
if(gameState == STATE_SWAP)
{
    DrawRectangle(
        swapSourceCol * 300,
        UI_HEIGHT + swapSourceRow * 300,
        300,
        300,
        Fade(GREEN, 0.35f)
    );
}

/* ---- Swap Target Highlight (animation) ---- */
if(swapAnimating)
{
    DrawRectangle(
        swapTargetCol * 300,
        UI_HEIGHT + swapTargetRow * 300,
        300,
        300,
        Fade(LIME, 0.45f)
    );
}

    /* ---- Forced Board Highlight ---- */
    if(!freeMove && nextRow >= 0 && nextCol >= 0)
    {
        DrawRectangle(
            nextCol * 300,
            UI_HEIGHT + nextRow * 300,
            300,
            300,
            Fade(YELLOW, 0.25f)
        );
    }

    /* ---- Thin Lines ---- */
    for(int i = 0; i <= 9; i++)
    {
        if(i % 3 != 0)
        {
            // vertical
            DrawLine(
                i * CELL_SIZE,
                UI_HEIGHT,
                i * CELL_SIZE,
                UI_HEIGHT + SCREEN_SIZE,
                thinLine
            );

            // horizontal
            DrawLine(
                0,
                UI_HEIGHT + i * CELL_SIZE,
                SCREEN_SIZE,
                UI_HEIGHT + i * CELL_SIZE,
                thinLine
            );
        }
    }

    /* ---- Thick Lines ---- */
    for(int i = 0; i <= 9; i += 3)
    {
        // vertical thick
        DrawRectangle(
            i * CELL_SIZE - 2,
            UI_HEIGHT,
            4,
            SCREEN_SIZE,
            thickLine
        );

        // horizontal thick
        DrawRectangle(
            0,
            UI_HEIGHT + i * CELL_SIZE - 2,
            SCREEN_SIZE,
            4,
            thickLine
        );
    }
}

void drawMarks()
{
    for(int br=0;br<3;br++)
        for(int bc=0;bc<3;bc++)
            for(int cr=0;cr<3;cr++)
                for(int cc=0;cc<3;cc++)
                {
                    char m = sb[br][bc][cr][cc];
                    if(m==EMPTY) continue;

                    int x = bc*300 + cc*CELL_SIZE;
                    int y = br*300 + cr*CELL_SIZE + UI_HEIGHT;

                    if(m=='X')
                    {
                        DrawLineEx((Vector2){x+20,y+20},
                                   (Vector2){x+80,y+80},5,xColor);
                        DrawLineEx((Vector2){x+80,y+20},
                                   (Vector2){x+20,y+80},5,xColor);
                    }
                    else
                    {
                        DrawRing((Vector2){x+50,y+50},
                                 30,36,0,360,64,oColor);
                    }
                }
}

void drawBigBoardMarks()
{
    for(int br = 0; br < 3; br++)
    {
        for(int bc = 0; bc < 3; bc++)
        {
            if(bb[br][bc] == 'X' || bb[br][bc] == 'O')
            {
                int startX = bc * 300;
                int startY = UI_HEIGHT + br * 300;
                int padding = 40;

                if(bb[br][bc] == 'X')
                {
                    DrawLineEx(
                        (Vector2){startX + padding, startY + padding},
                        (Vector2){startX + 300 - padding, startY + 300 - padding},
                        10,
                        MAROON
                    );

                    DrawLineEx(
                        (Vector2){startX + 300 - padding, startY + padding},
                        (Vector2){startX + padding, startY + 300 - padding},
                        10,
                        MAROON
                    );
                }
                else
                {
                    DrawRing(
                        (Vector2){startX + 150, startY + 150},
                        100 - 12,
                        100,
                        0, 360,
                        64,
                        DARKBLUE
                    );
                }
            }
        }
    }
}


void drawWinLine()
{
    if(!gameOver || winner==EMPTY) return;

    int bottom = UI_HEIGHT + SCREEN_SIZE;

    if(winLineType==0) // row
        DrawLineEx((Vector2){0, UI_HEIGHT + winLineIndex*300 + 150},
                   (Vector2){SCREEN_SIZE, UI_HEIGHT + winLineIndex*300 + 150},
                   8, DARKGREEN);

    else if(winLineType==1) // col
        DrawLineEx((Vector2){winLineIndex*300 + 150, UI_HEIGHT},
                   (Vector2){winLineIndex*300 + 150, bottom},
                   8, DARKGREEN);

    else if(winLineType==2)
        DrawLineEx((Vector2){0, UI_HEIGHT},
                   (Vector2){SCREEN_SIZE, bottom},
                   8, DARKGREEN);

    else if(winLineType==3)
        DrawLineEx((Vector2){SCREEN_SIZE, UI_HEIGHT},
                   (Vector2){0, bottom},
                   8, DARKGREEN);
}

void drawGameOver()
{
     if(!gameOver) return;  
     
     // Dark overlay background
    DrawRectangle(200,
                  UI_HEIGHT + 320,
                  500,
                  150,
                  Fade(BLACK, 0.8f));

    if(winner == EMPTY)
    {
        drawCentered("DRAW",
                     UI_HEIGHT + 360,
                     32,
                     WHITE);
    }
    else if(gameMode == MODE_NORMAL)
    {
        drawCentered(TextFormat("Player %c Wins!", winner),
                     UI_HEIGHT + 360,
                     32,
                     WHITE);
    }
    else
    {
        drawCentered(TextFormat("Player %c Loses!", winner),
                     UI_HEIGHT + 360,
                     32,
                     WHITE);
    }
}

void drawSwapPopup()
{
    if(!showSwapPopup) return;

   DrawRectangle(150,
              UI_HEIGHT + 280,
              600,
              200,
              Fade(BLACK, 0.85f));

    drawCentered(
        TextFormat("Player %c - Select Board to Swap",
                   currentPlayer),
        UI_HEIGHT + 340,
        32,
        ORANGE
    );
}


/* ========= MAIN ========= */

int main()
{
    InitWindow(SCREEN_SIZE,SCREEN_SIZE + UI_HEIGHT,
               "Strategic Tic Tac Toe");
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        if(gameState==STATE_MENU)
        {
            if(IsKeyPressed(KEY_ONE))
            {
                gameMode=MODE_NORMAL;
                initialize();
                gameState=STATE_PLAYING;
            }
            if(IsKeyPressed(KEY_TWO))
            {
                gameMode=MODE_MISERE;
                initialize();
                gameState=STATE_PLAYING;
            }
        }

       if(gameState == STATE_PLAYING && !gameOver)
{
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        int gRow = (mouse.y - UI_HEIGHT) / CELL_SIZE;
        int gCol = mouse.x / CELL_SIZE;

        if(gRow >= 0 && gRow < 9 && gCol >= 0 && gCol < 9)
        {
            int sbRow = gRow / 3;
            int sbCol = gCol / 3;
            int cRow  = gRow % 3;
            int cCol  = gCol % 3;

            int valid = freeMove ||
                        (sbRow == nextRow && sbCol == nextCol);

            if(valid &&
               sb[sbRow][sbCol][cRow][cCol] == EMPTY)
            {
                sb[sbRow][sbCol][cRow][cCol] = currentPlayer;

                /* Small win */
                if(checkSmallWin(sbRow, sbCol))
                {
                    bb[sbRow][sbCol] = currentPlayer;

                    int canSwap =
                        (currentPlayer == 'X' && !xSwapUsed) ||
                        (currentPlayer == 'O' && !oSwapUsed);

                    if(canSwap)
                    {
                        swapSourceRow = sbRow;
                        swapSourceCol = sbCol;
                        gameState = STATE_SWAP;

                        showSwapPopup = 1;
                        swapPopupTimer = 0.0f;
                    }
                }
                else if(checkSmallFull(sbRow, sbCol))
{
    bb[sbRow][sbCol] = 'D';   // mark small board as draw
}

nextRow = cRow;
nextCol = cCol;

/* If next board is won or drawn : free move */
if(bb[nextRow][nextCol] != EMPTY)
{
    freeMove = 1;
    nextRow = -1;
    nextCol = -1;
}
else
{
    freeMove = 0;
}

                /* Big win */
                if(checkBigWin())
{
    gameOver = 1;

    if(gameMode == MODE_NORMAL)
        winner = currentPlayer;
    else
        winner = (currentPlayer == 'X') ? 'O' : 'X';
}
else if(checkBigFull())
{
    gameOver = 1;
    winner = EMPTY;   // Means draw
}


                if(gameState == STATE_PLAYING && !gameOver)
                {
                    currentPlayer =
                        (currentPlayer == 'X') ? 'O' : 'X';
                         
                }
            }
        }
    }
}

if(gameState == STATE_SWAP)
{
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();

        int tr = (mouse.y - UI_HEIGHT) / 300;
        int tc = mouse.x / 300;

        if(tr >= 0 && tr < 3 &&
           tc >= 0 && tc < 3 &&
           !(tr == swapSourceRow &&
             tc == swapSourceCol))
        {
            swapTargetRow = tr;
            swapTargetCol = tc;
            swapAnimating = 1;
            swapTimer = 0;
        }
    }
}

if(swapAnimating)
{
    swapTimer += GetFrameTime();

    if(swapTimer >= swapDuration)
    {
         printf("Swapping (%d,%d) with (%d,%d)\n",
           swapSourceRow, swapSourceCol,
           swapTargetRow, swapTargetCol);
        /* Swap bb */
        char tempBB =
            bb[swapTargetRow][swapTargetCol];
        bb[swapTargetRow][swapTargetCol] =
            bb[swapSourceRow][swapSourceCol];
        bb[swapSourceRow][swapSourceCol] = tempBB;

        /* Swap sb */
        for(int r=0;r<3;r++)
            for(int c=0;c<3;c++)
            {
                char t =
                    sb[swapTargetRow][swapTargetCol][r][c];
                sb[swapTargetRow][swapTargetCol][r][c] =
                    sb[swapSourceRow][swapSourceCol][r][c];
                sb[swapSourceRow][swapSourceCol][r][c] = t;
            }

        if(currentPlayer == 'X')
            xSwapUsed = 1;
        else
            oSwapUsed = 1;

        if(checkBigWin())
{
    gameOver = 1;

    if(gameMode == MODE_NORMAL)
        winner = currentPlayer;
    else
        winner = (currentPlayer == 'X') ? 'O' : 'X';
}

        /* Reset forced move after swap */
        nextRow = -1;
        nextCol = -1;
        freeMove = 1;

        swapAnimating = 0;
        gameState = STATE_PLAYING;

        if(!gameOver){
            currentPlayer =
                (currentPlayer == 'X') ? 'O' : 'X';
        }
    }
}

if(gameOver)
{
    if(IsKeyPressed(KEY_R))
    {
        initialize();
        gameState = STATE_PLAYING;
    }

    if(IsKeyPressed(KEY_M))
    {
        gameState = STATE_MENU;
        gameOver = 0;
    }
}

        /* -------- Swap Popup Timer -------- */
if(showSwapPopup)
{
    swapPopupTimer += GetFrameTime();

    if(swapPopupTimer >= swapPopupDuration)
        showSwapPopup = 0;
}

        BeginDrawing();

        if(gameState==STATE_MENU)
            drawMenu();
        else
        {
            ClearBackground((Color){235, 238, 245, 255});
            drawUI();
            drawBoard();
            drawBigBoardMarks();
            drawMarks();
            drawWinLine();
            drawSwapPopup();
            drawGameOver();

        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
