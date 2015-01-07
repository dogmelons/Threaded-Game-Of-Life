/*
 * Author: Kyle Easterling
 * For: Distributed and Parallel Computing
 * Program: Threaded Game Of Life in C
 *
 *Description: This program runs Conway's Game Of Life utilizing pthreads
 * to split the task of processing. The game board is divided up between
 * threads by rows, and when there are more threads than rows, individual
 * rows are divided up between threads as well. This was done mainly for
 * simplicity code-wise, since it means contiguous memory per thread as the
 * board is stored as a single array in row-major order. Other approaches
 * such as a tile based method may divide the board more evenly, but this
 * method is fairly even as well so long as you don't have more threads than
 * rows (which if you do, then you probably have too many threads to begin 
 * with or you have a board that consists of few long rows).
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#include "Board_Splitter.h"
#include "cyclic_barrier.h"

#define NUMBER_OF_THREADS 10

int rows = 0;
int columns = 0;
int generations = 0;
int size = 0;
int* board;
int* copy;

// Returns a pointer to an int array corresponding to the board in file_name.
// The array is in row-major order.
void readBoard(char* file_name)
{
    FILE* input;
    input = fopen(file_name, "r");
    if (!input)
    {
        printf("File failed to open.");
        exit(EXIT_FAILURE);
    }

    fscanf(input, "%d %d %d", &generations, &rows, &columns);
    size = rows * columns;
    board = malloc(sizeof(int) * size);

    int character = 0;
    int cursor = 0;

    do 
    {
        character = fgetc(input);
        if (EOF != character) 
        {
            // Ignore whitsespace in reading in the string
            if (!isspace(character)) 
            {
                if ('*' == character)
                {
                    board[cursor] = 1;
                    cursor = cursor + 1;
                }
                else if ('.' == character)
                {
                    board[cursor] = 0;
                    cursor = cursor + 1;
                }                
            }
        }
    } while (EOF != character);

    fclose(input);
    // Make sure we actually initialized our board properly
    if (cursor != size)
    {
        printf("Invalid file input.");
        free(board);
        exit(EXIT_FAILURE);
    }
}

void printBoard(FILE* output)
{
    for (int pos = 0; pos < size; ++pos)
    {
        if (((pos % columns) == 0) ^ (pos == 0))
        {
	    fprintf(output, "\n");
        }
        fprintf(output, "%c", (board[pos] == 0 ? '.' : '*'));
    }
}

// Sets copy to the next generation of board.
void processGeneration(thread_param_t* actuals)
{
    int startIndex = (actuals->firstRow * columns) + actuals->firstCol;

    int row = 0;
    int col = 0;
    int neighbors = 0;

    for (int index = startIndex; index < (startIndex + actuals->length); ++index)
    {
        neighbors = 0;
        row = index / columns;
        col = index % (columns);

        // check previous row for neighbors
        if (row != 0)
        {
            if (board[index - columns] == 1)
            {
                ++neighbors;
            }

            if (col != 0)
            {
                if (board[index - columns - 1] == 1)
                    ++neighbors;
            }
            if (col != (columns - 1))
            {
                if (board[index - columns + 1] == 1)
                    ++neighbors;
            }
        }

        // check current row for neighbors
        if (col != 0)
        {
            if (board[index - 1] == 1)
            {
                ++neighbors;
            }
        }
        if (col != (columns - 1))
        {
            if (board[index + 1] == 1)
            {
                ++neighbors;
            }
        }

        // check next row for neighbors;
        if (row != (rows - 1))
        {
            if (board[index + columns] == 1)
            {
                ++neighbors;
            }

            if (col != 0)
            {
                if (board[index + columns - 1] == 1)
                    ++neighbors;
            }
            if (col != (columns - 1))
            {
                if (board[index + columns + 1] == 1)
                    ++neighbors;
            }
        }
        // Update board copy based on neighbors. This way we avoid changing
        // board and simulate a simultaneous update.
        if ((neighbors >= 4) || (neighbors <= 1))
        {
            copy[index] = 0;
        }
        if ((board[index] == 0) && (neighbors == 3))
        {
            copy[index] = 1;
        }
    }
}

void* thread_run(void* arguments)
{
    thread_param_t* actuals;
    actuals = (thread_param_t*)arguments;
    int value = 0;

    for (int gen = 0; gen < generations; ++gen)
    {
        processGeneration(actuals);

        value = cyclic_barrier_await(actuals->barrier);
    }

    return NULL;
}

void synchronizeBoard(void)
{
    memcpy(board, copy, size * sizeof(int));
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Run this program with the board file name argument.");
        exit(EXIT_FAILURE);
    }

    FILE* output;
    time_t start;
    time_t end;

    //initialize board
    readBoard(argv[1]);
    copy = malloc(sizeof(int) * size);

    memcpy(copy, board, size * sizeof(int));

    //set up threading stuff
    pthread_t* threads;
    thread_param_t* parameters;
    cyclic_barrier_t barrier;
    int index = 0;

    threads = malloc(NUMBER_OF_THREADS * sizeof(pthread_t));
    parameters = malloc(NUMBER_OF_THREADS * sizeof(thread_param_t));

    splitBoard(parameters, NUMBER_OF_THREADS, rows, columns);

    cyclic_barrier_init(&barrier, NULL, NULL, NUMBER_OF_THREADS, synchronizeBoard);

    for (index = 0; index < NUMBER_OF_THREADS; index++) 
    {
        parameters[index].barrier = &barrier;
        pthread_create(&threads[index], NULL, thread_run, &parameters[index]);
    }

    start = clock();

    for (index = 0; index < NUMBER_OF_THREADS; index++) 
    {
        pthread_join(threads[index], NULL);
    }

    end = clock();

    printf("Runtime was %.6f\n", ((double)(end-start)) / 1.0E6);
    
    //print final board to specified output file or to stdout
    if(1 < argc)
    {
        output = fopen(argv[2], "w");
    }
    else
    {
       output = stdout;
    }
    
    printBoard(output);
    
    if(output != stdout)
    {
	fclose(output);
    }

    cyclic_barrier_destroy(&barrier);

    free(board);
    free(copy);
    free(parameters);
    free(threads);

    return 0;
}
