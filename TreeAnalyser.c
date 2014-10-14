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
#include <GL/glut.h>
#include "TreeAnalyser.h"
#include "DAMSONCRaytracer/raytracer.h"
#include "DBSimulator/treeconsts.h"

// Prototype functions
void initialiseTreeDepthCounter(void);
void populateTreeDepthCounter(void);
void _childTreeDepthCounter(int nodeIdx, int depth);
void initialiseTreeNodeCounter(void);
void populateTreeNodeCounter(void);
int _childTreeNodeCounter(int nodeIdx);
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
int LoadScenery(char *filename);
void ReadTexture(int textureIdx, char *filename);
void setMaterial(int materialIdx, int textureIdx);

// Global variables
int mainWindow, treeSubWindow, sceneSubWindow;
int SceneryLoaded = 0, xOrigin = -1, yOrigin = -1;
float deltaAngleX = 0.0, deltaAngleY = 0.0, deltaMoveFB = 0.0, deltaMoveLR = 0.0, deltaMoveUD = 0.0, angleX = 0.0, angleY = 0.0;

// Camera angles
float lx = 0.0; lz = -1.0; 

// Tree variables
float StatsVector[STATS_VECTOR_SIZE];
float ObjectDB[MAX_TRIANGLES][TRIANGLE_SIZE];
float MaterialDB[MAX_MATERIALS][MATERIAL_SIZE];
int TextureDB[MAX_TEXTURES][TEXTURE_SIZE];
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
int noTriangles = 0;
int noMaterials = 0;
int noTextures = 0;

// Tree stat counter:
int TreeDepthCounter[MAX_TREE_DEPTH];
int TreeNodeCounter[MAX_BOUNDING_BOXES];

// A container for textures.
typedef struct Texture
{
    int *data;
}
Texture;

// Variable for accessing the texture data.
Texture Textures[MAX_TEXTURES];

// Function to initialise the tree depth counter:
void initialiseTreeDepthCounter(void)
{
    int n;
    for (n = 0; n < MAX_TREE_DEPTH; n++)
        TreeDepthCounter[n] = 0;
}

void populateTreeDepthCounter(void)
{
    // Call the child thread:
    _childTreeDepthCounter(0, 0);
}

void _childTreeDepthCounter(int nodeIdx, int depth)
{
    int n;
    
    // Check if this branch is a leaf node.
    if (TreeMatrix[nodeIdx][TREE_MATRIX_LEAF_NODE] < 0)
        return;
    
    TreeDepthCounter[depth]++;
    
    // First try the left:
    _childTreeDepthCounter(TreeMatrix[nodeIdx][TREE_MATRIX_LEFT_NODE], depth + 1);
    // Then try the right:
    _childTreeDepthCounter(TreeMatrix[nodeIdx][TREE_MATRIX_RIGHT_NODE], depth + 1);
}

void initialiseTreeNodeCounter(void)
{
    int n;
    for (n = 0; n < MAX_BOUNDING_BOXES; n++)
        TreeNodeCounter[n] = 0;
}

void populateTreeNodeCounter(void)
{
    _childTreeNodeCounter(0);
}

int _childTreeNodeCounter(int nodeIdx)
{
    int n, count = 0, idx;
    
    if (TreeMatrix[nodeIdx][TREE_MATRIX_LEAF_NODE] >= 0)
    {
        // Begin recursive search
        idx = TreeMatrix[nodeIdx][TREE_MATRIX_LEAF_NODE];
        
        while (idx >= 0)
        {
            count++;
            idx = NodeList[idx][NODE_LIST_NEXT_INDEX];
        }
        return count;
    }
    
    count = _childTreeNodeCounter(TreeMatrix[nodeIdx][TREE_MATRIX_LEFT_NODE]);
    count += _childTreeNodeCounter(TreeMatrix[nodeIdx][TREE_MATRIX_RIGHT_NODE]);
    
    TreeNodeCounter[nodeIdx] = count;
    
    return count;
}

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
    
    mainWindow = glutCreateWindow("Tree Analyser");
    
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
    
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glLoadIdentity();
    glOrtho(0.0, SCREEN_WIDTH - 1.0, 0.0, SCREEN_HEIGHT - 1.0, -1.0,  1.0);
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
    if (key == ESCAPE_KEY)
        exit(0);
}

// Keyboard special key capture
void specialFunc(int key, int xmouse, int ymouse)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            deltaMoveFB = 0.5;
            break;
        case GLUT_KEY_DOWN:
            deltaMoveFB = -0.5;
            break;
        case GLUT_KEY_LEFT:
            deltaMoveLR = -0.5;
            break;
        case GLUT_KEY_RIGHT:
            deltaMoveLR = 0.5;
            break;
        case GLUT_KEY_PAGE_UP:
            deltaMoveUD = 0.5;
            break;
        case GLUT_KEY_PAGE_DOWN:
            deltaMoveUD = -0.5;
            break;
    }
}

// Keyboard special release keys capture
void specialReleaseFunc(int key, int xmouse, int ymouse)
{
    switch(key)
    {
        case GLUT_KEY_UP:
        case GLUT_KEY_DOWN:
            deltaMoveFB = 0;
            break;
        case GLUT_KEY_LEFT:
        case GLUT_KEY_RIGHT:
            deltaMoveLR = 0;
            break;
        case GLUT_KEY_PAGE_UP:
        case GLUT_KEY_PAGE_DOWN:
            deltaMoveUD = 0;
            break;
    }
}

// Mouse button capture.
void mouseFunc(int button, int state, int xmouse, int ymouse)
{
    if (button == GLUT_RIGHT_BUTTON)
    {
        // Check if the button was released:
        if (state == GLUT_UP)
        {
            angleX += deltaAngleX;
            angleY += deltaAngleY;
            
            xOrigin = -1;
            yOrigin = -1;
        }
        else
        {
            xOrigin = xmouse;
            yOrigin = ymouse;
        }
    }
}

// Mouse movement capture.
void mouseMoveFunc(int xmouse, int ymouse)
{
    // If the xOrigin is positive then the right mouse button is held down:
    if (xOrigin >= 0)
    {
        // Update the deltaAngle:
        deltaAngleX = (xmouse - xOrigin) * 0.001;
        deltaAngleY = (ymouse - yOrigin) * 0.001;
    }
}


int main (int argc, char *argv[])
{
    char *currObj, *parVal = "", *treeFilename = 0, *sceneFilename = 0;
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
    
    // Check the tree variable for assignment.
    if (!treeFilename)
    {
        // It wasn't assigned to anything.
        printf("ERROR: Tree filename needs to be specified.\n\n");
        exit(-1);
    }
    
    // If here, we can load the tree:
    LoadTree(treeFilename);
    
    // Has the scenery filename been defined?
    if (sceneFilename)
    {
        // Yes, then load the scenery file.
        if(!LoadScenery(sceneFilename))
            printf("WARNING: Unable to load the scenery file \"%s\". Verify that it exists and is valid.\n\n", sceneFilename);
        else
            SceneryLoaded = 1;
    }
    
    // Now begin by computing the tree stats.
    initialiseTreeDepthCounter();
    
    // Then populate the node counter
    populateTreeDepthCounter();
    
    // Then get node stats. Start by initialising counters:
    initialiseTreeNodeCounter();
    // And then populate the array:
    populateTreeNodeCounter();
    
    return 1;
}

// Function to load a tree file to memory
void LoadTree(char *filename)
{
    FILE *fp;
    int n, m;
    
    printf("Restoring tree to memory...\n");
    
    // Open the file for reading
    fp = fopen(filename, "rb");
    
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

// Function to load the scenery file to memory and populate the object and material database
int LoadScenery(char *filename)
{
    FILE *fp;
    int x, y, z, a, b;
    int n, m, zeroCheck, matIdx, textIdx, localNoTriangles, localNoMaterials, localNoTextures;
    char *textureFilename;
    int DominantAxisIdx, NUDom, NVDom, NDDom, BUDom, BVDom, CUDom, CVDom, totalTriangles = noTriangles;
    
    // File initialisation:
    printf("Reading world file \"%s\"\n", filename);
    
    fp = fopen(filename, "rb");
    
    // Populate local counters
    fread(&localNoMaterials, sizeof(int), 1, fp);
    fread(&localNoTextures, sizeof(int), 1, fp);
    
    // Verify that we're not going to overflow:
    if (localNoTextures + noTextures > MAX_TEXTURES)
    {
        printf("Too many textures requested (%i). Current texture limit set to %i.\n\n", localNoTextures + noTextures, MAX_TEXTURES);
        fclose(fp);
        return 0;
    }
    if (localNoMaterials + noMaterials > MAX_MATERIALS)
    {
        printf("Too many materials requested (%i). Current material limit set to %i.\n\n", localNoMaterials + noMaterials, MAX_MATERIALS);
        fclose(fp);
        return 0;
    }
    
    // Now read the textures:
    for (n = 0; n < localNoTextures; n++)
    {
        // Read the filename:
        fread(&m, sizeof(int), 1, fp);
        textureFilename = (char *) malloc(sizeof(char) * (m + 1));
        // Blank memory:
        memset(textureFilename, 0, sizeof(char) * (m + 1));
        // Then read the m characters:
        fread(textureFilename, sizeof(char), m, fp);
        
        // Cater for the special case:
        if (strcmp(textureFilename, "terrain.tga") != 0 && m != 25)
        {
            memcpy(textureFilename, textureFilename + 12, 36);
            memset(textureFilename + 36, 0, sizeof(char) * 12);
        }
        else
        {
            if (m == 25)
                textureFilename[8] = '/';
        }
        
        // Read the texture to memory
        ReadTexture(noTextures + n, textureFilename);
        
        // Before finally freeing up the memory:
        free(textureFilename);
    }
    
    // Increment the global number of textures
    noTextures += localNoTextures;
    
    // We should now encounter a zero in the file. Verify this is the case:
    fread(&zeroCheck, sizeof(int), 1, fp);
    if (zeroCheck != 0)
    {
        printf("\nERROR: Error encountered entering filenames. Failed zero check.\n");
        fclose(fp);
        return 0;
    }
    
    // Now proceed to load materials
    for (n = 0; n < localNoMaterials; n++)
    {
        // Read the material index and then read the texture index:
        fread(&matIdx, sizeof(int), 1, fp);
        fread(&textIdx, sizeof(int), 1, fp);
        
        // Now create a material:
        setMaterial(matIdx, textIdx);
    }
    
    // Increment the global number of materials
    noMaterials == localNoMaterials;
    
    // We should now encounter a zero in the file. Verify this is the case:
    fread(&zeroCheck, sizeof(int), 1, fp);
    if (zeroCheck != 0)
    {
        printf("\nERROR: Error encountered pairing materials with textures. Failed zero check.\n");
        fclose(fp);
        return 0;
    }
    
    // Start the triangle loading process.
    fread(&localNoTriangles, sizeof(int), 1, fp);
    
    while(!feof(fp))
    {
        // Check to see if we're over the limit:
        if (localNoTriangles + noTriangles > MAX_TRIANGLES)
        {
            printf("Too many triangles requested (%i). Current triangle limit set to %i.\n\n", localNoTriangles + noTriangles, MAX_TRIANGLES);
            fclose(fp);
            return 0;
        }
        
        for (n = 0; n < localNoTriangles; n++)
        {
            // Start reading the parameters for each triangle
            
            // VECTOR A:
            // Vector coords
            fread(&x, sizeof(int), 1, fp);
            fread(&y, sizeof(int), 1, fp);
            fread(&z, sizeof(int), 1, fp);
            // UV Coords
            fread(&a, sizeof(int), 1, fp);
            fread(&b, sizeof(int), 1, fp);
            
            // Add vector and UV coords to the DB
            ObjectDB[noTriangles][TriangleAx] = (float) x / 65536.0;
            ObjectDB[noTriangles][TriangleAy] = (float) y / 65536.0;
            ObjectDB[noTriangles][TriangleAz] = (float) z / 65536.0;
            ObjectDB[noTriangles][TriangleAu] = (float) a / 65536.0;
            ObjectDB[noTriangles][TriangleAv] = (float) b / 65536.0;
            
            // VECTOR B:
            // Vector coords
            fread(&x, sizeof(int), 1, fp);
            fread(&y, sizeof(int), 1, fp);
            fread(&z, sizeof(int), 1, fp);
            // UV Coords
            fread(&a, sizeof(int), 1, fp);
            fread(&b, sizeof(int), 1, fp);
            
            // Add vector and UV coords to the DB
            ObjectDB[noTriangles][TriangleBx] = (float) x / 65536.0;
            ObjectDB[noTriangles][TriangleBy] = (float) y / 65536.0;
            ObjectDB[noTriangles][TriangleBz] = (float) z / 65536.0;
            ObjectDB[noTriangles][TriangleBu] = (float) a / 65536.0;
            ObjectDB[noTriangles][TriangleBv] = (float) b / 65536.0;
            
            // VECTOR C:
            // Vector coords
            fread(&x, sizeof(int), 1, fp);
            fread(&y, sizeof(int), 1, fp);
            fread(&z, sizeof(int), 1, fp);
            // UV Coords
            fread(&a, sizeof(int), 1, fp);
            fread(&b, sizeof(int), 1, fp);
            
            // Add vector and UV coords to the DB
            ObjectDB[noTriangles][TriangleCx] = (float) x / 65536.0;
            ObjectDB[noTriangles][TriangleCy] = (float) y / 65536.0;
            ObjectDB[noTriangles][TriangleCz] = (float) z / 65536.0;
            ObjectDB[noTriangles][TriangleCu] = (float) a / 65536.0;
            ObjectDB[noTriangles][TriangleCv] = (float) b / 65536.0;
            
            // TRIANGLE PARAMETERS:
            // The dominant axis:
            fread(&DominantAxisIdx, sizeof(int), 1, fp);
            ObjectDB[noTriangles][TriangleDominantAxisIdx] = (float) DominantAxisIdx / 65536.0;
            
            // vmu, wmu, normdom:
            fseek(fp, 9 * sizeof(int), SEEK_CUR);
            
            // VECTOR normcrvmuwmu:
            // Vector coords
            fread(&x, sizeof(int), 1, fp);
            fread(&y, sizeof(int), 1, fp);
            fread(&z, sizeof(int), 1, fp);
            
            // Add vector and UV coords to the DB
            ObjectDB[noTriangles][Trianglenormcrvmuwmux] = (float) x / 65536.0;
            ObjectDB[noTriangles][Trianglenormcrvmuwmuy] = (float) y / 65536.0;
            ObjectDB[noTriangles][Trianglenormcrvmuwmuz] = (float) z / 65536.0;
            
            fread(&NUDom, sizeof(int), 1, fp);
            fread(&NVDom, sizeof(int), 1, fp);
            fread(&NDDom, sizeof(int), 1, fp);
            fread(&BUDom, sizeof(int), 1, fp);
            fread(&BVDom, sizeof(int), 1, fp);
            fread(&CUDom, sizeof(int), 1, fp);
            fread(&CVDom, sizeof(int), 1, fp);
            
            ObjectDB[noTriangles][TriangleNUDom] = (float) NUDom / 65536.0;
            ObjectDB[noTriangles][TriangleNVDom] = (float) NVDom / 65536.0;
            ObjectDB[noTriangles][TriangleNDDom] = (float) NDDom / 65536.0;
            ObjectDB[noTriangles][TriangleBUDom] = (float) BUDom / 65536.0;
            ObjectDB[noTriangles][TriangleBVDom] = (float) BVDom / 65536.0;
            ObjectDB[noTriangles][TriangleCUDom] = (float) CUDom / 65536.0;
            ObjectDB[noTriangles][TriangleCVDom] = (float) CVDom / 65536.0;
            
            noTriangles++;
        }
        
        // Next, read the material index for this batch of triangles:
        fread(&matIdx, sizeof(int), 1, fp);
        
        // Triangles are now added. Assign material index to the triangles:
        for (n = totalTriangles; n < noTriangles; n++)
            ObjectDB[n][TriangleMaterialIdx] = (float) matIdx / 65536.0;
        
        // Then add this batch to the list:
        totalTriangles += localNoTriangles;
        
        // This is followed by a zero check:
        fread(&zeroCheck, sizeof(int), 1, fp);
        if (zeroCheck != 0)
        {
            printf("\nERROR: Error encountered pairing triangle points with UV values. Failed zero check.\n");
            fclose(fp);
            return 0;
        }
        // Now read the next number of triangle values. EOF will raise if this failed.
        fread(&localNoTriangles, sizeof(int), 1, fp);
    }
    
    // File read.
    printf("Scenery file read successfully.\n\n");
    
    // Close the file pointer before terminating:
    fclose(fp);
    return 1;
}

void ReadTexture(int textureIdx, char *filename)
{
    int width, height, i, size, pixelbits, channels;
    int a;
    int *bitmap;
    unsigned char *data;
    
    // Open the file
    FILE *f = fopen(filename, "rb");
    
    // Make sure it opened
    if (f)
    {
        // Extract height and width from file header
        unsigned char buffer[20];
        fread(buffer, 1, 20, f);
        width = *(buffer + 12) + 256 * *(buffer + 13);
        height = *(buffer + 14) + 256 * *(buffer + 15);
        pixelbits = *(buffer + 16);
        fclose(f);
        size = height * width;
        
        // Compute the number of channels to expect.
        channels = (pixelbits == 32) ? 4 : 3;
        
        // Now to read the pixel data
        f = fopen(filename, "rb");
        data = (unsigned char *)malloc(sizeof(unsigned char) * size * channels + 1024);
        fread(data, 1, size * channels + 1024, f);
        fclose(f);
        
        // Convert 8 bits to fixed point
        bitmap = malloc(sizeof(int) * size * channels);
        for (i = 0; i < size; i++)
        {
            // shifting by 8 is equivalent to 256. Note that as 1 starts at bit 16 and char is up to 8 bits, shift by eight to left to align.
            bitmap[i * channels + 0] = ((int)data[i * channels + 20]) << 8;
            bitmap[i * channels + 1] = ((int)data[i * channels + 19]) << 8;
            bitmap[i * channels + 2] = ((int)data[i * channels + 18]) << 8;
            // Then record alpha if necessary
            if (channels == 4)
                bitmap[i * channels + 3] = ((int)data[i * channels + 21]) << 8;
        }
        Textures[textureIdx].data = bitmap;
        
        // Free up the data variable:
        free(data);
        
        // Now populate the TextureDB array:
        TextureDB[textureIdx][TextureHeight] = height;
        TextureDB[textureIdx][TextureWidth] = width;
        TextureDB[textureIdx][TextureAlpha] = (channels == 3) ? 0 : 1;
        TextureDB[textureIdx][TextureMemStart] = 0;
    }
    else
    {
        printf("Error encountered opening texture file \"%s\".\n", filename);
    }
}

void setMaterial(int materialIdx, int textureIdx)
{
    int n;
    
    // Set the texture index:
    MaterialDB[materialIdx][MaterialTextureIndex] = (float) textureIdx / 65536.0;
    
    // Then set everything to defaults:
    for (n = 0; n < 3; n ++)
        MaterialDB[materialIdx][MaterialColour + n] = 0.7; // Light grey
}
