#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "commands.h"

#define MAX_DEPTH 6

void CopyMove(MoveInstance* dest, MoveInstance* src){
    dest->from.col = src->from.col;
    dest->from.row = src->from.row;
    dest->to.col = src->to.col;
    dest->to.row = src->to.row;
    dest->promote = src->promote;
}

int GetEvaluation(MatchDataInstance* matchData, UndoStackInstance* undoStack, unsigned int depth, int alpha, int beta){
    if (depth == 0) return matchData->eval;

    int maxEval = INT_MIN;
    int minEval = INT_MAX;
    bool foundMove = false;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            SquareInstance square = {'a' + j, 8 - i};
            char piece = matchData->Board.squares[i][j];
            if (matchData->playerTurn == 'w'){
                if (piece >= 'A' && piece <= 'Z'){ //For all white pieces on board
                    LegalMovesInstance LegalMovesInst = GetLegalMoves(matchData, square);
                    for (int k = 0; k < LegalMovesInst.numMoves; k++){ //Try out all legal moves
                        foundMove = true;
                        MoveInstance currentMove = LegalMovesInst.LegalMoves[k];
                        MakeMove(matchData, currentMove, undoStack);
                        int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta);
                        if (moveEval > maxEval) maxEval = moveEval;
                        if (moveEval > alpha) alpha = moveEval;
                        UnmakeMove(matchData, undoStack);
                        if (beta <= alpha) break;
                    }
                    free(LegalMovesInst.LegalMoves);
                }
            } else{
                if (piece >= 'a' && piece <= 'z'){ //For all black pieces on board
                    LegalMovesInstance LegalMovesInst = GetLegalMoves(matchData, square);
                    for (int k = 0; k < LegalMovesInst.numMoves; k++){ //Try out all legal moves
                        foundMove = true;
                        MoveInstance currentMove = LegalMovesInst.LegalMoves[k];
                        MakeMove(matchData, currentMove, undoStack);
                        int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta);
                        if (moveEval < minEval) minEval = moveEval;
                        if (moveEval < beta) beta = moveEval;
                        UnmakeMove(matchData, undoStack);
                        if (beta <= alpha) break;
                    }
                    free(LegalMovesInst.LegalMoves);
                }
            }
        }
    }

    if (!foundMove){ //No available moves
        if (matchData->playerTurn == 'w'){
            return (matchData->whiteKingChecked) ? INT_MIN + depth : 0; // Black wins | Stalemate
        } else{
            return (matchData->blackKingChecked) ? INT_MAX - depth : 0; // White wins | Stalemate
        }
    } else{
        return (matchData->playerTurn == 'w') ? maxEval : minEval;
    }
}

MoveInstance GetBestMove(MatchDataInstance* matchData, UndoStackInstance* undoStack){

    MoveInstance BestMove;
    int maxEval = INT_MIN;
    int minEval = INT_MAX;
    int alpha = INT_MIN;
    int beta = INT_MAX;
    bool moveFound = false;

    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            SquareInstance square = {'a' + j, 8 - i};
            char piece = matchData->Board.squares[i][j];
            if (matchData->playerTurn == 'w'){
                if (piece >= 'A' && piece <= 'Z'){ //For all white pieces on board
                    LegalMovesInstance LegalMovesInst = GetLegalMoves(matchData, square);
                    //printf("-------\n");
                    for (int k = 0; k < LegalMovesInst.numMoves; k++){ //Try out all legal moves
                        MoveInstance currentMove = LegalMovesInst.LegalMoves[k];
                        /*printf("%c%d%c%d", currentMove.from.col, currentMove.from.row, currentMove.to.col, currentMove.to.row);
                        if (currentMove.promote) putchar(currentMove.promote);
                        putchar('\n');*/
                        MakeMove(matchData, currentMove, undoStack);
                        int moveEval = GetEvaluation(matchData, undoStack, MAX_DEPTH-1, alpha, beta);
                        if (moveEval > maxEval){
                            moveFound = true;
                            maxEval = moveEval;
                            CopyMove(&BestMove, &currentMove);
                        }
                        if (moveEval > alpha) alpha = moveEval;
                        UnmakeMove(matchData, undoStack);
                        if (beta <= alpha) break;
                    }
                }
            } else{
                if (piece >= 'a' && piece <= 'z'){ //For all black pieces on board
                    LegalMovesInstance LegalMovesInst = GetLegalMoves(matchData, square);
                    for (int k = 0; k < LegalMovesInst.numMoves; k++){ //Try out all legal moves
                        MoveInstance currentMove = LegalMovesInst.LegalMoves[k];
                        MakeMove(matchData, currentMove, undoStack);
                        int moveEval = GetEvaluation(matchData, undoStack, MAX_DEPTH-1, alpha, beta);
                        if (moveEval < minEval){
                            moveFound = true;
                            minEval = moveEval;
                            CopyMove(&BestMove, &currentMove);
                        }
                        if (moveEval < beta) beta = moveEval;
                        UnmakeMove(matchData, undoStack);
                        if (beta <= alpha) break;
                    }
                }
            }
        }
    }

    if (!moveFound) return (MoveInstance){{'0',0},{'0',0},0}; //No available moves
    
    return BestMove;
}