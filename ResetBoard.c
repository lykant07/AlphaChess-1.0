#include "commands.h"
#include <stdbool.h>

char startingBoard[8][8] = {
{'r','n','b','q','k','b','n','r'},
{'p','p','p','p','p','p','p','p'},
{'.','.','.','.','.','.','.','.'},
{'.','.','.','.','.','.','.','.'},
{'.','.','.','.','.','.','.','.'},
{'.','.','.','.','.','.','.','.'},
{'P','P','P','P','P','P','P','P'},
{'R','N','B','Q','K','B','N','R'}
};

void ResetBoard(MatchDataInstance* matchData){
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            matchData -> Board.squares[i][j] = startingBoard[i][j];
        }
    }
    matchData -> eval = 0;
    matchData -> playerTurn = 'w';
    matchData -> whiteKingChecked = false;
    matchData -> blackKingChecked = false;
    matchData -> whiteKingSquare = (SquareInstance){'e', 1};
    matchData -> blackKingSquare = (SquareInstance){'e', 8};
    matchData -> castlingRights = (CastlingDataInstance){true, true, true, true};
    matchData -> enPassant = (SquareInstance){0,0};
    matchData -> halfMoves = 0;
    matchData -> fullMoves = 1;
    matchData -> gameFinished = false;
}