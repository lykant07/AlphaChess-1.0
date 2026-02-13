#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/select.h>
#include <unistd.h>
#include "commands.h"

#define MAX_DEPTH 6

bool inputAvailable(void) {
    struct timeval tv = {0, 0}; //Return results immediately
    fd_set fds; //The files we care about
    FD_ZERO(&fds); //Clear fds
    FD_SET(STDIN_FILENO, &fds); //Add stdin to fds
    return select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0; //Check readability of stdin
}

void CopyMove(MoveInstance* dest, MoveInstance* src){
    dest->from.col = src->from.col;
    dest->from.row = src->from.row;
    dest->to.col = src->to.col;
    dest->to.row = src->to.row;
    dest->promote = src->promote;
}

int GetEvaluation(MatchDataInstance* matchData, UndoStackInstance* undoStack, unsigned int depth, int alpha, int beta, bool* searchStopped){
    if (inputAvailable()){
        char buf[256];
        char* temp = fgets(buf, sizeof(buf), stdin);
        if (temp){
            if (strncmp(buf, "stop", 4) == 0){
                *searchStopped = true;
            }
        }
    }
    
    if (depth == 0 || (*searchStopped)) return matchData->eval;

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
                        int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta, searchStopped);
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
                        int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta, searchStopped);
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

    MoveInstance BestMove = {{'0',0},{'0',0},0};
    bool moveFound = false;
    bool searchStopped = false;
    
    for (int depth = 1; depth <= MAX_DEPTH && !searchStopped; depth++){ //Check all depths starting from 1 to ensure that we always have a move ready
        int maxEval = INT_MIN;
        int minEval = INT_MAX;
        int alpha = INT_MIN;
        int beta = INT_MAX;
        MoveInstance TempBestMove = {{'0',0},{'0',0},0};
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
                            int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta, &searchStopped);
                            if (moveEval > maxEval){
                                moveFound = true;
                                maxEval = moveEval;
                                CopyMove(&TempBestMove, &currentMove);
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
                            int moveEval = GetEvaluation(matchData, undoStack, depth-1, alpha, beta, &searchStopped);
                            if (moveEval < minEval){
                                moveFound = true;
                                minEval = moveEval;
                                CopyMove(&TempBestMove, &currentMove);
                            }
                            if (moveEval < beta) beta = moveEval;
                            UnmakeMove(matchData, undoStack);
                            if (beta <= alpha) break;
                        }
                    }
                }
            }
        }
        BestMove = TempBestMove;
    }

    if (!moveFound) return (MoveInstance){{'0',0},{'0',0},0}; //No available moves
    
    return BestMove;
}