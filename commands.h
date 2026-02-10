#include <stdbool.h>

typedef struct{
    char squares[8][8];
} BoardInstance;

typedef struct{ //col is alphanumeric, while row is numeric
    char col;
    int row;
} SquareInstance;

typedef struct{
    SquareInstance from;
    SquareInstance to;
    char promote;
} MoveInstance;

typedef struct{
    bool WhiteKingside;
    bool WhiteQueenside;
    bool BlackKingside;
    bool BlackQueenside;
} CastlingDataInstance;

typedef struct{
    BoardInstance Board;
    int eval;
    char playerTurn;
    bool whiteKingChecked;
    bool blackKingChecked;
    SquareInstance whiteKingSquare;
    SquareInstance blackKingSquare;
    CastlingDataInstance castlingRights;
    SquareInstance enPassant;
    int halfMoves;
    int fullMoves;
    bool gameFinished;
} MatchDataInstance;

void ResetBoard(MatchDataInstance* matchData);
void ShowBoard(BoardInstance Board);
char* GetFEN(MatchDataInstance* matchData);
bool LoadFEN(MatchDataInstance* matchData, char* FEN);
bool PlayMove(MatchDataInstance* matchData, MoveInstance move);
bool PlayMoves(MatchDataInstance* matchData, char* moves);
MoveInstance GetBestMove(MatchDataInstance matchData);