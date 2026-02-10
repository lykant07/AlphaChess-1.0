#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "commands.h"

#define MAX_LINE_LENGTH 512
#define MAX_PARAMS 128

int main(void){
    char* engineName = "AlphaChess";
    char* engineVersion = "1.0";
    bool quitSession = false;
    MatchDataInstance matchData = {
        .Board = {{{0}}},
        .playerTurn = 'w',
        .castlingRights = {true, true, true, true},
        .enPassant = {0,0},
        .halfMoves = 0,
        .fullMoves = 1
    };
    do{
        bool validCommand = true;
        char line[MAX_LINE_LENGTH];
        if (!fgets(line, sizeof(line), stdin)) break;

        char* params[MAX_PARAMS];
        int numParams = 0;

        char* param = strtok(line, " \n");
        while (param && numParams < MAX_PARAMS) {
            params[numParams++] = param;
            param = strtok(NULL, " \n");
        }

        if (numParams == 0) continue;

        do{
            validCommand = true;
            if (strcmp(params[0], "position") == 0){
                if (numParams == 1){ //position
                    continue;
                } else if (numParams == 2){// position [...]
                    if (strcmp(params[1], "startpos") == 0){ //position startpos
                        ResetBoard(&matchData);
                    } else{
                        continue;
                    }  
                } else if (numParams < 7){//position startpos moves [MOVES] (possibly)
                    if (strcmp(params[1], "startpos") == 0 && strcmp(params[2], "moves") == 0){
                        if (numParams > 3){ //position startpos moves....
                            char moves[128] = {0};
                            int k = 0;
                            for (int i = 3; i < numParams; i++){
                                for (int j = 0; params[i][j] != '\0'; j++){
                                    moves[k++] = params[i][j];
                                }
                                moves[k++] = ' ';
                            }
                            if (!PlayMoves(&matchData, moves)) validCommand = false;
                        } else{ //position startpos moves
                            continue;
                        }
                    } else{
                        validCommand = false;
                    }
                } else{
                    if (strcmp(params[1], "startpos") == 0){ //position startpos....
                        if (strcmp(params[2], "moves") == 0){ //position startpos moves .....
                            char moves[128] = {0};
                            int k = 0;
                            for (int i = 3; i < numParams; i++){
                                for (int j = 0; params[i][j] != '\0'; j++){
                                    moves[k++] = params[i][j];
                                }
                                moves[k++] = ' ';
                            }
                            if (!PlayMoves(&matchData, moves)){
                                validCommand = false;
                            }
                        } else{ //position startpos [garbage]
                            validCommand = false;
                        }
                    } else{  //position [FEN] [move moves?]
                        char FEN[128];
                        snprintf(FEN, sizeof(FEN), "%s %s %s %s %s %s", params[1], params[2], params[3], params[4], params[5], params[6]);
                        if (LoadFEN(&matchData, FEN)){
                            if (numParams == 8){// position [FEN] [something]
                                validCommand = false;
                            } else if (numParams > 8){
                                if (strcmp(params[7], "moves") == 0){// position [FEN] moves [moves?]
                                    char moves[128] = {0};
                                    int k = 0;
                                    for (int i = 8; i < numParams; i++){
                                        for (int j = 0; params[i][j] != '\0'; j++){
                                            moves[k++] = params[i][j];
                                        }
                                        moves[k++] = ' ';
                                    }
                                    if (!PlayMoves(&matchData, moves)) validCommand = false;
                                } else{// position [FEN] [random]
                                    validCommand = false;
                                }
                            }
                        } else{
                            validCommand = false;
                        }
                    }
                }
            } else if (numParams == 1){
                if (strcmp(params[0], "uci") == 0){
                    printf("id name %s %s\n", engineName, engineVersion);
                    printf("id author lykant07\n");
                    printf("uciok\n");
                    fflush(stdout);
                } else if (strcmp(params[0], "isready") == 0){
                    printf("readyok\n");
                    fflush(stdout);
                } else if (strcmp(params[0], "ucinewgame") == 0){
                    ResetBoard(&matchData);
                } else if (strcmp(params[0], "showboard") == 0){
                    ShowBoard(matchData.Board);
                } else if (strcmp(params[0], "quit") == 0){
                    printf("quit\n");
                    fflush(stdout);
                    quitSession = true;
                }
            } else{
                validCommand = false;
            }

            if (!validCommand){
                if (numParams > 1){
                    for (int i = 1; i < numParams; i++){
                        params[i-1] = params[i];
                    }
                }
                numParams--;
            }
        } while (numParams > 0 && !validCommand); //Loop until a correct command is found (strip the 1st parameter)

        /*printf("%s\n", GetFEN(&matchData));
        for (int i = 0; i < 8; i++){
            for (int j = 0; j < 8; j++){
                printf("%c ", matchData.Board.squares[i][j]);
            }
            putchar('\n');
        }*/
    } while (!quitSession);

    return 0;
}