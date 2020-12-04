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

    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int initialized = 0;
    static int tick = 1; // artificial time
    static int timestamps[MAXPROCESSES][MAXPROCPAGES]; // Helps keep track of least recently used

    // local counters
    int proctmp;
    int pagetmp;

    if (!initialized) {
        for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {
            for (pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++)
                timestamps[proctmp][pagetmp] = 0;
        }
        initialized = 1;
    }


    int pCounter, oldPage, minTime, lruPage; 

    // proctmp : index through q 
    // pagetmp : new page

    for (proctmp = 0; proctmp < MAXPROCESSES; proctmp++) {

        if (q[proctmp].active) {
            
            // set variables for current process
            pCounter = q[proctmp].pc;
            oldPage = pCounter / PAGESIZE;
            timestamps[proctmp][oldPage] = tick;
            
            if (!q[proctmp].pages[oldPage]) {

                if (!pagein(proctmp, oldPage)) {
                    // reset minimum
                    minTime = tick;
                    
                    for (pagetmp = 0; pagetmp < MAXPROCPAGES; pagetmp++) {

                        if (q[proctmp].pages[pagetmp] == 1) {
                            if (timestamps[proctmp][pagetmp] < minTime) {
                                minTime = timestamps[proctmp][pagetmp];
                                lruPage = pagetmp;
                            }
                        }
                        else if (!(q[proctmp].pages[pagetmp])) continue;
                    }

                    if (!pageout(proctmp, lruPage))
                        printf(" FAILED TO PAGEOUT\n");
                }
            }
        }
    }

    /* advance time for next pageit iteration */
    tick++;
}