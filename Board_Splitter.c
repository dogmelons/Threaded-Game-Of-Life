#include "Board_Splitter.h"

int splitBoard(thread_param_t* threadList, int threadCount, int rows, int columns)
{
    //if there are equal/less threads than rows
    if ((rows / threadCount) != 0)
    {
        int extraRows = rows % threadCount;
        int maxPer = rows / threadCount;

        int currentRow = 0;
        for (int i = 0; i < threadCount; ++i)
        {
            if (extraRows > 0)
            {
                threadList[i].firstRow = currentRow;
                threadList[i].firstCol = 0;
                threadList[i].length = (maxPer + 1) * columns;
                currentRow += maxPer + 1;
                --extraRows;
            }
            else
            {
                threadList[i].firstRow = currentRow;
                threadList[i].firstCol = 0;
                threadList[i].length = maxPer * columns;
                currentRow += maxPer;
            }
        }
    }
    //if there are more threads than cells (this should never happen, but just in case...)
    else if (threadCount > (rows * columns))
    {
        int overflow = 0;
        overflow = threadCount - (rows * columns);
        int currentRow = 0;
        int currentCol = 0;

        for (int i = 0; i < threadCount; ++i)
        {
            if ((threadCount - i) > overflow)
            {
                threadList[i].firstRow = currentRow;
                threadList[i].firstCol = currentCol;
                threadList[i].length = 1;

                if ((currentCol + 1) >= columns)
                {
                    currentCol = 0;
                    ++currentRow;
                }
                else
                {
                    ++currentCol;
                }
            }
            else
            {
                //this may need tweaking, but for now having a length of 0 should suffice for extra threads
                threadList[i].firstRow = 0;
                threadList[i].firstCol = 0;
                threadList[i].length = 0;
            }
        }
    }
    //otherwise we have to consider splitting rows
    //for now each thread beyond rows just gets one cell, this will be upgraded to actually split rows eventually
    else
    {
        int currentRow = 0;
        int currentCol = 0;

        for (int i = 0; i < threadCount; ++i)
        {
            //assign one cell to each extra thread
            if ((threadCount - i) > (rows - currentRow))
            {
                threadList[i].firstRow = currentRow;
                threadList[i].firstCol = currentCol;
                threadList[i].length = 1;

                if ((currentCol + 1) >= columns)
                {
                    currentCol = 0;
                    ++currentRow;
                }
                else
                {
                    ++currentCol;
                }
            }
            else
            {
                //if we're transitioning from assigning cells to assigning rows
                if (currentCol != 0)
                {
                    int columnsLeft = columns - currentCol;
                    threadList[i].firstRow = currentRow;
                    threadList[i].firstCol = currentCol;
                    threadList[i].length = columnsLeft;

                    currentCol = 0;
                    ++currentRow;
                }
                else
                {
                    threadList[i].firstRow = currentRow;
                    threadList[i].firstCol = currentCol;
                    threadList[i].length = columns;

                    ++currentRow;
                }
            }
        }
    }

    return 0;
}