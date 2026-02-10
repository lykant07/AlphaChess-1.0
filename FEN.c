#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "commands.h"

static char lower(char ch){
    if (ch >= 'A' && ch <= 'Z') ch += 32;
    return ch;
}

char* GetFEN(MatchDataInstance* matchData){
    char* FEN = malloc(64 * sizeof(char));
    if (!FEN){ //Malloc failed
        return NULL;
    }
    int fenIndex = 0;
    int consecutiveSpaces = 0;
    for (int i = 0; i < 8; i++){
        for (int j = 0; j < 8; j++){
            if (lower(matchData -> Board.squares[i][j]) == 'r'
            || lower(matchData -> Board.squares[i][j]) == 'n'
            || lower(matchData -> Board.squares[i][j]) == 'b'
            || lower(matchData -> Board.squares[i][j]) == 'k'
            || lower(matchData -> Board.squares[i][j]) == 'q'
            || lower(matchData -> Board.squares[i][j]) == 'p'){
                if (consecutiveSpaces > 0){
                    FEN[fenIndex++] = '0' + consecutiveSpaces;
                    consecutiveSpaces = 0;
                }
                FEN[fenIndex++] = matchData -> Board.squares[i][j];
            } else if (matchData -> Board.squares[i][j] == '.'){
                consecutiveSpaces++;
            } else{
                free(FEN);
                return NULL;
            }
        }
        if (consecutiveSpaces > 0){
            FEN[fenIndex++] = '0' + consecutiveSpaces;
            consecutiveSpaces = 0;
        }
        if (i != 7) FEN[fenIndex++] = '/';
    }

    return FEN;
}

bool LoadFEN(MatchDataInstance* matchData, char* FENstr){

    char* FEN[6];
    int numParams = 0;

    char* param = strtok(FENstr, " \n");
    while (param){
        if (numParams >= 6) return false; //More than 6 params
        FEN[numParams++] = param;
        param = strtok(NULL, " \n");
    }

    if (numParams != 6) return false; //Less than 6 params

    int row = 0;
    int col = 0;
    for (int j = 0; FEN[0][j] != '\0'; j++){
        if (col > 7 && FEN[0][j] != '/') return false; //Too many characters in row
        if (lower(FEN[0][j]) == 'r'
        || lower(FEN[0][j]) == 'n'
        || lower(FEN[0][j]) == 'b'
        || lower(FEN[0][j]) == 'k'
        || lower(FEN[0][j]) == 'q'
        || lower(FEN[0][j]) == 'p'){
            matchData -> Board.squares[row][col++] = FEN[0][j];
        } else if (FEN[0][j] == '/'){
            if (col != 8) return false; //Not a full row
            row++;
            if (row > 7) return false; //Too many rows
            col = 0;
        } else{
            if (FEN[0][j] >= '1' && FEN[0][j] <= '8'){
                int num = FEN[0][j] - '0';
                if (col + num > 8){ //Adding the number lead to column above 8
                    return false;
                }
                for (int k = 0; k < num; k++){
                    matchData -> Board.squares[row][col++] = '.';
                }
                
            } else{ //Illegal character
                return false;
            }
        }
    }

    return true;
}