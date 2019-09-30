#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct representing a Pixel with RGB values
typedef struct
{
    char red, green, blue;
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
 *  returns: new head pointer pointing at the start of the list
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

    for (int i = 0; i < N * N; i++) {
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
 * Fill the empty grid randomly with an amount of`nbConductingCells`conducting
 * fibers that are 3 cells long
 * 
 *  grid: pointer to a square array of integer values of size N*N
 *  nbConductingCells: integer specifying the number of conducting fibers
 *                      in the grid
 *  N: size of one side of the grid
 * 
 *  returns: /
 */
void createConductingFibers(int *grid, unsigned int nbConductingCells, unsigned int N) {

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
	  long index = i + rand() / (RAND_MAX / (N*N - i) + 1);
      long temp = randomIndex[index];
	  randomIndex[index] = randomIndex[i];
	  randomIndex[i] = temp;
	}
    
    for (long i = 0; i < nbConductingCells; i++) {
        int dir = rand() % 2;
       
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

int main(int argc, char **argv) {
    unsigned int flag, N;
    float d;

    sscanf(argv[1], "%d", &flag);
    sscanf(argv[2], "%d", &N);
    sscanf(argv[3], "%f", &d);

    printf("Flag: %d N: %d d: %f \n", flag, N, d);

    assert((flag == 0) || (flag == 1));
    assert(N > 0);
    assert(d >= 0 && d <= 1);
    // ----------- Allocation of memory ------------------

    int *grid = malloc(N * N * sizeof(int));

    if (grid == NULL) {
        perror("Error! memory not allocated.");
        exit(EXIT_FAILURE);
    }

    PPMImage *image = malloc(sizeof(PPMImage));

    if (image == NULL) {
        perror("Error! memory not allocated.");
        free(grid);
        exit(EXIT_FAILURE);
    }

    image->data = malloc(N * N * sizeof(PPMPixel));

    if (image->data == NULL) {
        perror("Error! memory not allocated.");
        free(grid);
        free(image);
        exit(EXIT_FAILURE);
    }

    // -----------Checking conductivity of the grid------------------
    srand(time(NULL));
    time_t before = clock();
    
    long nbConductingCells = (float) d *(N * N);
    createConductingFibers(grid, nbConductingCells,N);
    int gridConductivity = isGridConducting(grid, N);
    
    int timeElapsed = (clock() - before) * 1000 / CLOCKS_PER_SEC;
    printf("Grid is conducting? : %s\n", gridConductivity ? "Yes!" : "No!");
    printf("Time elapsed: %dms\n", timeElapsed);

    // -----------Creating image------------------

    createImage(grid, N, image, "conductingMaterial.ppm");

    free(grid);
    free(image->data);
    free(image);
}
