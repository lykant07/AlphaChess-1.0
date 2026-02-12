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
    int numMoves;
    MoveInstance* LegalMoves;
} LegalMovesInstance;

typedef struct{
    bool WhiteKingside;
    bool WhiteQueenside;
    bool BlackKingside;
    bool BlackQueenside;
} CastlingDataInstance;

typedef struct {
    char capturedPiece;          // '.' if none
    SquareInstance capturedSq;   // needed for en passant

    SquareInstance from;
    SquareInstance to;

    char movedPiece;

    // Game state
    SquareInstance oldEnPassant;
    CastlingDataInstance oldCastling;
    int oldHalfMoves;
    int oldFullMoves;
    int oldEval;
    bool oldGameFinished;

    SquareInstance oldWhiteKing;
    SquareInstance oldBlackKing;
} MoveUndo;

typedef struct{
    MoveUndo* stack;
    int top;
    int maxSize;
} UndoStackInstance;

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
void MakeMove(MatchDataInstance* md, MoveInstance move, UndoStackInstance* undoStack);
void UnmakeMove(MatchDataInstance* md, UndoStackInstance* undoStack);
bool PlayMove(MatchDataInstance* matchData, MoveInstance move, UndoStackInstance* undoStack);
bool PlayMoves(MatchDataInstance* matchData, char* moves, UndoStackInstance* undoStack);
LegalMovesInstance GetLegalMoves(MatchDataInstance* matchData, SquareInstance square);
MoveInstance GetBestMove(MatchDataInstance* matchData, UndoStackInstance* undoStack);