/*
    treesconsts.h
    
    Provides constants for tree subroutines.
    
     Author: Andrew Hills (a.hills@sheffield.ac.uk)
    Version: 0.1 (18th September 2014)
    
*/

#define MAX_BOUNDING_BOXES                      (2 * MAX_TRIANGLES)
#define MAX_TREE_DEPTH                          20
#define FIND_NEAREST_INITIAL_DIST               10000.0

// Defines for bounding box elements:
#define TREE_BOUNDING_BOX_LOCATION_X            0
#define TREE_BOUNDING_BOX_LOCATION_Y            1
#define TREE_BOUNDING_BOX_LOCATION_Z            2
#define TREE_BOUNDING_BOX_SIZE_X                3
#define TREE_BOUNDING_BOX_SIZE_Y                4
#define TREE_BOUNDING_BOX_SIZE_Z                5
#define TREE_BOUNDING_BOX_ARRAY_SIZE            6

// Defines for the tree list:
#define TREE_LIST_NEXT_INDEX                    0
#define TREE_LIST_EXTEND_LEFT                   1
#define TREE_LIST_EXTEND_RIGHT                  2
#define TREE_LIST_SPLIT_POSITION                3
#define TREE_LIST_SIZE                          4

// Defines fo the tree matrix
#define TREE_MATRIX_SPLIT_POSITION              0
#define TREE_MATRIX_AXIS_INDEX                  1
#define TREE_MATRIX_LEFT_NODE                   2
#define TREE_MATRIX_RIGHT_NODE                  3
#define TREE_MATRIX_LEAF_NODE                   4
#define TREE_MATRIX_SIZE                        5

// Defines for internal tree stack
#define TREE_STACK_NODE_INDEX                   0
#define TREE_STACK_DISTANCE                     1
#define TREE_STACK_VECTOR_X                     2
#define TREE_STACK_VECTOR_Y                     3
#define TREE_STACK_VECTOR_Z                     4
#define TREE_STACK_PREVIOUS_INDEX               5
#define TREE_STACK_SIZE                         6

#define SPLIT_LIST_SPLIT_POSITION               0
#define SPLIT_LIST_NEXT_INDEX                   1
#define SPLIT_LIST_LEFT_COUNT                   2
#define SPLIT_LIST_RIGHT_COUNT                  3
#define SPLIT_LIST_SIZE                         4

// Defines for node list
#define NODE_LIST_PRIMITIVE_INDEX               0
#define NODE_LIST_NEXT_INDEX                    1
#define NODE_LIST_SIZE                          2
