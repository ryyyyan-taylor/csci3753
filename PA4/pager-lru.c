/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains an lru pageit
 *      implmentation.
 */

#include <stdio.h>
#include <stdlib.h>

#include "simulator.h"

void pageit(Pentry q[MAXPROCESSES]) {

    /* ===== START GIVEN ===== */
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES]; // Helps keep track of least recently used

    /* Local vars */
    int proctmp;
    int pagetmp;

    /* initialize static vars on first run */
    if (!initialized) {
        for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
            for (pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++)
                timestamps[proctmp][pagetmp] = 0;
        }
        initialized = 1;
    }

    /* TODO: Implement LRU Paging */
    /* ===== END GIVEN ===== */


    int pCounter; // pc of current process
    int oldPage; // Our old page
    int minTime; // Keeping track of l-r used page with artificial "time" ticks
    int lruPage; // Keeping track of the lru page

    // i : index through q 
    // j : new page

    for (int i = 0; i < MAXPROCESSES; i++) {
        /* Is process active? If not then it doesn't enter any of the following */
        if (q[i].active) {
            /* The following is for first active process */
            pCounter = q[i].pc; // program counter value for this process
            oldPage = pCounter / PAGESIZE; // page the program counter needs(might actually be the old one soon)
            timestamps[i][oldPage] = tick;
            /* Check if the page is swapped out? */
            if (!q[i].pages[oldPage]) {
                /* Try to perform the swap in with pagein() if we can */
                if (!pagein(i, oldPage)) { // Will enter block if pageavail==0
                    minTime = tick; // Set minimum to keep track of time while iterating
                    /* If swapping fails, this is where we decide swap out another page(our "new" page) */
                    for (int j = 0; j < MAXPROCPAGES; j++) {

                        if (q[i].pages[j] == 1) {
                            if (timestamps[i][j] < minTime) {
                                minTime = timestamps[i][j];
                                lruPage = j;
                            }
                        } else if (!(q[i].pages[j]))
                            continue;
                    }

                    if (!pageout(i, lruPage))
                        printf(" FAILED TO PAGEOUT\n");
                }
            }
        }
    }

    /* advance time for next pageit iteration */
    tick++;
}