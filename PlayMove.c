#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "commands.h"

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
            return +1;
            break;
        case 'R':
            return +5;
            break;
        case 'N':
            return +3;
            break;
        case 'B':
            return +3;
            break;
        case 'Q':
            return +9;
            break;
        case 'p':
            return +1;
            break;
        case 'r':
            return -5;
            break;
        case 'n':
            return -3;
            break;
        case 'b':
            return -3;
            break;
        case 'q':
            return -9;
            break;
        default:
            return 0;
            break;
    }
}

bool AttackingSquare(MatchDataInstance* matchData, SquareInstance square, char attackingColor){
    int rowIndex =  8 - square.row;
    int colIndex = square.col - 'a';
    char (*Board)[8] = matchData->Board.squares;


    for (int j = colIndex+1; j < 8; j++){ //Check row (right side)
        if ((attackingColor == 'w' && isBlack(Board[rowIndex][j])) || (attackingColor == 'b' && isWhite(Board[rowIndex][j]))){
            break; // A same color piece is blocking the way
        } else if (attackingColor == 'w'){ //Check for white attackers
             if (Board[rowIndex][j] == 'R' || Board[rowIndex][j] == 'Q' || (Board[rowIndex][j] == 'K' && j == colIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[rowIndex][j] == 'r' || Board[rowIndex][j] == 'q' || (Board[rowIndex][j] == 'k' && j == colIndex+1)) return true;
        }
    }

    for (int j = colIndex-1; j >= 0; j--){ //Check row (left side)
        if ((attackingColor == 'w' && isBlack(Board[rowIndex][j])) || (attackingColor == 'b' && isWhite(Board[rowIndex][j]))){
            break; // A same color piece is blocking the way
        } else if (attackingColor == 'w'){ //Check for white attackers
             if (Board[rowIndex][j] == 'R' || Board[rowIndex][j] == 'Q' || (Board[rowIndex][j] == 'K' && j == colIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[rowIndex][j] == 'r' || Board[rowIndex][j] == 'q' || (Board[rowIndex][j] == 'k' && j == colIndex+1)) return true;
        }
    }

    for (int i = rowIndex-1; i >= 0; i--){ //Check column (top)
        if ((attackingColor == 'w' && isBlack(Board[i][colIndex])) || (attackingColor == 'b' && isWhite(Board[i][colIndex]))){
            break; // A same color piece is blocking the way
        } else if (attackingColor == 'w'){ //Check for white attackers
             if (Board[i][colIndex] == 'R' || Board[i][colIndex] == 'Q' || (Board[i][colIndex] == 'K' && i == rowIndex-1)) return true;
        } else{ //Check for black attackers
            if (Board[i][colIndex] == 'r' || Board[i][colIndex] == 'q' || (Board[i][colIndex] == 'k' && i == rowIndex-1)) return true;
        }
    }

    for (int i = rowIndex+1; i < 8; i++){ //Check column (bottom)
        if ((attackingColor == 'w' && isBlack(Board[i][colIndex])) || (attackingColor == 'b' && isWhite(Board[i][colIndex]))){
            break; // A same color piece is blocking the way
        } else if (attackingColor == 'w'){ //Check for white attackers
             if (Board[i][colIndex] == 'R' || Board[i][colIndex] == 'Q' || (Board[i][colIndex] == 'K' && i == rowIndex+1)) return true;
        } else{ //Check for black attackers
            if (Board[i][colIndex] == 'r' || Board[i][colIndex] == 'q' || (Board[i][colIndex] == 'k' && i == rowIndex+1)) return true;
        }
    }

    for (int k = 1; isValidSquare((SquareInstance){square.col-k, square.row-k}); k++){ //Below (/)
        if (attackingColor == 'w'){
            if (Board[rowIndex+k][colIndex-k] == 'B' || Board[rowIndex+k][colIndex-k] == 'Q' || ((Board[rowIndex+k][colIndex-k] == 'K' || Board[rowIndex+k][colIndex-k] == 'P') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex-k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex+k][colIndex-k] == 'b' || Board[rowIndex+k][colIndex-k] == 'q' || (Board[rowIndex+k][colIndex-k] == 'k' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex-k])) break; //Same color piece in the way
        }
    }
    for (int k = 1; isValidSquare((SquareInstance){square.col+k, square.row+k}); k++){ //Above (/)
        if (attackingColor == 'w'){
            if (Board[rowIndex-k][colIndex+k] == 'B' || Board[rowIndex-k][colIndex+k] == 'Q' || (Board[rowIndex-k][colIndex+k] == 'K' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex+k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex-k][colIndex+k] == 'b' || Board[rowIndex-k][colIndex+k] == 'q' || ((Board[rowIndex-k][colIndex+k] == 'k' || Board[rowIndex-k][colIndex+k] == 'p') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex+k])) break; //Same color piece in the way
        }
    }

    for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ //Above (\)
        if (attackingColor == 'w'){
            if (Board[rowIndex-k][colIndex-k] == 'B' || Board[rowIndex-k][colIndex-k] == 'Q' || (Board[rowIndex-k][colIndex-k] == 'K' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex-k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex-k][colIndex-k] == 'b' || Board[rowIndex-k][colIndex-k] == 'q' || ((Board[rowIndex-k][colIndex-k] == 'k' || Board[rowIndex-k][colIndex-k] == 'p') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex-k][colIndex-k])) break; //Same color piece in the way
        }
    }
    for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){ //Below (\)
        if (attackingColor == 'w'){
            if (Board[rowIndex+k][colIndex+k] == 'B' || Board[rowIndex+k][colIndex+k] == 'Q' || ((Board[rowIndex+k][colIndex+k] == 'K' || Board[rowIndex+k][colIndex+k] == 'P') && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex+k])) break; //Same color piece in the way
        } else{
            if (Board[rowIndex+k][colIndex+k] == 'b' || Board[rowIndex+k][colIndex+k] == 'q' || (Board[rowIndex+k][colIndex+k] == 'k' && k == 1)) return true; //Attacking enemy piece
            if (isBlack(Board[rowIndex+k][colIndex+k])) break; //Same color piece in the way
        }
    }

    //Check for knight threats :)
    if (isValidSquare((SquareInstance){square.col-1,square.row+2}) && ((Board[rowIndex-2][colIndex-1] == 'N' && attackingColor == 'w') || (Board[rowIndex-2][colIndex-1] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-2,square.row+1}) && ((Board[rowIndex-1][colIndex-2] == 'N' && attackingColor == 'w') || (Board[rowIndex-1][colIndex-2] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+1,square.row+2}) && ((Board[rowIndex-2][colIndex+1] == 'N' && attackingColor == 'w') || (Board[rowIndex-2][colIndex+1] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+2,square.row+1}) && ((Board[rowIndex-1][colIndex+2] == 'N' && attackingColor == 'w') || (Board[rowIndex-1][colIndex+2] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-1,square.row-2}) && ((Board[rowIndex+2][colIndex-1] == 'N' && attackingColor == 'w') || (Board[rowIndex+2][colIndex-1] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col-2,square.row-1}) && ((Board[rowIndex+1][colIndex-2] == 'N' && attackingColor == 'w') || (Board[rowIndex+1][colIndex-2] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+1,square.row-2}) && ((Board[rowIndex+2][colIndex+1] == 'N' && attackingColor == 'w') || (Board[rowIndex+2][colIndex+1] == 'n' && attackingColor == 'b'))) return true;
    if (isValidSquare((SquareInstance){square.col+2,square.row-1}) && ((Board[rowIndex+1][colIndex+2] == 'N' && attackingColor == 'w') || (Board[rowIndex+1][colIndex+2] == 'n' && attackingColor == 'b'))) return true;
    //This is all combinations of -2,-1,1,2 where abs(Δcol) != abs(Δrow)

    return false;
}

bool MoveChecksOwnKing(MatchDataInstance* matchData, MoveInstance move){
    MoveUndo stack[2];
    UndoStackInstance undoStack = {stack, -1, 2};
    MakeMove(matchData, move, &undoStack);
    if (matchData->playerTurn == 'w'){
        if (AttackingSquare(matchData, matchData->whiteKingSquare, 'b')){
            UnmakeMove(matchData, &undoStack);
            return true;
        }
    } else{
        if (AttackingSquare(matchData, matchData->blackKingSquare, 'w')){
            UnmakeMove(matchData, &undoStack);
            return true;
        }
    }

    UnmakeMove(matchData, &undoStack);
    return false;
}

bool MoveChecksOpponentKing(MatchDataInstance* matchData, MoveInstance move){
    MoveUndo stack[2];
    UndoStackInstance undoStack = {stack, -1, 2};
    MakeMove(matchData, move, &undoStack);

    if (matchData->playerTurn == 'w'){
        if (AttackingSquare(matchData, matchData->blackKingSquare, 'w')){
            UnmakeMove(matchData, &undoStack);
            
            return true;
        }
    } else{
        if (AttackingSquare(matchData, matchData->whiteKingSquare, 'b')){
            UnmakeMove(matchData, &undoStack);
            return true;
        }
    }

    UnmakeMove(matchData, &undoStack);
    return false;
}

void MakeMove(MatchDataInstance* md, MoveInstance move, UndoStackInstance* undoStack){
    char (*Board)[8] = md->Board.squares;

    int iFrom = 8 - move.from.row;
    int jFrom = move.from.col - 'a';
    int iTo   = 8 - move.to.row;
    int jTo   = move.to.col - 'a';
    char piece = Board[iFrom][jFrom];

    undoStack->top++;
    int top = undoStack->top;
    if (top+1 >= undoStack->maxSize){
        undoStack->maxSize *= 2;
        MoveUndo* temp = realloc(undoStack->stack, undoStack->maxSize * sizeof(MoveUndo));
        if (!temp) exit(1);
        undoStack->stack = temp;
    }

    // Save undo info
    undoStack->stack[top].from = move.from;
    undoStack->stack[top].to = move.to;
    undoStack->stack[top].movedPiece = piece;
    undoStack->stack[top].capturedPiece = Board[iTo][jTo];
    undoStack->stack[top].oldEnPassant = md->enPassant;
    undoStack->stack[top].capturedSq = move.to;
    undoStack->stack[top].oldCastling = md->castlingRights;
    undoStack->stack[top].oldHalfMoves = md->halfMoves;
    undoStack->stack[top].oldFullMoves = md->fullMoves;
    undoStack->stack[top].oldEval = md->eval;
    undoStack->stack[top].oldWhiteKing = md->whiteKingSquare;
    undoStack->stack[top].oldBlackKing = md->blackKingSquare;
    undoStack->stack[top].oldGameFinished = md->gameFinished;


    if (piece == 'K'){ //Remove castling rights (due to king moving) and update king position
        //printf("White King move\n");
        md->whiteKingSquare = move.to;
        md->castlingRights.WhiteKingside = false;
        md->castlingRights.WhiteQueenside = false;
    } else if (piece == 'k'){
        //printf("Black King move\n");
        md->blackKingSquare = move.to;
        md->castlingRights.BlackKingside = false;
        md->castlingRights.BlackQueenside = false;
    }
    if (lower(piece) == 'p' || Board[iTo][jTo] != '.'){
        //printf("Reseting half moves\n");
        md->halfMoves = 0;
    } else{
        //printf("Half moves increased by 1\n");
        md->halfMoves++;
        if (md->halfMoves >= 50) md->gameFinished = true;
    }
    if (md->playerTurn == 'b'){
        //printf("Full moves increased by 1\n");
        md->fullMoves++;
    }

    md->eval -= GetPieceValue(Board[iTo][jTo]); //0 if it is empty
    Board[iTo][jTo] = Board[iFrom][jFrom];
    Board[iFrom][jFrom] = '.';
    if (piece == 'K' && move.from.col == 'e' && move.from.row == '1' && move.to.col == 'c' && move.to.row == 1){ //White castle queenside
        Board[7][3] = 'R';
        Board[7][0] = '.';
    } else if (piece == 'K' && move.from.col == 'e' && move.from.row == '1' && move.to.col == 'g' && move.to.row == 1){ //White castle kingside
        Board[7][5] = 'R';
        Board[7][7] = '.'; 
    } else if (piece == 'k' && move.from.col == 'e' && move.from.row == '8' && move.to.col == 'c' && move.to.row == 8){ //Black castle queenside
        Board[0][3] = 'r';
        Board[0][0] = '.'; 
    } else if (piece == 'k' && move.from.col == 'e' && move.from.row == '8' && move.to.col == 'g' && move.to.row == 8){ //Black castle kingside
        Board[0][5] = 'r';
        Board[0][7] = '.'; 
    }
    
    if (piece == 'P' && move.to.col == md->enPassant.col && move.to.row == md->enPassant.row){ //Capture En Passant white
        undoStack->stack[top].capturedPiece = 'p';
        undoStack->stack[top].capturedSq = (SquareInstance){move.to.col, move.to.row-1};
        Board[iTo+1][jTo] = '.';
        md->eval += 1;
    } else if (piece == 'p' && move.to.col == md->enPassant.col && move.to.row == md->enPassant.row){ //Capture En Passant black
        undoStack->stack[top].capturedPiece = 'P';
        undoStack->stack[top].capturedSq = (SquareInstance){move.to.col, move.to.row+1};
        Board[iTo-1][jTo] = '.';
        md->eval -= 1;
    }

    if (piece == 'P' && move.to.row == move.from.row+2){ //Create En Passant white
        md->enPassant = (SquareInstance){move.from.col, move.from.row+1};
    } else if (piece == 'p' && move.to.row == move.from.row-2){ //Create En Passant black
        md->enPassant = (SquareInstance){move.from.col, move.from.row-1};
    } else{
        md->enPassant = (SquareInstance){0,0}; //Clear en passant after next turn
    }

    if (Board[0][0] != 'r'){ //Remove castling rights (due to rook moving)
        md->castlingRights.BlackQueenside = false;
    }
    if (Board[0][7] != 'r'){
        md->castlingRights.BlackKingside = false;
    }
    if (Board[7][0] != 'R'){
        md->castlingRights.WhiteQueenside = false;
    }
    if (Board[7][7] != 'R'){
        md->castlingRights.WhiteKingside = false;
    }

    if (AttackingSquare(md, md->whiteKingSquare, 'b')){
        md->whiteKingChecked = true;
    } else{
        md->whiteKingChecked = false;
    }

    if (AttackingSquare(md, md->blackKingSquare, 'w')){
        md->blackKingChecked = true;
    } else{
        md->blackKingChecked = false;
    }

    if (move.promote){
        md->eval -= GetPieceValue(Board[iTo][jTo]); //Subtract the pawn value from the final value
        Board[iTo][jTo] = (md->playerTurn == 'w') ? upper(move.promote) : move.promote;
        md->eval += GetPieceValue(Board[iTo][jTo]); //Add the value of the promoted piece
    }


    md->playerTurn = (md->playerTurn) == 'w' ? 'b' : 'w'; //Switch turns

}

void UnmakeMove(MatchDataInstance* md, UndoStackInstance* undoStack) {
    char (*Board)[8] = md->Board.squares;
    int top = undoStack->top;
    MoveUndo topUndo = undoStack->stack[top];

    int iFrom = 8 - topUndo.from.row;
    int jFrom = topUndo.from.col - 'a';
    int iTo   = 8 - topUndo.to.row;
    int jTo   = topUndo.to.col - 'a';

    // Restore turn
    md->playerTurn = (md->playerTurn == 'w') ? 'b' : 'w';

    // Restore pieces
    Board[iFrom][jFrom] = topUndo.movedPiece;
    Board[iTo][jTo] = topUndo.capturedPiece;

    // Restore en passant captured pawn
    if (lower(topUndo.movedPiece) == 'p' &&
        topUndo.capturedSq.row != topUndo.to.row) {

        int sqI = 8 - topUndo.capturedSq.row;
        int sqJ = topUndo.capturedSq.col - 'a';
        Board[sqI][sqJ] = topUndo.capturedPiece;
        Board[iTo][jTo] = '.';
    }

    // Restore rook if castling
    if (topUndo.movedPiece == 'K' && topUndo.from.col == 'e') {
        if (topUndo.to.col == 'g') { Board[7][7] = 'R'; Board[7][5] = '.'; }
        else if (topUndo.to.col == 'c') { Board[7][0] = 'R'; Board[7][3] = '.'; }
    }

    if (topUndo.movedPiece == 'k' && topUndo.from.col == 'e') {
        if (topUndo.to.col == 'g') { Board[0][7] = 'r'; Board[0][5] = '.'; }
        else if (topUndo.to.col == 'c') { Board[0][0] = 'r'; Board[0][3] = '.'; }
    }

    // Restore state
    md->enPassant = topUndo.oldEnPassant;
    md->castlingRights = topUndo.oldCastling;
    md->halfMoves = topUndo.oldHalfMoves;
    md->fullMoves = topUndo.oldFullMoves;
    md->eval = topUndo.oldEval;
    md->whiteKingSquare = topUndo.oldWhiteKing;
    md->blackKingSquare = topUndo.oldBlackKing;
    md->gameFinished = topUndo.oldGameFinished;

    undoStack->top--; //pop top element
}

LegalMovesInstance GetLegalMoves(MatchDataInstance* matchData, SquareInstance square){
    char (*Board)[8] = matchData->Board.squares;
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
            if (Board[iFrom-1][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row+1}, 0})){
                if (square.row == 7){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'r'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 'q'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+1}, 0};
                }
                if (square.row == 2 && Board[iFrom-2][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row+2}, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row+2}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col-1, square.row+1})){ //Diagonal left capture
            if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col-1, square.row+1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col-1, square.row+1}, 0})){
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
            if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col+1, square.row+1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col+1, square.row+1}, 0})){
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
        if (abs(square.col - matchData->enPassant.col) == 1 && square.row == matchData->enPassant.row-1 && !MoveChecksOwnKing(matchData, (MoveInstance){square, matchData->enPassant, 0})){
            LegalMoves[numLegalMoves++] = (MoveInstance){square, matchData->enPassant, 0};
        }
    } else if (piece == 'p'){
        if (isValidSquare((SquareInstance){square.col, square.row-1})){
            if (Board[iFrom+1][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row-1}, 0})){
                if (square.row == 2){
                    if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'n'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'b'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'q'};
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 'r'};
                } else{
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-1}, 0};
                }
                if (square.row == 7 && Board[iFrom+2][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, square.row-2}, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, square.row-2}, 0};
                }
            }
        }
        if (isValidSquare((SquareInstance){square.col-1, square.row-1})){
            if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col-1, square.row-1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col-1, square.row-1}, 0})){
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
            if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col+1, square.row-1}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col+1, square.row-1}, 0})){
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
        if (abs(square.col - matchData->enPassant.col) == 1 && square.row == matchData->enPassant.row+1 && !MoveChecksOwnKing(matchData, (MoveInstance){square, matchData->enPassant, 0})){
            LegalMoves[numLegalMoves++] = (MoveInstance){square, matchData->enPassant, 0};
        }
    } else if (lower(piece) == 'r'){
        for (int i = square.row+1; isValidSquare((SquareInstance){square.col, i}); i++){ //Vertical up
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int i = square.row-1; isValidSquare((SquareInstance){square.col, i}); i--){ //Vertical down
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square,{square.col, i}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int j = square.col+1; isValidSquare((SquareInstance){j, square.row}); j++){ //Horizontal right
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int j = square.col-1; isValidSquare((SquareInstance){j, square.row}); j--){ //Horizontal left
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){j, square.row})) break;
        }

    } else if (lower(piece) == 'n'){
        for (int colOffset = -2; colOffset <= 2; colOffset++){
            if (colOffset == 0) continue;
            for (int rowOffset = -2; rowOffset <= 2; rowOffset++){
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                if (rowOffset == 0 || abs(colOffset) == abs(rowOffset)) continue;
                SquareInstance currentSquare = {square.col + colOffset, square.row + rowOffset};
                if (isValidSquare(currentSquare)){
                    if (!isSameColor(matchData->Board, square, currentSquare)){
                        if (!MoveChecksOwnKing(matchData, (MoveInstance){square, currentSquare, 0})){
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
                if (Board[iFrom+k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData->Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row+k}); k++){ // (/) top
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col+k, square.row+k};
                if (Board[iFrom-k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData->Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){// (\) bottom
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col+k, square.row-k};
                if (Board[iFrom+k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData->Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    break;
                }
            }

            for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ // (\) top
                if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
                SquareInstance currSquare = {square.col-k, square.row+k};
                if (Board[iFrom-k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                    LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                } else if (isSameColor(matchData->Board, square, currSquare)){
                    break;
                } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
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
                    if (!AttackingSquare(matchData, currSquare, enemyColor) && !isSameColor(matchData->Board, square, currSquare)){
                        LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                    }
                }
            }
        }

        if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
        if (piece == 'K'){ //White king castling
            if (square.col == 'e' && square.row == 1){
                if (matchData->castlingRights.WhiteQueenside){ //Castle queenside
                    if (Board[7][0] == 'R' && Board[7][1] == '.' && Board[7][2] == '.' && Board[7][3] == '.'){
                        if (!AttackingSquare(matchData, (SquareInstance){'c',1}, 'b')
                        && !AttackingSquare(matchData, (SquareInstance){'d',1}, 'b')
                        && !matchData->whiteKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'c', 1}, 0};
                        }
                    }
                }
                if (matchData->castlingRights.WhiteKingside){ //Castle kingside
                    if (Board[7][5] == '.' && Board[7][6] == '.' && Board[7][7] == 'R'){
                        if (!AttackingSquare(matchData, (SquareInstance){'f',1}, 'b')
                        && !AttackingSquare(matchData, (SquareInstance){'g',1}, 'b')
                        && !matchData->whiteKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'g', 1}, 0};
                        }
                    }
                }
            }
        } else{ //Black king castling
            if (square.col == 'e' && square.row == 8){
                if (matchData->castlingRights.BlackQueenside){ //Castle queenside
                    if (Board[0][0] == 'r' && Board[0][1] == '.' && Board[0][2] == '.' && Board[0][3] == '.'){
                        if (!AttackingSquare(matchData, (SquareInstance){'c',8}, 'w')
                        && !AttackingSquare(matchData, (SquareInstance){'d',8}, 'w')
                        && !matchData->blackKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'c', 8}, 0};
                        }
                    }
                }
                if (matchData->castlingRights.BlackKingside){ //Castle kingside
                    if (Board[0][5] == '.' && Board[0][6] == '.' && Board[0][7] == 'r'){
                        if (!AttackingSquare(matchData, (SquareInstance){'f',8}, 'w')
                        && !AttackingSquare(matchData, (SquareInstance){'g',8}, 'w')
                        && !matchData->blackKingChecked){
                            LegalMoves[numLegalMoves++] = (MoveInstance){square, {'g', 8}, 0};
                        }
                    }
                }
            }
        }
    } else if (lower(piece) == 'q'){
        for (int i = square.row+1; isValidSquare((SquareInstance){square.col, i}); i++){ //Vertical up
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int i = square.row-1; isValidSquare((SquareInstance){square.col, i}); i--){ //Vertical down
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-i][jFrom] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square,{square.col, i}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){square.col, i}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){square.col, i}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {square.col, i}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){square.col, i})) break;
        }

        for (int j = square.col+1; isValidSquare((SquareInstance){j, square.row}); j++){ //Horizontal right
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int j = square.col-1; isValidSquare((SquareInstance){j, square.row}); j--){ //Horizontal left
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            if (Board[8-square.row][j - 'a'] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
            } else if (areEnemyPieces(matchData->Board, square, (SquareInstance){j, square.row}) && !MoveChecksOwnKing(matchData, (MoveInstance){square, (SquareInstance){j, square.row}, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, {j, square.row}, 0};
                break;
            } else if (isSameColor(matchData->Board, square, (SquareInstance){j, square.row})) break;
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row-k}); k++){ // (/) bottom
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col-k, square.row-k};
            if (Board[iFrom+k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData->Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row+k}); k++){ // (/) top
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col+k, square.row+k};
            if (Board[iFrom-k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData->Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col+k,square.row-k}); k++){// (\) bottom
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col+k, square.row-k};
            if (Board[iFrom+k][jFrom+k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData->Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
                break;
            }
        }

        for (int k = 1; isValidSquare((SquareInstance){square.col-k,square.row+k}); k++){ // (\) top
            if (numLegalMoves == maxArrSize){maxArrSize *= 2; LegalMoves = realloc(LegalMoves, maxArrSize * sizeof(MoveInstance));}
            SquareInstance currSquare = {square.col-k, square.row+k};
            if (Board[iFrom-k][jFrom-k] == '.' && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
                LegalMoves[numLegalMoves++] = (MoveInstance){square, currSquare, 0};
            } else if (isSameColor(matchData->Board, square, currSquare)){
                break;
            } else if (areEnemyPieces(matchData->Board, square, currSquare) && !MoveChecksOwnKing(matchData, (MoveInstance){square, currSquare, 0})){
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

bool PlayMove(MatchDataInstance* matchData, MoveInstance move, UndoStackInstance* undoStack){
    bool validMove = false;

    //printf("Checking move %c%d%c%d\n", move.from.col, move.from.row, move.to.col, move.to.row);
    //printf("Player turn: %c\n", matchData->playerTurn);

    LegalMovesInstance LegalMovesInst = GetLegalMoves(matchData, move.from);
    MoveInstance* LegalMoves = LegalMovesInst.LegalMoves;

    for (int i = 0; i < LegalMovesInst.numMoves; i++){
        if (LegalMoves[i].to.col == move.to.col && LegalMoves[i].to.row == move.to.row && LegalMoves[i].promote == move.promote){
            validMove = true;
            free(LegalMoves);
            break;
        }
    }

    if (validMove){
        //printf("Valid\n");
        MakeMove(matchData, move, undoStack);
        return true;
    } else{
        //printf("Invalid\n");
        free(LegalMoves);
        return false;
    }
}

bool PlayMoves(MatchDataInstance* matchData, char* movesStr, UndoStackInstance* undoStack){
    MoveInstance moves[32];
    int numMoves = 0;

    MoveInstance move = StringToMove(strtok(movesStr, " \n"));
    while (move.from.col != 0 && numMoves < 32) {
        moves[numMoves++] = move;
        move = StringToMove(strtok(NULL, " \n"));
    }

    for (int i = 0; i < numMoves; i++){
        if (!PlayMove(matchData, moves[i], undoStack)){
            return false; //Make sure to reset the board back to normal
        }
    }

    return true;
}