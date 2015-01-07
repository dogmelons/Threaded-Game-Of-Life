This program runs Conway's Game Of Life utilizing pthreads to split the task of processing. The game board is divided up between
 threads by rows, and when there are more threads than rows, individual rows are divided up between threads as well. This was done 
 mainly for simplicity code-wise, since it means contiguous memory per thread as the board is stored as a single array in row-major 
 order. Other approaches such as a tile based method may divide the board more evenly, but this method is fairly even as well so long 
 as you don't have more threads than rows (which if you do, then you probably have too many threads to begin with or you have a board 
 that consists of few long rows).