# 🎮 Strategic Tic Tac Toe

A custom Ultimate Tic Tac Toe variant built in C using Raylib.

A strategic twist on the classic game with board control, swap powers, and multiple game modes.

---

## 🚀 Features

-  Normal Mode  
-  Misère Mode (3 in a row loses)  
-  One-time Board Swap   
-  Smooth Swap Animation  
-  Forced Board Mechanic  
-  Big Board Win Detection  
-  Clean UI with Turn & Swap Indicators  

---

##  How It Works

###  Forced Board Rule
Your move determines which small board your opponent must play in next.  
If that board is already completed, your opponent gets a free move.

###  Swap Power
When you win a small board:
- You may swap it with any other board.
- Each player can use this power only once per game.
- Includes visual animation.

---

## 🏁 Game Modes

| Mode   | Description |
|--------|------------|
| Normal | Get 3 big boards in a row to win |
| Misère | Getting 3 in a row makes you lose |

---

## 🕹 Controls

- Mouse Click → Place mark  
- 1 → Start Normal Mode  
- 2 → Start Misère Mode  
- R → Restart Game  
- M → Return to Menu  

---

## 🛠 Built With

- C Language  
- Raylib Graphics Library  
