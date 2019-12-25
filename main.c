#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <omp.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>

#define PLOTTING false

// Struct representing a Pixel with RGB values
typedef struct
{
    unsigned char red, green, blue;
} PPMPixel;

// Struct representing an image in PPM format
typedef struct
{
    int width, height;
    PPMPixel *data;
} PPMImage;

// Struct to store coordiantes
typedef struct
{
    int x, y;

} Point;


static int32_t s_randtbl[32] =
  {
    3,

    -1726662223, 379960547, 1735697613, 1040273694, 1313901226,
    1627687941, -179304937, -2073333483, 1780058412, -1989503057,
    -615974602, 344556628, 939512070, -1249116260, 1507946756,
    -812545463, 154635395, 1388815473, -1926676823, 525320961,
    -1009028674, 968117788, -123449607, 1284210865, 435012392,
    -2017506339, -911064859, -370259173, 1132637927, 1398500161,
    -205601318,
  };

typedef struct Seed{
  int32_t randtbl[32];
  int32_t *fptr;
  int32_t *rptr;
  int32_t *end_ptr;
  int32_t *state;
} Seed; 

// The random function you could used in your project instead of the C 'rand' function. 
unsigned int my_rand(Seed *seed) 
{
  int32_t *fptr = seed->fptr;
  int32_t *rptr = seed->rptr;
  int32_t *end_ptr = seed->end_ptr;
  unsigned int val;

  val = *fptr += (unsigned int) *rptr;
  ++fptr;
  if (fptr >= end_ptr) {
    fptr = seed->state;
    ++rptr;
  }
  else {
    ++rptr;
    if (rptr >= end_ptr)
      rptr = seed->state;
  }
  seed->fptr = fptr;
  seed->rptr = rptr;

  return val;
}

void init_seed(Seed *seed)
{
  for(int i = 0; i < 32; ++i) {
    seed->randtbl[i] = s_randtbl[i];
  }
  seed->fptr = &(seed->randtbl[2]);
  seed->rptr = &(seed->randtbl[1]);
  seed->end_ptr = &(seed->randtbl[sizeof (seed->randtbl) / sizeof (seed->randtbl[0])]);
  seed->state = &(seed->randtbl[1]);

  unsigned int init = (time(NULL) << omp_get_thread_num());
  seed->state[0] = init;
  int32_t *dst = seed->state;
  int32_t word = init;
  int kc = 32;
  for(int i = 1; i < kc; ++i) {
    long int hi = word / 127773;
    long int lo = word % 127773;
    word = 16807 * lo - 2836 * hi;
    if (word < 0)
      word += 2147483647;
    *++dst = word;
  }
  seed->fptr = &(seed->state[3]);
  seed->rptr = &(seed->state[0]);
  kc *= 10;
  while (--kc >= 0) {
    my_rand(seed);
  }
}


//Struct representing a node in a linked list
//Is used as a stack
typedef struct stackNode {
    Point data;
    struct stackNode *next;
} stackNode;

/*
 * Function: push
 * ----------------------------
 *  Appends a stackNode containing `newPoint` at the beginning of 
 *  a linked list accessed by the `head` pointer 
 * 
 *  *head: pointer to a stackNode representing the beginning of the list
 *  newPoint: Point to be added as data to the new stackNode
 *
 *  returns: new head pointer of type `stackNode` pointing at the start of the list
 */
stackNode *push(stackNode *head, Point newPoint) {
    stackNode *newNode = malloc(sizeof(stackNode));
    if (newNode == NULL) {
        perror("Error! memory not allocated.");
        exit(EXIT_FAILURE);
    }
    newNode->data = newPoint;
    newNode->next = head;
    head = newNode;
    return head;
}

/*
 * Function: pop
 * ----------------------------
 *  Removes a stackNode containing at the beginning of 
 *  a linked list accessed by the `head` pointer.
 *  Stores the data of the stackNode in the pointer *point
 * 
 *  *head: pointer to a stackNode representing the beginning of the list
 *  *point: Point storing the data of the removed stackNode
 *
 *  returns: new head pointer pointing at the start of the list
 */
stackNode *pop(stackNode *head, Point *point) {
    stackNode *oldCell = head;
    *point = head->data;
    head = head->next;
    oldCell->next = NULL;
    free(oldCell);
    return head;
}

/*
 * Function: writePPM
 * ----------------------------
 *  Creates a file of the PPM image format
 *
 *  filename: name of the file the image will be saved at
 *  img: image to write to file
 *
 *  returns: /
 */
void writePPM(const char *filename, PPMImage *img) {
    FILE *fp;
    fp = fopen(filename, "wb");
    if (!fp) {
        fprintf(stderr, "Unable to open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    //write the header file
    //image format
    fprintf(fp, "P6 ");

    //image size
    fprintf(fp, "%d %d ", img->width, img->height);

    // rgb component depth
    fprintf(fp, "%d\n", 255);

    // pixel data
    fwrite(img->data, 3 * img->height, img->width, fp);
    fclose(fp);
}

/*
 * Function: floodFillConnected
 * ----------------------------
 *  If the cell in a grid is made of conducting material, assign the cell 
 *  at (x, y) the value 2 . Uses a stack to emulate recursion and connects
 *  each neighboring conducting cell together by assigning it the value 2.
 *
 *  grid: pointer to a square array of integer values of size N*N
 *  x: integer between 0 and N-1, specifying a row in the grid
 *  y: integer between 0 and N-1, specifying a column in the grid
 *  N: size of one side of the grid
 * 
 *  returns: /
 */
void floodFillConnected(int *grid, int x, int y, int N) {
    stackNode *head = calloc(sizeof(stackNode), sizeof(stackNode));
    if (head == NULL) {
        perror("Error! memory not allocated.");
        exit(EXIT_FAILURE);
    }
    head->next = NULL;

    // Push first point on the Stack
    head = push(head, (Point){x, y});

    Point *newPoint = malloc(sizeof(Point));
    if (newPoint == NULL) {
        perror("Error! memory not allocated.");
        free(head->next);
        free(head);
        exit(EXIT_FAILURE);
    }

    //While the stack is not empty
    while (head != NULL) {
        //Pop a point from the stack and push its neighbours on
        head = pop(head, newPoint);
        int x_ = newPoint->x;
        int y_ = newPoint->y;

        if (grid[N * x_ + y_] == 1) {
            //The cell is conducting and connected
            grid[N * x_ + y_] = 2;

            if (y_ + 1 < N) {
                head = push(head, (Point){x_, y_ + 1});
            }
            if (y_ - 1 >= 0) {
                head = push(head, (Point){x_, y_ - 1});
            }
            if (x_ + 1 < N) {
                head = push(head, (Point){x_ + 1, y_});
            }
            if (x_ - 1 >= 0) {
                head = push(head, (Point){x_ - 1, y_});
            }
        }
    }

    free(head);
    free(newPoint);
}

/*
 * Function: isGridConducting
 * ----------------------------
 *  Check if the whole grid is conducting by verifying if there
 *  is a connected path of cells between the right and left side
 *  of the grid.
 * 
 *  grid: pointer to a square array of integer values of size N*N
 *  N: size of one side of the grid
 * 
 *  returns: 1 if the grid is conducting, 0 if not
 */
int isGridConducting(int *grid, int N) {
    int x;

    // Start a call on all the leftmost cells
    // to create a path of conducting cells from left to right
    for (x = 0; x < N; x++) {
        floodFillConnected(grid, x, 0, N);
    }

    // Check if any of the rightmost cells are conducting
    // If one is found, the grid is conducting
    for (x = 0; x < N; x++) {
        if (grid[N * x + (N - 1)] == 2) {
            return 1;
        }
    }

    return 0;
}

/*
 * Function: createImage
 * ----------------------------
 *  Creates a PPMImage from the grid and saves it to disk in ppm format
 *  The pixels are either black, grey or orange depending on their conductiv
 *  properties
 * 
 *  grid: pointer to a square array of integer values of size N*N
 *  N: size of one side of the grid
 *  image: pointer to an instance of PPMImage object
 *  filename: name of the file the image will be saved at
 *
 * 
 *  returns: true if the grid is conducting, false if not
 */
void createImage(int *grid, int N, PPMImage *image, const char *filename) {
    PPMPixel grey = {128, 128, 128};
    PPMPixel black = {0, 0, 0};
    PPMPixel orange = {255, 165, 0};

    image->width = N;
    image->height = N;

    for (long i = 0; i < N * N; i++) {
        //Non-conducting
        if (grid[i] == 0) {
            image->data[i] = grey;
        }
        //Conducting but not connected
        else if (grid[i] == 1) {
            image->data[i] = black;
        }
        //Conducting and connected
        else if (grid[i] == 2) {
            image->data[i] = orange;
        }
    }
    writePPM(filename, image);
}

/* 
 * Fill the empty grid randomly with an amount of`nbConductingFibers`conducting
 * fibers that are 3 cells long
 * 
 *  grid: pointer to a square array of integer values of size N*N
 *  nbConductingFibers: integer specifying the number of conducting fibers
 *                      in the grid
 *  N: size of one side of the grid
 * 
 *  returns: /
 */
void createConductingFibers(int *grid, unsigned int nbConductingFibers, unsigned int N) {

    Seed seed;
    init_seed(&seed);

    long* randomIndex = malloc(N*N*sizeof(long));
    if(randomIndex == NULL) {
        perror("Error! memory not allocated.");
        free(grid);
        exit(EXIT_FAILURE);
    }

    //Array containing cell number that will be conductive
    for(long i=0; i < N* N; i ++){
        randomIndex[i] = i;
    }
    //Shuffle array 
    for (long i = 0; i < N*N - 1; i++) {
	  long index = i + my_rand(&seed) % (N*N - i);
      long temp = randomIndex[index];
	  randomIndex[index] = randomIndex[i];
	  randomIndex[i] = temp;
	}
    
    for (long i = 0; i < nbConductingFibers; i++) {
        int dir = my_rand(&seed) % 2;
       
        grid[randomIndex[i]] = 1;

        // Set vertical neighbours to 1
        if (dir == 1) {
            //Check boundary conditions
            if (randomIndex[i] - N >= 0) {
                grid[randomIndex[i] - N] = 1;
            }
            if (randomIndex[i] + N < N * N) {
                grid[randomIndex[i] + N] = 1;
            }
        }
        // Set horizontal neighbours to 1
        else {
            //Check boundary conditions
            if (randomIndex[i] % N != 0) {
                grid[randomIndex[i] - 1] = 1;
            }
            if ((randomIndex[i] + 1) % N != 0) {
                grid[randomIndex[i] + 1] = 1;
            }
        }
    }
}

/*
 * Function: monteCarlo
 * ----------------------------
 *  Computes the number of conducting grids out of `M` grids.
 *
 *  grid: pointer to a square array of integer values of size N*N
 *  nbConductingFibers: integer specifying the number of conducting fibers
 *                      in the grid
 *  N: size of one side of the grid
 *  M: number of grids to check
 * 
 *  returns: number of conducting grids
 */
int monteCarlo(unsigned int nbConductingFibers, int N, int M) {
    unsigned int nbConducting = 0;
    unsigned int numberOfThreads;
    int thread_id;
    int *grid = NULL;
    double *wtime = 0;

    #pragma omp parallel private(grid, thread_id) 
    {   
        #pragma omp single
        {
            numberOfThreads = omp_get_num_threads();
            wtime = malloc(numberOfThreads * sizeof(double));
            if(wtime == NULL){
                perror("Error! memory not allocated.");
                exit(EXIT_FAILURE);
            }
        }
        grid = malloc(N * N * sizeof(int));
        if (grid == NULL) {
            perror("Error! memory not allocated.");
            free(wtime);
            exit(EXIT_FAILURE);
        }

        thread_id = omp_get_thread_num();
        wtime[thread_id] = omp_get_wtime();

        #pragma omp for schedule(runtime) reduction(+:nbConducting)
        for(int j = 0; j < M; j++){
            memset(grid, 0, N * N * sizeof(int));
            createConductingFibers(grid, nbConductingFibers, N);
            nbConducting += isGridConducting(grid, N);
        }
        wtime[thread_id] = (omp_get_wtime() - wtime[thread_id]) * 1000;

    }

    double totalTime = 0;
    for(int i = 0; i< numberOfThreads; i++){
        totalTime += wtime[i];
        // printf("Thread %d timeElapsed: %lf\n", i, wtime[i]);
    }

    // Get the scheduling type and chunk size that was
    omp_sched_t kind;
    int chunk;
    omp_get_schedule(&kind, &chunk);
    const char *kind_verbose[4];
    kind_verbose[0] = "static";
    kind_verbose[1] = "dynamic";
    kind_verbose[2] = "guided";
    kind_verbose[3] = "auto";

    if(PLOTTING){
        printf("%u ", numberOfThreads);
        printf("%0.lf ", totalTime/numberOfThreads);
        printf("%s ", kind_verbose[kind-1]);
        printf("%d ", chunk);
    }else{
        printf("Number of threads: %u\n", numberOfThreads);
        printf("Average time per thread: %0.lf ms\n", totalTime/numberOfThreads);
    }
    

    return nbConducting;
}


int main(int argc, char **argv) {
    unsigned int flag, N, M;
    float d;


    assert(argc >= 4); // Check number of arguments
    //Scan arguments
    sscanf(argv[1], "%u", &flag);
    sscanf(argv[2], "%u", &N);
    sscanf(argv[3], "%f", &d);

    //Check argument validity
    assert((flag == 0) || (flag == 1));
    assert(N > 0);
    assert(d >= 0 && d <= 1);

    // Deadline
    if(flag == 1){
        assert(argc == 5);
        sscanf(argv[4], "%u", &M);
        assert(M > 0);
        if(PLOTTING){
            printf("%u %u %.3f %u ", flag, N, d, M);
        }else{
            printf("Args :  Flag: %u  N: %u  d: %.3f  M: %u\n", flag, N, d, M);
        }
    // Intermediate deadline
    }else{
        if(PLOTTING){
            printf("%u %u %.3f \n", flag, N, d);
        }else{
            printf("Args :  Flag: %u  N: %u  d: %.3f\n", flag, N, d);
        }
    }

    srand(time(NULL));
    long nbConductingFibers =(float) d *(N * N);

    if(flag == 0){ //Intermediate deadline

        PPMImage *image = malloc(sizeof(PPMImage));

        if (image == NULL) {
            perror("Error! memory not allocated.");
            exit(EXIT_FAILURE);
        }

        image->data = malloc(N * N * sizeof(PPMPixel));

        if (image->data == NULL) {
            perror("Error! memory not allocated.");
            free(image);
            exit(EXIT_FAILURE);
        }

        int *grid = malloc(N * N * sizeof(int));
        if (grid == NULL) {
            perror("Error! memory not allocated.");
            free(image->data);
            free(image);
            exit(EXIT_FAILURE);
        }
        createConductingFibers(grid, nbConductingFibers,N);
        unsigned int gridConductivity = isGridConducting(grid, N);
        printf("Grid is conducting? : %s\n", gridConductivity ? "Yes!" : "No!");
        // -----------Creating image------------------

        createImage(grid, N, image, "conductingMaterial.ppm");
        free(grid);
        free(image->data);
        free(image);
 
    }else{ //Deadline
        unsigned int nbConducting = monteCarlo(nbConductingFibers, N, M);
        float probability = (float)nbConducting / M;
        if(PLOTTING){
            printf("%.3f\n", probability);
        } else{
            printf("Probability of conductivity: %.3f (%d/%d)\n", probability, nbConducting, M);
        }
    }


}
