#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "simulator.h"
#include <stdbool.h>

// simple struct to keep track of page stats
typedef struct Pages {
    int page;
    int freq;
    int* timestamp;
} pages;

// simple swap used to sort predictions
void swap(pages* x, pages* y) {
    pages temp = *x;
    *x = *y;
    *y = temp;
}

// basically just bubble sort, used to sort predictions to descending order
void sortPredictions(pages* pred) {

    // find size
    int num = 0;
    while (pred[num].page != -1 && num < MAXPROCPAGES)
        num++;

    bool status;
    do {
        status = false;
        for (int i = 1; i < num; i++) {
            if (pred[i - 1].freq > pred[i].freq) {
                swap(pred + (i - 1), pred + i);
                status = true;
            }
        }

    } while (status);
}


void pageit(Pentry q[MAXPROCESSES]) {

    /* This file contains the stub for a predictive pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES];

    
    int proctmp, pagetmp;
    int last, lruPage, curPage;

    int process[MAXPROCESSES];
    int pc[MAXPROCESSES];
    pages hist[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];

    /* initialize static vars on first run */
    if (!initialized) {
    /* Init complex static vars here */
        for (int x = 0; x < MAXPROCESSES; x++) {
            for (int y = 0; y < MAXPROCPAGES; y++) {
                for (int z = 0; z < MAXPROCPAGES; z++) {
                    hist[x][y][z].page = -1;
                    hist[x][y][z].freq = -1;
                    hist[x][y][z].timestamp = NULL;
                }
            }
        }
        for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
            for (pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++)
                timestamps[proctmp][pagetmp] = 0;
            process[proctmp] = 0;
        }
        initialized = 1;
    }


    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {

        // edge cases
        if (!q[proctmp].active) continue;
        if (last == -1) continue;

        // set variables for current process
        last = pc[proctmp] / PAGESIZE;
        pc[proctmp] = q[proctmp].pc;
        curPage = q[proctmp].pc / PAGESIZE;

        if (last == curPage)
            continue;

        pageout(proctmp, last);
        
        // temp copy array to increment freq of pages
        pages* updateFreq = hist[proctmp][last];

        for (int i = 0; i < MAXPROCPAGES; i++) {
            
            // count if page already appears
            if (updateFreq[i].page == curPage) {
                updateFreq[i].freq++;
                break;
            }

            // add page if empty
            else if (updateFreq[i].page < 0) {
                updateFreq[i].page = curPage;
                updateFreq[i].freq = 1;
                updateFreq[i].timestamp = &(timestamps[proctmp][i]);
                break;
            }
        }
    }


    // update all process to current tick
    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
        
        if (!q[proctmp].active) continue;

        pagetmp = (q[proctmp].pc) / PAGESIZE;
        timestamps[proctmp][pagetmp] = tick;
    }

    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {

        // pageout everything if innactive
        if (!q[proctmp].active) {
            for (pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++)
                pageout(proctmp, pagetmp);
            continue;
        }

        pagetmp = q[proctmp].pc / PAGESIZE;

        // continue if already in memory
        if (q[proctmp].pages[pagetmp] == 1)
            continue;

        if (pagein(proctmp, pagetmp)) {
            process[proctmp] = 0;
            continue;
        }

        if (process[proctmp] == 1)
            continue;


        int n = INT_MAX;
        // have we found lru page
        bool status = false;
        for (int i = 0; i < MAXPROCESSES; i++) {
            for (int j = 0; j < MAXPROCPAGES; j++) {
                // find lru via timestamp
                if (q[i].pages[j] == 1 && timestamps[i][j] < n) {
                    n = timestamps[i][j];
                    lruPage = j;
                    status = true;
                }
            }
        }

        if (status == true) continue;

        // edge case if not found lru page, try paging out last one
        if (!pageout(proctmp, lruPage)) {
            fprintf(stderr, "Page out failed, exiting\n");
            exit(EXIT_FAILURE);
        }

        // if all else fails keep waiting
        process[proctmp] = 1;
    }

    // where predictions is created, sorted, and tells us what to pagein for each process
    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {

        pages* predictions;
        
        if (!q[proctmp].active) continue;

        int counter = q[proctmp].pc;
        // jump forward, 100 ish seemed to be best
        predictions = hist[proctmp][(counter + 101) / PAGESIZE];

        // sort by most likely
        sortPredictions(predictions);

        // just getting size of predictions
        int size = 0;
        while (size < MAXPROCPAGES && predictions[size].page != -1)
            size++;

        // pagein the most likely to be used
        for (int i = 0; i < size; i++)
            pagein(proctmp, predictions[i].page);
    }

    /* advance time for next pageit iteration */
    tick++;
}