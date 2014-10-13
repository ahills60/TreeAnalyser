/* 
    TreeAnalyser.c
    
    Tree Analyser
    
    For reading, analysing and visualising trees from the ray tracer.
    
    Version: 0.1 (13th October 2014)
     Author: Andrew Hills (a.hills@sheffield.ac.uk)
    
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "TreeAnalyser.h"

// Prototype functions
void mainWindowRenderer(void);
void reshapeFunc(int newWidth, int newHeight);
void idleFunc(void);
void treeSubWindowRenderer(void);
void sceneSubWindowRenderer(void);
void initialiseGLUT(int argc, char *argv[]);
void initUI();
void keyboardFunc(unsigned char key, int xmouse, int ymouse);
void specialFunc(int key, int xmouse, int ymouse);
void specialReleaseFunc(int key, int xmouse, int ymouse);
void mouseFunc(int button, int state, int xmouse, int ymouse);
void mouseMoveFunc(int xmouse, int ymouse);
void LoadTree(char *filename);

// Global variables
int mainWindow, treeSubWindow, sceneSubWindow;

// Tree variables
float StatsVector[STATS_VECTOR_SIZE];
float ObjectDB[MAX_TRIANGLES][TRIANGLE_SIZE];
int SceneBoundingBox[TREE_BOUNDING_BOX_ARRAY_SIZE];
int TreeMatrix[MAX_BOUNDING_BOXES][TREE_MATRIX_SIZE];
int TreeList[MAX_TRIANGLES][TREE_LIST_SIZE];
int SplitList[MAX_TRIANGLES * 2 + 8][SPLIT_LIST_SIZE];
int NodeList[MAX_TRIANGLES * 2][NODE_LIST_SIZE];

// Counters for the above lists.
int SplitListTop = 0;
int noSplitListEntries = 0;
int noTreeListEntries = 0;
int noTreeMatrixEntries = 0;
int noNodeListEntries = 0;


// Main window display function
void mainWindowRenderer(void)
{
    glutSetWindow(mainWindow);
    glClear(GL_COLOR_BUFFER_BIT);
    glutSwapBuffers();
    
    // Call child display renderers:
    treeSubWindowRenderer();
    sceneSubWindowRenderer();
}

// Main window reshape function
void reshapeFunc(int newWidth, int newHeight)
{
    // Always force the size to be set to defaults
    glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
}

// Main window idle function
void idleFunc(void)
{
    glutPostRedisplay();
}

// Tree sub window display function
void treeSubWindowRenderer(void)
{
    glutSetWindow(treeSubWindow);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // Finally, swap buffers:
    glutSwapBuffers();
}

// Scene sub window display function
void sceneSubWindowRenderer(void)
{
    glutSetWindow(sceneSubWindow);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    // Finally, swap buffers:
    glutSwapBuffers();
}

// function to initialise GLUT window and output
void initialiseGLUT(int argc, char *argv[])
{
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Set up the window position:
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE);
    
    printf("Initialising GLUT... ");
    glutInit(&argc, argv);
    printf("Done\n");
    
    mainWindow = glutCreateWindow("DAMSON parser visualiser");
    
    // Attach renderers to the main window
    glutDisplayFunc(mainWindowRenderer);
    glutReshapeFunc(reshapeFunc);
    glutIdleFunc(idleFunc);
    // Then initialise UI elements
    initUI();
    
    treeSubWindow = glutCreateSubWindow(mainWindow, BORDER_SIZE, BORDER_SIZE, SCREEN_WIDTH / 3 - BORDER_SIZE / 2, SCREEN_HEIGHT - 2 * BORDER_SIZE);
    
    // Attach renderers to the tree sub window
    glutDisplayFunc(treeSubWindowRenderer);
    // Then initialise UI elements
    initUI();
    
    sceneSubWindow  = glutCreateSubWindow(mainWindow, SCREEN_WIDTH / 3 + BORDER_SIZE / 2, BORDER_SIZE, 2 * (SCREEN_WIDTH / 3) - BORDER_SIZE, SCREEN_HEIGHT - 2 * BORDER_SIZE);
    
    // Attach renderers to the scene sub window
    glutDisplayFunc(sceneSubWindowRenderer);
    // Then initialise UI elements
    initUI();
    
    glutDisplayFunc(displayFunc);
    glutIdleFunc(idleFunc);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialFunc);
    glutReshapeFunc(reshapeFunc);
    
    glViewport(0, 0, SceneWidth, SceneHeight);
    glLoadIdentity();
    glOrtho(0.0, SceneWidth - 1.0, 0.0, SceneHeight - 1.0, -1.0,  1.0);
}

// Common UI elements are initialised within this function
void initUI()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboardFunc);
    glutSpecialFunc(specialFunc);
    glutSpecialUpFunc(specialReleaseFunc);
    glutMouseFunc(mouseFunc);
    glutMotionFunc(mouseMoveFunc);
}

// Keyboard key capture
void keyboardFunc(unsigned char key, int xmouse, int ymouse)
{
    
}

// Keyboard speciak key capture
void specialFunc(int key, int xmouse, int ymouse)
{
    
}

// Keyboard special release keys capture
void specialReleaseFunc(int key, int xmouse, int ymouse)
{
    
}

// Mouse button capture.
void mouseFunc(int button, int state, int xmouse, int ymouse)
{
    
}

// Mouse movement capture.
void mouseMoveFunc(int xmouse, int ymouse)
{
    
}


int main (int argc, char *argv[])
{
    char *currObj, *parVal = "", *treeFilename, *sceneFilename;
    int isParam, i, n, a;
    
    printf("\nTreeAnalyser ");
    printf("Version: %i.%i.%i (%s)\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_DATE);
    printf("Author: Andrew Hills (a.hills@sheffield.ac.uk)\n\n");
    
    for (i = 1; i < argc; i++)
    {
        currObj = argv[i];
        isParam = 0;
        a = strlen(argv[i]);
        for (n = 0; n < a; n++)
        {
            if (argv[i][n] == '-')
                isParam = 1;
            else
                break;
        }
        if (isParam)
        {
            memmove(&currObj[0], &currObj[n], strlen(currObj) - n + 1);
            parVal = currObj;
        }
        else
        {
            if (strcmp(parVal, ""))
            {
                if (!strcmp(parVal, "tree"))
                {
                    // Read in the tree filename
                    treeFilename = currObj;
                }
                else if (!strcmp(parVal, "scene"))
                {
                    // Read in the scene filename
                    sceneFilename = currObj;
                }
                else
                {
                    printf("Unrecognised input \"%s\"\n\n", parVal);
                }
            }
        }
    }
    
    if (!tree)
    {
        // It wasn't assigned to anything.
        printf("ERROR: Tree filename needs to be specified.\n\n");
        exit(-1);
    }
    
    return 1;
}

// Function to load a tree file to memory
void LoadTree(char *filename)
{
    FILE *fp;
    int n, m;
    
    printf("Restoring tree to memory...\n");
    
    // Open the file for reading
    fp = fopen(filename, "r");
    
    if (!fp)
    {
        // Invalid file
        printf("ERROR: Unable to open \"%s\" for reading.\n\n", filename);
        exit(-100);
    }
    
    // If here, the file was opened successfully. Now start reading:
    for (n = 0; n < TREE_BOUNDING_BOX_ARRAY_SIZE; n++)
        fread(&SceneBoundingBox[n], sizeof(int), 1, fp);
    
    // Then load constants:
    fread(&SplitListTop, sizeof(int), 1, fp);
    fread(&noSplitListEntries, sizeof(int), 1, fp);
    fread(&noTreeListEntries, sizeof(int), 1, fp);
    fread(&noTreeMatrixEntries, sizeof(int), 1, fp);
    fread(&noNodeListEntries, sizeof(int), 1, fp);
    
    // Now import the tree matrix:
    for (m = 0; m < noTreeMatrixEntries; m++)
        for (n = 0; n < TREE_MATRIX_SIZE; n++)
            fread(&TreeMatrix[m][n], sizeof(int), 1, fp);
    
    // Then the tree list:
    for (m = 0; m < noTreeListEntries; m++)
        for (n = 0; n < TREE_LIST_SIZE; n++)
            fread(&TreeList[m][n], sizeof(int), 1, fp);
    
    // Then the split list:
    for (m = 0; m < noSplitListEntries; m++)
        for (n = 0; n < SPLIT_LIST_SIZE; n++)
            fread(&SplitList[m][n], sizeof(int), 1, fp);
    
    // Finally the node list:
    for (m = 0; m < noNodeListEntries; m++)
        for (n = 0; n < NODE_LIST_SIZE; n++)
            fread(&NodeList[m][n], sizeof(int), 1, fp);
    
    // As there's nothing else to read, close the file pointer:
    fclose(fp);
    
    printf("Tree state restored from \"%s\".\n\n", filename);
}
