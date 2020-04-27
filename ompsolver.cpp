#include "game.h"
#include <list> 
#include <string>
#include <tuple>
#include "CycleTimer.h"

//using namespace Game;
using namespace std;

//TODO? do we want to change this to store in 2 int* ? or keep as is
tuple<int,int> Game::chooseRandomMove() {
    int xinit = rand() % height;
    int yinit = rand() % width;
    while (playboard[xinit][yinit] == 1) {
        xinit = rand() % height;
        yinit = rand() % width;
    }
    return make_tuple(xinit,yinit);
}

//count uncovered adj mines (meaning already marked)
int Game::countAdjMines(int x, int y) {
    int c = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int xi = x+i;
            int yi = y+j;
            if (xi >= 0 && xi < height && yi >= 0 && yi < width && !(i == 0 && j == 0)) {
                if (playboard[xi][yi] == 1 && board[xi][yi] == -1) {
                    c++;
                } 
            }
        }
    } 
    return c;
}

//reveal neighbors not revealed yet
void Game::revealNeighbors(int x, int y) {
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int xi = x+i;
            int yi = y+j;
            if (xi >= 0 && xi < height && yi >= 0 && yi < width && !(i == 0 && j == 0)) {
                if (playboard[xi][yi] == 0) {
                    playboard2[xi][yi] = 1;
                    if (board[xi][yi] == -1) {
                        printf("DID A BAD\n");
                    }
                } 
            }
        }
    } 
}

int Game::countUnrevealed(int x, int y) {
    int c = 0;
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int xi = x+i;
            int yi = y+j;
            if (xi >= 0 && xi < height && yi >= 0 && yi < width && !(i == 0 && j == 0)) {
                if (playboard[xi][yi] == 0) {
                    c++;
                } 
            }
        }
    } 
    return c;
}
void Game::markNeighbors(int x, int y) {
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            int xi = x+i;
            int yi = y+j;
            if (xi >= 0 && xi < height && yi >= 0 && yi < width && !(i == 0 && j == 0)) {
                int gotta_do_stuff = 0;
                int tempminecount;
                #pragma omp critical 
                {
                    if (playboard2[xi][yi] == 0) {
                        playboard2[xi][yi] = 1;
                        gotta_do_stuff = 1;
                        tempminecount = playminecount;
                        playminecount++;
                    }
                }
                if(gotta_do_stuff){

                    // printf("tmc: %d\n",tempminecount);
                    playmines[tempminecount * 2] = xi;
                    playmines[tempminecount * 2 + 1] = yi;
                    
                }
            }
        }
    } 
    
}

double Game::seqSolve() {

    // int totalBytes = sizeof(int) * height * width;
    double startTime = CycleTimer::currentSeconds();

    double firsttimer = 0;
    double secondtimer = 0;


    int guesses = 0;  
    while(playminecount < numMines) {
        tuple<int,int> rmove = chooseRandomMove();
        int x = get<0>(rmove);
        int y = get<1>(rmove);
        guesses++;
        if (board[x][y] == -1) {
            printf("\n");
            printf("oops %dth guess was a bomb big sad\n",guesses);
            return -1;
        } else {
            //reveal
            playboard[x][y] = 1;
        }
        bool progress = true;
        while (progress) {
            progress = false;
            double starttim = CycleTimer::currentSeconds();
            #pragma omp parallel for collapse(2)
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    if (playboard[i][j] == 1 && board[i][j] != -1 ) { //clear square
                        int adjmines = countAdjMines(i,j);
                        int unrevealed = countUnrevealed(i,j);
                        if (unrevealed != 0 ){
                            if (adjmines == board[i][j]) { //all mines found
                                //reveal neighbors
                                progress = true;
                                revealNeighbors(i,j);
                            }
                            if (unrevealed == board[i][j] - adjmines && unrevealed >= 0) {
                                progress = true;
                                markNeighbors(i,j);
                            }
                        }
                        
                    }
                }
            }
            double midtim = CycleTimer::currentSeconds();
            #pragma omp parallel for collapse(2)
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    playboard[i][j] |= playboard2[i][j];
                }
            }
            double endtim = CycleTimer::currentSeconds();
            firsttimer += midtim - starttim;
            secondtimer += endtim - midtim;
        }
    }

    double endTime = CycleTimer::currentSeconds();


    double overallDuration = endTime - startTime;
    printf("firsttim: %0.3f ms secondtim: %0.3f ms\n",1000.f * firsttimer, 1000.f * secondtimer);
    // printf("Overall: %.3f ms\t\t[%.3f GB/s]\n", 1000.f * overallDuration, toBW(totalBytes, overallDuration));
    return overallDuration;
}