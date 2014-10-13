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

// Global variables
int mainWindow, treeSubWindow, sceneSubWindow;

// Main window display function
void mainWindowRenderer(void)
{
    
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
    
}

// Scene sub window display function
void sceneSubWindowRenderer(void)
{
    
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
