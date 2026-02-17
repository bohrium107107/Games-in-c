#include "raylib.h"

#define SCREEN_SIZE 900
#define UI_HEIGHT 120
#define CELL_SIZE 100
#define EMPTY '.'

/* ===== STATES ===== */
#define STATE_MENU 0
#define STATE_PLAYING 1
#define STATE_SWAP 2

/* ===== MODES ===== */
#define MODE_NORMAL 0
#define MODE_MISERE 1

char sb[3][3][3][3];
char bb[3][3];

char currentPlayer = 'X';

int nextRow = -1;
int nextCol = -1;
int freeMove = 1;

int gameState = STATE_MENU;
int gameMode = MODE_NORMAL;

int gameOver = 0;
char winner = EMPTY;

/* ===== Swap System ===== */
int xSwapUsed = 0;
int oSwapUsed = 0;

int swapSourceRow = -1;
int swapSourceCol = -1;

int swapTargetRow = -1;
int swapTargetCol = -1;

int swapAnimating = 0;
float swapTimer = 0.0f;
float swapDuration = 0.4f;

/* ================= INITIALIZE ================= */

void initialize()
{
    for(int br=0;br<3;br++)
    {
        for(int bc=0;bc<3;bc++)
        {
            bb[br][bc] = EMPTY;

            for(int cr=0;cr<3;cr++)
                for(int cc=0;cc<3;cc++)
                    sb[br][bc][cr][cc] = EMPTY;
        }
    }

    currentPlayer = 'X';
    nextRow = -1;
    nextCol = -1;
    freeMove = 1;

    gameOver = 0;
    winner = EMPTY;

    xSwapUsed = 0;
    oSwapUsed = 0;

    swapSourceRow = -1;
    swapSourceCol = -1;
    swapTargetRow = -1;
    swapTargetCol = -1;

    swapAnimating = 0;
    swapTimer = 0.0f;
}

/* ================= CHECK FUNCTIONS ================= */

int checkSmallWin(int sr,int sc)
{
    char (*board)[3] = sb[sr][sc];

    for(int i=0;i<3;i++)
        if(board[i][0]!=EMPTY &&
           board[i][0]==board[i][1] &&
           board[i][1]==board[i][2])
            return 1;

    for(int i=0;i<3;i++)
        if(board[0][i]!=EMPTY &&
           board[0][i]==board[1][i] &&
           board[1][i]==board[2][i])
            return 1;

    if(board[0][0]!=EMPTY &&
       board[0][0]==board[1][1] &&
       board[1][1]==board[2][2])
        return 1;

    if(board[0][2]!=EMPTY &&
       board[0][2]==board[1][1] &&
       board[1][1]==board[2][0])
        return 1;

    return 0;
}

int checkSmallFull(int sr,int sc)
{
    for(int r=0;r<3;r++)
        for(int c=0;c<3;c++)
            if(sb[sr][sc][r][c]==EMPTY)
                return 0;
    return 1;
}

int checkBigWin()
{
    for(int i=0;i<3;i++)
        if(bb[i][0]!=EMPTY && bb[i][0]!='D' &&
           bb[i][0]==bb[i][1] &&
           bb[i][1]==bb[i][2])
            return 1;

    for(int i=0;i<3;i++)
        if(bb[0][i]!=EMPTY && bb[0][i]!='D' &&
           bb[0][i]==bb[1][i] &&
           bb[1][i]==bb[2][i])
            return 1;

    if(bb[0][0]!=EMPTY && bb[0][0]!='D' &&
       bb[0][0]==bb[1][1] &&
       bb[1][1]==bb[2][2])
        return 1;

    if(bb[0][2]!=EMPTY && bb[0][2]!='D' &&
       bb[0][2]==bb[1][1] &&
       bb[1][1]==bb[2][0])
        return 1;

    return 0;
}

int checkBigFull()
{
    for(int r=0;r<3;r++)
        for(int c=0;c<3;c++)
            if(bb[r][c]==EMPTY)
                return 0;
    return 1;
}

/* ================= MAIN ================= */

int main()
{
    InitWindow(SCREEN_SIZE, SCREEN_SIZE+UI_HEIGHT,
               "Strategic Tic Tac Toe");
    SetTargetFPS(60);

    while(!WindowShouldClose())
    {
        /* ===== MENU ===== */
        if(gameState==STATE_MENU)
        {
            if(IsKeyPressed(KEY_ONE))
            {
                gameMode = MODE_NORMAL;
                initialize();
                gameState = STATE_PLAYING;
            }
            if(IsKeyPressed(KEY_TWO))
            {
                gameMode = MODE_MISERE;
                initialize();
                gameState = STATE_PLAYING;
            }
        }

        /* ===== PLAYING ===== */
        else if(gameState==STATE_PLAYING)
        {
            if(gameOver && IsKeyPressed(KEY_R))
                initialize();

            if(gameOver && IsKeyPressed(KEY_M))
                gameState = STATE_MENU;

            if(!gameOver &&
               IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mouse = GetMousePosition();

                int gRow = (mouse.y-UI_HEIGHT)/CELL_SIZE;
                int gCol = mouse.x/CELL_SIZE;

                if(gRow>=0 && gRow<9 &&
                   gCol>=0 && gCol<9)
                {
                    int sbRow = gRow/3;
                    int sbCol = gCol/3;
                    int cRow = gRow%3;
                    int cCol = gCol%3;

                    int valid = freeMove ||
                               (sbRow==nextRow &&
                                sbCol==nextCol);

                    if(valid &&
                       sb[sbRow][sbCol][cRow][cCol]==EMPTY)
                    {
                        sb[sbRow][sbCol][cRow][cCol] =
                            currentPlayer;

                        if(checkSmallWin(sbRow,sbCol))
                        {
                            bb[sbRow][sbCol] =
                                currentPlayer;

                            int canSwap =
                              (currentPlayer=='X' && !xSwapUsed) ||
                              (currentPlayer=='O' && !oSwapUsed);

                            if(canSwap)
                            {
                                swapSourceRow = sbRow;
                                swapSourceCol = sbCol;
                                gameState = STATE_SWAP;
                            }
                        }
                        else if(checkSmallFull(sbRow,sbCol))
                            bb[sbRow][sbCol] = 'D';

                        nextRow = cRow;
                        nextCol = cCol;
                        freeMove =
                          (bb[nextRow][nextCol]!=EMPTY);

                        if(checkBigWin())
                        {
                            gameOver=1;
                            winner=currentPlayer;
                        }
                        else if(checkBigFull())
                        {
                            gameOver=1;
                            winner=EMPTY;
                        }

                        if(gameState==STATE_PLAYING &&
                           !gameOver)
                            currentPlayer =
                              (currentPlayer=='X')?'O':'X';
                    }
                }
            }
        }

        /* ===== SWAP SELECTION ===== */
        else if(gameState==STATE_SWAP)
        {
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                Vector2 mouse = GetMousePosition();

                int tr = (mouse.y-UI_HEIGHT)/300;
                int tc = mouse.x/300;

                if(tr>=0 && tr<3 &&
                   tc>=0 && tc<3 &&
                   !(tr==swapSourceRow &&
                     tc==swapSourceCol))
                {
                    swapTargetRow=tr;
                    swapTargetCol=tc;
                    swapAnimating=1;
                    swapTimer=0;
                }
            }
        }

        /* ===== SWAP ANIMATION ===== */
        if(swapAnimating)
        {
            swapTimer+=GetFrameTime();

            if(swapTimer>=swapDuration)
            {
                char tempBB =
                    bb[swapTargetRow][swapTargetCol];
                bb[swapTargetRow][swapTargetCol] =
                    bb[swapSourceRow][swapSourceCol];
                bb[swapSourceRow][swapSourceCol]=tempBB;

                for(int r=0;r<3;r++)
                    for(int c=0;c<3;c++)
                    {
                        char t=
                          sb[swapTargetRow][swapTargetCol][r][c];
                        sb[swapTargetRow][swapTargetCol][r][c]=
                          sb[swapSourceRow][swapSourceCol][r][c];
                        sb[swapSourceRow][swapSourceCol][r][c]=t;
                    }

                if(currentPlayer=='X')
                    xSwapUsed=1;
                else
                    oSwapUsed=1;

                if(checkBigWin())
                {
                    gameOver=1;
                    winner=currentPlayer;
                }

                swapAnimating=0;
                gameState=STATE_PLAYING;

                if(!gameOver)
                    currentPlayer=
                      (currentPlayer=='X')?'O':'X';
            }
        }

        /* ===== DRAW ===== */
        BeginDrawing();

        if(gameState==STATE_MENU)
        {
            ClearBackground((Color){255,182,193,255});

            DrawText("Strategic Tic Tac Toe",
                     200,80,40,BLACK);

            DrawText("RULES:",100,160,30,DARKGRAY);
            DrawText("- Win small boards.",
                     120,200,20,BLACK);
            DrawText("- Forced board rule applies.",
                     120,230,20,BLACK);
            DrawText("- Each player gets ONE swap.",
                     120,260,20,MAROON);
            DrawText("- Misere: 3 in row loses.",
                     120,290,20,BLUE);

            DrawText("Press 1 - Normal Mode",
                     250,380,30,DARKGREEN);
            DrawText("Press 2 - Misere Mode",
                     250,430,30,MAROON);
        }
        else
        {
            ClearBackground(RAYWHITE);

            DrawRectangle(0,0,
                          SCREEN_SIZE,UI_HEIGHT,
                          LIGHTGRAY);

            DrawText(TextFormat("X Swap: %s",
                     xSwapUsed?"Used":"Available"),
                     20,20,20,RED);

            DrawText(TextFormat("O Swap: %s",
                     oSwapUsed?"Used":"Available"),
                     20,50,20,BLUE);

            if(!freeMove)
                DrawRectangle(nextCol*300,
                              nextRow*300+UI_HEIGHT,
                              300,300,
                              Fade(YELLOW,0.2f));

            if(gameState==STATE_SWAP)
                DrawRectangle(swapSourceCol*300,
                              swapSourceRow*300+UI_HEIGHT,
                              300,300,
                              Fade(GREEN,0.3f));

            if(swapAnimating)
            {
                float alpha=0.8f;

                DrawRectangle(swapSourceCol*300,
                              swapSourceRow*300+UI_HEIGHT,
                              300,300,
                              Fade(GOLD,alpha));

                DrawRectangle(swapTargetCol*300,
                              swapTargetRow*300+UI_HEIGHT,
                              300,300,
                              Fade(GOLD,alpha));
            }

            for(int i=0;i<=9;i++)
                if(i%3!=0)
                {
                    DrawLine(i*CELL_SIZE,
                             UI_HEIGHT,
                             i*CELL_SIZE,
                             SCREEN_SIZE+UI_HEIGHT,
                             BLACK);

                    DrawLine(0,
                             i*CELL_SIZE+UI_HEIGHT,
                             SCREEN_SIZE,
                             i*CELL_SIZE+UI_HEIGHT,
                             BLACK);
                }

            for(int i=0;i<=9;i+=3)
            {
                DrawRectangle(i*CELL_SIZE,
                              UI_HEIGHT,
                              4,SCREEN_SIZE,BLACK);

                DrawRectangle(0,
                              i*CELL_SIZE+UI_HEIGHT,
                              SCREEN_SIZE,4,BLACK);
            }

            for(int br=0;br<3;br++)
                for(int bc=0;bc<3;bc++)
                    for(int cr=0;cr<3;cr++)
                        for(int cc=0;cc<3;cc++)
                        {
                            char m=
                              sb[br][bc][cr][cc];
                            if(m!=EMPTY)
                            {
                                int x=bc*300+cc*CELL_SIZE;
                                int y=br*300+cr*CELL_SIZE
                                      +UI_HEIGHT;
                                int p=20;

                                if(m=='X')
                                {
                                    DrawLineEx(
                                      (Vector2){x+p,y+p},
                                      (Vector2){x+100-p,y+100-p},
                                      6,RED);

                                    DrawLineEx(
                                      (Vector2){x+100-p,y+p},
                                      (Vector2){x+p,y+100-p},
                                      6,RED);
                                }
                                else
                                {
                                    float r=50-p;
                                    DrawRing(
                                      (Vector2){x+50,y+50},
                                      r-8,r,
                                      0,360,64,BLUE);
                                }
                            }
                        }

            if(gameOver)
            {
                DrawRectangle(200,450,
                              500,100,
                              Fade(BLACK,0.7f));

                if(winner!=EMPTY)
                {
                    if(gameMode==MODE_NORMAL)
                        DrawText(TextFormat(
                          "Player %c Wins!",winner),
                          300,480,40,WHITE);
                    else
                        DrawText(TextFormat(
                          "Player %c Loses!",winner),
                          300,480,40,WHITE);
                }
                else
                    DrawText("Game Draw!",
                             350,480,40,WHITE);

                DrawText("Press R to Restart | M Menu",
                         230,520,20,WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
