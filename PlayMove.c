#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"

typedef struct{
    int numMoves;
    MoveInstance* LegalMoves;
} LegalMovesInstance;

static bool isBlack(char ch){
    return (ch == 'p' || ch == 'r' || ch == 'b' || ch == 'n' || ch == 'k' || ch == 'q') ? true : false;
}

static bool isWhite(char ch){
    return (ch == 'P' || ch == 'R' || ch == 'B' || ch == 'N' || ch == 'K' || ch == 'Q') ? true : false;
}

static char lower(char ch){
    return (ch >= 'A' && ch <= 'Z') ? ch + 32 : ch;
}

static char upper(char ch){
    return (ch >= 'a' && ch <= 'z') ? ch - 32 : ch;
}

static bool isValidSquare(SquareInstance square){
    return (square.col >= 'a' && square.col <= 'h' && square.row >= 1 && square.row <= 8) ? true : false;
}

static bool isSameColor(BoardInstance Board, SquareInstance sq1, SquareInstance sq2){
    char piece1 = Board.squares[8 - sq1.row][sq1.col - 'a'];
    char piece2 = Board.squares[8 - sq2.row][sq2.col - 'a'];
    if (piece1 >= 'A' && piece1 <= 'Z' && piece2 >= 'A' && piece2 <= 'Z') return true;
    if (piece1 >= 'a' && piece1 <= 'z' && piece2 >= 'a' && piece2 <= 'z') return true;
    return false;
}

static bool areEnemyPieces(BoardInstance Board, SquareInstance sq1, SquareInstance sq2){
    char piece1 = Board.squares[8 - sq1.row][sq1.col - 'a'];
    char piece2 = Board.squares[8 - sq2.row][sq2.col - 'a'];
    if (piece1 >= 'A' && piece1 <= 'Z' && piece2 >= 'a' && piece2 <= 'z') return true;
    if (piece1 >= 'a' && piece1 <= 'z' && piece2 >= 'A' && piece2 <= 'Z') return true;
    return false;
}

static MoveInstance StringToMove(char* str){
    MoveInstance invalid = {{0,0},{0,0}, 0};
    if (str == NULL) return invalid;
    int len = strlen(str);
    if (len != 4 && len != 5) return invalid;
    
    SquareInstance square1 = {str[0], str[1] - '0'};
    SquareInstance square2 = {str[2], str[3] - '0'};
    MoveInstance move = {square1, square2, 0};
    if (isValidSquare(square1) && isValidSquare(square2)){
        if (len == 5){
            if (str[4] == 'q' || str[4] == 'r' || str[4] == 'b' || str[4] == 'n'){
                move.promote = str[4];
                return move;
            } else{
                return invalid;
            }
        } else{
            move.promote = 0;
            return move;
        }
    } else{
        return invalid;
    }
}

int GetPieceValue(char ch){
    switch(ch){
        case '.':
            return 0;
            break;
        case 'P':
            return -1;
            break;
        case 'R':
            return -5;
            break;
        case 'N':
            return -3;
            break;
        case 'B':
            return -3;
            break;
        case 'Q':
            return -9;
            break;
        case 'p':
            return +1;
            break;
        case 'r':
            return +5;
            break;
        case 'n':
            return +3;
            break;
        case 'b':
            return +3;
            break;
        case 'q':
            return +9;
            break;
        default:
            return 0;
            break;
    }
}

bool AttackingSquare(MatchDataInstance matchData, SquareInstance square, char color){
    int rowIndex =  8 - square.row;
    int colIndex = square.col - 'a';
    char (*Board)[8] = matchData.Board.squares;


    for (int j = colIndex+1; j < 8; j++){ //Check row (right side)
        if ((color == 'w' && isBlack(Board[rowIndex][j])) || (color == 'b' && isWhite(Board[rowIndex][j]))){
            break; // A same color piece is blocking the way
        } else if (color == 'w'){ //Check for white attackers
             if (Board[rowIndex][j] == 'R' || Board[rowIndex][j] == 'Q' || (Board[rowIndex][j] == 'K' && j == colIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[rowIndex][j] == 'r' || Board[rowIndex][j] == 'q' || (Board[rowIndex][j] == 'k' && j == colIndex+1)) return true;
        }
    }

    for (int j = colIndex-1; j >= 0; j--){ //Check row (left side)
        if ((color == 'w' && isBlack(Board[rowIndex][j])) || (color == 'b' && isWhite(Board[rowIndex][j]))){
            break; // A same color piece is blocking the way
        } else if (color == 'w'){ //Check for white attackers
             if (Board[rowIndex][j] == 'R' || Board[rowIndex][j] == 'Q' || (Board[rowIndex][j] == 'K' && j == colIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[rowIndex][j] == 'r' || Board[rowIndex][j] == 'q' || (Board[rowIndex][j] == 'k' && j == colIndex+1)) return true;
        }
    }

    for (int i = rowIndex-1; i >= 0; i--){ //Check column (top)
        if ((color == 'w' && isBlack(Board[i][colIndex])) || (color == 'b' && isWhite(Board[i][colIndex]))){
            break; // A same color piece is blocking the way
        } else if (color == 'w'){ //Check for white attackers
             if (Board[i][colIndex] == 'R' || Board[i][colIndex] == 'Q' || (Board[i][colIndex] == 'K' && i == rowIndex-1)) return true;
        } else{ //Check for black attackers
            if (Board[i][colIndex] == 'r' || Board[i][colIndex] == 'q' || (Board[i][colIndex] == 'k' && i == rowIndex-1)) return true;
        }
    }

    for (int i = rowIndex+1; i < 8; i++){ //Check column (bottom)
        if ((color == 'w' && isBlack(Board[i][colIndex])) || (color == 'b' && isWhite(Board[i][colIndex]))){
            break; // A same color piece is blocking the way
        } else if (color == 'w'){ //Check for white attackers
             if (Board[i][colIndex] == 'R' || Board[i][colIndex] == 'Q' || (Board[i][colIndex] == 'K' && i == rowIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[i][colIndex] == 'r' || Board[i][colIndex] == 'q' || (Board[i][colIndex] == 'k' && i == rowIndex+1)) return true;
        }
    }

    for (int k = 1; isValidSquare((SquareInstance){square.col-k, square.row-k}); k++){ //Below (/)
        if (color == 'w'){
            if (Board[rowIndex+k][colIndex-k] == 'B' || Board[rowIndex+k][colIndex-k] == 'Q' || ((Board[rowIndex+k][colIndex-k] == 'K' || Board[rowIndex+k][colIndex-k] == 'P') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex-k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex+k][colIndex-k] == 'b' || Board[rowIndex+k][colIndex-k] == 'q' || (Board[rowIndex+k][colIndex-k] == 'k' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex-k])) break; //Same color piece in the way
        }
    }
    for (int k = 1; isValidSquare((SquareInstance){square.col+k, square.row+k}); k++){ //Above (/)
        if (color == 'w'){
            if (Board[rowIndex-k][colIndex+k] == 'B' || Board[rowIndex-k][colIndex+k] == 'Q' || (Board[rowIndex-k][colIndex+k] == 'K' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex+k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex-k][colIndex+k] == 'b' || Board[rowIndex-k][colIndex+k] == 'q' || ((Board[rowIndex-k][colIndex+k] == 'k' || Board[rowIndex-k][colIndex+k] == 'p') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex+k])) break; //Same color piece in the way
        }
    }

    for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ //Above (\)
        if (color == 'w'){
            if (Board[rowIndex-k][colIndex-k] == 'B' || Board[rowIndex-k][colIndex-k] == 'Q' || (Board[rowIndex-k][colIndex-k] == 'K' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex-k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex-k][colIndex-k] == 'b' || Board[rowIndex-k][colIndex-k] == 'q' || ((Board[rowIndex-k][colIndex-k] == 'k' || Board[rowIndex-k][colIndex-k] == 'p') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex-k])) break; //Same color piece in the way
        }
    }
    for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){ //Below (\)
        if (color == 'w'){
            if (Board[rowIndex+k][colIndex+k] == 'B' || Board[rowIndex+k][colIndex+k] == 'Q' || ((Board[rowIndex+k][colIndex+k] == 'K' || Board[rowIndex+k][colIndex+k] == 'P') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex+k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex+k][colIndex+k] == 'b' || Board[rowIndex+k][colIndex+k] == 'q' || (Board[rowIndex+k][colIndex+k] == 'k' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex+k])) break; //Same color piece in the way
        }
    }

    //Check for knight threats :)
    if (isValidSquare((SquareInstance){square.col-1,square.row+2}) && ((Board[rowIndex-2][colIndex-1] == 'N' && color == 'w') || (Board[rowIndex-2][colIndex-1] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-2,square.row+1}) && ((Board[rowIndex-1][colIndex-2] == 'N' && color == 'w') || (Board[rowIndex-1][colIndex-2] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+1,square.row+2}) && ((Board[rowIndex-2][colIndex+1] == 'N' && color == 'w') || (Board[rowIndex-2][colIndex+1] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+2,square.row+1}) && ((Board[rowIndex-1][colIndex+2] == 'N' && color == 'w') || (Board[rowIndex-1][colIndex+2] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-1,square.row-2}) && ((Board[rowIndex+2][colIndex-1] == 'N' && color == 'w') || (Board[rowIndex+2][colIndex-1] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-2,square.row-1}) && ((Board[rowIndex+1][colIndex-2] == 'N' && color == 'w') || (Board[rowIndex+1][colIndex-2] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+1,square.row-2}) && ((Board[rowIndex+2][colIndex+1] == 'N' && color == 'w') || (Board[rowIndex+2][colIndex+1] == 'n' && color == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+2,square.row-1}) && ((Board[rowIndex+1][colIndex+2] == 'N' && color == 'w') || (Board[rowIndex+1][colIndex+2] == 'n' && color == 'b'))) return true;
    //This is all combinations of -2,-1,1,2 where abs(Δcol) != abs(Δrow)

    return false;
}

bool MoveChecksOwnKing(MatchDataInstance matchData, MoveInstance move, bool isEnPassant){
    char (*Board)[8] = matchData.Board.squares;
    Board[8-move.to.row][move.to.col-'a'] = Board[8-move.from.row][move.from.col-'a'];
    Board[8-move.from.row][move.from.col-'a'] = '.';

    if (matchData.playerTurn == 'w'){
        if (isEnPassant) Board[8-move.to.row+1][move.to.col-'a'] = '.';
        if (AttackingSquare(matchData, matchData.whiteKingSquare, 'b')) return true;
    } else{
        if (isEnPassant) Board[8-move.to.row-1][move.to.col-'a'] = '.';
        if (AttackingSquare(matchData, matchData.blackKingSquare, 'w')) return true;
    }

    return false;
}

bool MoveChecksOpponentKing(MatchDataInstance matchData, MoveInstance move, bool isEnPassant){
    char (*Board)[8] = matchData.Board.squares;
    Board[8-move.to.row][move.to.col-'a'] = Board[8-move.from.row][move.from.col-'a'];
    Board[8-move.from.row][move.from.col-'a'] = '.';

    if (matchData.playerTurn == 'w'){
        if (isEnPassant) Board[8-move.to.row+1][move.to.col-'a'] = '.';
        if (AttackingSquare(matchData, matchData.blackKingSquare, 'w')) return true;
    } else{
        if (isEnPassant) Board[8-move.to.row-1][move.to.col-'a'] = '.';
        if (AttackingSquare(matchData, matchData.whiteKingSquare, 'b')) return true;
    }

    return false;
}

LegalMovesInstance GetLegalMoves(MatchDataInstance matchData, SquareInstance square){
    char (*Board)[8] = matchData.Board.squares;
    const int iFrom = 8 - square.row;
    const int jFrom = square.col - 'a';
    char piece = Board[iFrom][jFrom];

    int maxArrSize = 4;
    int numLegalMoves = 0;
    MoveInstance* LegalMoves = malloc(maxArrSize * sizeof(MoveInstance)); //All the legal moves

    if (piece == '.'){
        free(LegalMoves);
        return (LegalMovesInstance){0, NULL};
    } else if (piece == 'P'){
        if (isValidSquare((SquareInstance){square.col, square.row+1})){
            if (Board[iFrom-1][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row+1}, 0}, false)){
                if (square.row == 7){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'r'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'q'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 0};
                }
                if (square.row == 2 && Board[iFrom-2][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row+2}, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+2}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col-1, square.row+1})){ //Diagonal left capture
            if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col-1, square.row+1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col-1, square.row+1}, 0}, false)){
                if (square.row == 7){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row+1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row+1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row+1}, 'r'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row+1}, 'q'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row+1}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col+1, square.row+1})){ //Diagonal right capture
            if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col+1, square.row+1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col+1, square.row+1}, 0}, false)){
                if (square.row == 7){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row+1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row+1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row+1}, 'r'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row+1}, 'q'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row+1}, 0};
                }
            }
        }
        if (abs(square.col - matchData.enPassant.col) == 1 && square.row == matchData.enPassant.row-1 && !MoveChecksOwnKing(matchData, (MoveInstance){square, matchData.enPassant, 0}, true)){
            LegalMoves[numLegalMoves++] = (MoveInstance){square, matchData.enPassant, 0};
        }
    } else if (piece == 'p'){
        if (isValidSquare((SquareInstance){square.col, square.row-1})){
            if (Board[iFrom+1][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row-1}, 0}, false)){
                if (square.row == 2){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'q'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'r'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 0};
                }
                if (square.row == 7 && Board[iFrom+2][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row-2}, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-2}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col-1, square.row-1})){
            if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col-1, square.row-1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col-1, square.row-1}, 0}, false)){
                if (square.row == 2){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row-1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row-1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row-1}, 'q'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row-1}, 'r'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col-1, square.row-1}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col+1, square.row-1})){
            if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col+1, square.row-1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col+1, square.row-1}, 0}, false)){
                if (square.row == 2){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row-1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row-1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row-1}, 'q'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row-1}, 'r'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col+1, square.row-1}, 0};
                }
            }
        }
        if (abs(square.col - matchData.enPassant.col) == 1 && square.row == matchData.enPassant.row+1 && !MoveChecksOwnKing(matchData, (MoveInstance){square, matchData.enPassant, 0}, true)){
            LegalMoves[numLegalMoves++] = (MoveInstance){square, matchData.enPassant, 0};
        }
    } else if (lower(piece) == 'r'){
        for (int i = square.row+1; isValidSquare((SquareInstance){square.col, i}); i++){ //Vertical up
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int i = square.row-1; isValidSquare((SquareInstance){square.col, i}); i--){ //Vertical down
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square,{square.col, i}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int j = square.col+1; isValidSquare((SquareInstance){j, square.row}); j++){ //Horizontal right
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int j = square.col-1; isValidSquare((SquareInstance){j, square.row}); j--){ //Horizontal left
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){j, square.row})) break;
        }

    } else if (lower(piece) == 'n'){
        for (int colOffset = -2; colOffset <= 2; colOffset++){
            if (colOffset == 0) continue;
            for (int rowOffset = -2; rowOffset <= 2; rowOffset++){
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                if (rowOffset == 0 || abs(colOffset) == abs(rowOffset)) continue;
                SquareInstance currentSquare = {square.col + colOffset, square.row + rowOffset};
                if (isValidSquare(currentSquare)){
                    if (!isSameColor(matchData.Board, square, currentSquare)){
                        if (!MoveChecksOwnKing(matchData, (MoveInstance){square, currentSquare, 0}, false)){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, currentSquare, 0};
                        }
                    }
                }
            }
        }
    } else if (lower(piece) == 'b'){
            for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row-k}); k++){ // (/) bottom
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col-k, square.row-k};
                if (Board[iFrom+k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData.Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row+k}); k++){ // (/) top
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col+k, square.row+k};
                if (Board[iFrom-k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData.Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){// (\) bottom
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col+k, square.row-k};
                if (Board[iFrom+k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData.Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ // (\) top
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col-k, square.row+k};
                if (Board[iFrom-k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData.Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }
    } else if (lower(piece) == 'k'){
        char enemyColor = (piece == 'K') ? 'b' : 'w';
        for (int rowOffset = -1; rowOffset <= 1; rowOffset++){
            for (int colOffset = -1; colOffset <= 1; colOffset++){
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                if (rowOffset == 0 && colOffset == 0) continue;
                SquareInstance currSquare = {square.col + colOffset, square.row + rowOffset};
                if (isValidSquare(currSquare)){
                    if (!AttackingSquare(matchData, currSquare, enemyColor)){
                        LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    }
                }
            }
        }

        if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
        if (piece == 'K'){ //White king castling
            if (square.col == 'e' && square.row == 1){
                if (matchData.castlingRights.WhiteQueenside){ //Castle queenside
                    if (Board[7][0] == 'R' && Board[7][1] == '.' && Board[7][2] == '.' && Board[7][3] == '.'){
                        if (!AttackingSquare(matchData, (SquareInstance){'c',1}, 'b')
                        && !AttackingSquare(matchData, (SquareInstance){'d',1}, 'b')
                        && !matchData.whiteKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'c', 1}, 0};
                        }
                    }
                }
                if (matchData.castlingRights.WhiteKingside){ //Castle kingside
                    if (Board[7][5] == '.' && Board[7][6] == '.' && Board[7][7] == 'R'){
                        if (!AttackingSquare(matchData, (SquareInstance){'f',1}, 'b')
                        && !AttackingSquare(matchData, (SquareInstance){'g',1}, 'b')
                        && !matchData.whiteKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'g', 1}, 0};
                        }
                    }
                }
            }
        } else{ //Black king castling
            if (square.col == 'e' && square.row == 8){
                if (matchData.castlingRights.BlackQueenside){ //Castle queenside
                    if (Board[0][0] == 'r' && Board[0][1] == '.' && Board[0][2] == '.' && Board[0][3] == '.'){
                        if (!AttackingSquare(matchData, (SquareInstance){'c',8}, 'w')
                        && !AttackingSquare(matchData, (SquareInstance){'d',8}, 'w')
                        && !matchData.blackKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'c', 8}, 0};
                        }
                    }
                }
                if (matchData.castlingRights.BlackKingside){ //Castle kingside
                    if (Board[0][5] == '.' && Board[0][6] == '.' && Board[0][7] == 'r'){
                        if (!AttackingSquare(matchData, (SquareInstance){'f',8}, 'w')
                        && !AttackingSquare(matchData, (SquareInstance){'g',8}, 'w')
                        && !matchData.blackKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'g', 8}, 0};
                        }
                    }
                }
            }
        }
    } else if (lower(piece) == 'q'){
        for (int i = square.row+1; isValidSquare((SquareInstance){square.col, i}); i++){ //Vertical up
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int i = square.row-1; isValidSquare((SquareInstance){square.col, i}); i--){ //Vertical down
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square,{square.col, i}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int j = square.col+1; isValidSquare((SquareInstance){j, square.row}); j++){ //Horizontal right
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int j = square.col-1; isValidSquare((SquareInstance){j, square.row}); j--){ //Horizontal left
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData.Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData.Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row-k}); k++){ // (/) bottom
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col-k, square.row-k};
            if (Board[iFrom+k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData.Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row+k}); k++){ // (/) top
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col+k, square.row+k};
            if (Board[iFrom-k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData.Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){// (\) bottom
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col+k, square.row-k};
            if (Board[iFrom+k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData.Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ // (\) top
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col-k, square.row+k};
            if (Board[iFrom-k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData.Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData.Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0}, false)){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }
    }

    if (numLegalMoves == 0){
        free(LegalMoves);
        return (LegalMovesInstance){0, NULL};
    } else{
        return (LegalMovesInstance){numLegalMoves, LegalMoves};
    }
}

bool PlayMove(MatchDataInstance* matchData, MoveInstance move){
    char (*Board)[8] = matchData->Board.squares;
    int iFrom = 8 - move.from.row;
    int jFrom = move.from.col - 'a';
    int iTo = 8 - move.to.row;
    int jTo = move.to.col - 'a';
    bool validMove = false;
    char piece = matchData->Board.squares[iFrom][jFrom];

    //printf("Checking move %c%d%c%d\n", move.from.col, move.from.row, move.to.col, move.to.row);
    //printf("Player turn: %c\n", matchData->playerTurn);

    LegalMovesInstance LegalMovesInst = GetLegalMoves(*matchData, move.from);
    MoveInstance* LegalMoves = LegalMovesInst.LegalMoves;

    for (int i = 0; i < LegalMovesInst.numMoves; i++){
        if (LegalMoves[i].to.col == move.to.col && LegalMoves[i].to.row == move.to.row && LegalMoves[i].promote == move.promote){
            validMove = true;
            break;
        }
    }

    if (validMove){
        //printf("Valid\n");
        if (piece == 'K'){ //Remove castling rights (due to king moving) and update king position
            //printf("White King move\n");
            matchData->whiteKingSquare = move.to;
            matchData->castlingRights.WhiteKingside = false;
            matchData->castlingRights.WhiteQueenside = false;
        } else if (piece == 'k'){
            //printf("Black King move\n");
            matchData->blackKingSquare = move.to;
            matchData->castlingRights.BlackKingside = false;
            matchData->castlingRights.BlackQueenside = false;
        }
        if (lower(piece) == 'p' || Board[iTo][jTo] != '.'){
            //printf("Reseting half moves\n");
            matchData->halfMoves = 0;
        } else{
            //printf("Half moves increased by 1\n");
            matchData->halfMoves++;
            if (matchData->halfMoves >= 50) matchData->gameFinished = true;
        }
        if (matchData->playerTurn == 'b'){
            //printf("Full moves increased by 1\n");
            matchData->fullMoves++;
        }

        matchData->eval += GetPieceValue(Board[iTo][jTo]); //0 if it is empty
        Board[iTo][jTo] = Board[iFrom][jFrom];
        Board[iFrom][jFrom] = '.';
        if (piece == 'K' && move.from.col == 'e' && move.from.row == '1' && move.to.col == 'c' && move.to.row == 1){ //White castle queenside
            Board[7][3] = 'R';
            Board[7][0] = '.';
        } else if (piece == 'K' && move.from.col == 'e' && move.from.row == '1' && move.to.col == 'g' && move.to.row == 1){ //White castle kingside
            Board[7][5] = 'R';
            Board[7][7] = '.'; 
        } else if (piece == 'k' && move.from.col == 'e' && move.from.row == '8' && move.to.col == 'c' && move.to.row == 8){ //Black castle queenside
            Board[0][3] = 'R';
            Board[0][0] = '.'; 
        } else if (piece == 'k' && move.from.col == 'e' && move.from.row == '8' && move.to.col == 'g' && move.to.row == 8){ //Black castle kingside
            Board[0][5] = 'R';
            Board[0][7] = '.'; 
        }
        
        if (piece == 'P' && move.to.col == matchData->enPassant.col && move.to.row == matchData->enPassant.row){ //Capture En Passant white
            Board[iTo+1][jTo] = '.';
            matchData->eval += 1;
        } else if (piece == 'p' && move.to.col == matchData->enPassant.col && move.to.row == matchData->enPassant.row){ //Capture En Passant black
            Board[iTo-1][jTo] = '.';
            matchData->eval -= 1;
        }

        if (piece == 'P' && move.to.row == move.from.row+2){ //Create En Passant white
            matchData->enPassant = (SquareInstance){move.from.col, move.from.row-1};
        } else if (piece == 'p' && move.to.row == move.from.row-2){ //Create En Passant black
            matchData->enPassant = (SquareInstance){move.from.col, move.from.row+1};
        } else{
            matchData->enPassant = (SquareInstance){0,0}; //Clear en passant after next turn
        }

        if (Board[0][0] != 'r'){ //Remove castling rights (due to rook moving)
            matchData->castlingRights.BlackQueenside = false;
        }
        if (Board[0][7] != 'r'){
            matchData->castlingRights.BlackKingside = false;
        }
        if (Board[0][0] != 'R'){
            matchData->castlingRights.WhiteQueenside = false;
        }
        if (Board[0][7] != 'R'){
            matchData->castlingRights.WhiteKingside = false;
        }

        if (AttackingSquare(*matchData, matchData->whiteKingSquare, 'b')){
            matchData->whiteKingChecked = true;
        } else{
            matchData->whiteKingChecked = false;
        }

        if (AttackingSquare(*matchData, matchData->blackKingSquare, 'w')){
            matchData->blackKingChecked = true;
        } else{
            matchData->blackKingChecked = false;
        }

        if (move.promote){
            matchData->eval -= GetPieceValue(Board[iTo][jTo]); //Subtract the pawn value from the final value
            Board[iTo][jTo] = (matchData->playerTurn == 'w') ? upper(move.promote) : move.promote;
            matchData->eval += GetPieceValue(Board[iTo][jTo]); //Add the value of the promoted piece
        }


        matchData->playerTurn = (matchData->playerTurn) == 'w' ? 'b' : 'w'; //Switch turns
        return true;
    } else{
        //printf("Invalid\n");
        return false;
    }
}

bool PlayMoves(MatchDataInstance* matchData, char* movesStr){
    MoveInstance moves[32];
    int numMoves = 0;

    MoveInstance move = StringToMove(strtok(movesStr, " \n"));
    while (move.from.col != 0 && numMoves < 32) {
        moves[numMoves++] = move;
        move = StringToMove(strtok(NULL, " \n"));
    }

    for (int i = 0; i < numMoves; i++){
        if (!PlayMove(matchData, moves[i])){
            return false; //Make sure to reset the board back to normal
        }
    }

    return true;
}