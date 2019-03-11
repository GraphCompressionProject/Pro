//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_SUBDUE_HPP
#define SUBDUE_SUBDUE_HPP




// Substructure evaluation methods
#define EVAL_MDL      1
#define EVAL_SIZE     2
#define EVAL_SETCOVER 3

// Graph match search space limited to V^MATCH_SEARCH_THRESHOLD_EXPONENT
// If set to zero, then no limit
#define MATCH_SEARCH_THRESHOLD_EXPONENT 4.0

// Starting strings for input files
#define SUB_TOKEN        "S"  // new substructure
#define PREDEF_SUB_TOKEN "PS" // new predefined substructure
#define POS_EG_TOKEN     "XP" // new positive example
#define NEG_EG_TOKEN     "XN" // new negative example

// Vertex and edge labels used for graph compression
#define SUB_LABEL_STRING     "SUB"
#define OVERLAP_LABEL_STRING "OVERLAP"
#define PREDEFINED_PREFIX    "PS"

#define NEG 0
#define POS 1

// Costs of various graph match transformations
#define INSERT_VERTEX_COST             1.0 // insert vertex
#define DELETE_VERTEX_COST             1.0 // delete vertex
#define SUBSTITUTE_VERTEX_LABEL_COST   1.0 // substitute vertex label
#define INSERT_EDGE_COST               1.0 // insert edge
#define INSERT_EDGE_WITH_VERTEX_COST   1.0 // insert edge with vertex
#define DELETE_EDGE_COST               1.0 // delete edge
#define DELETE_EDGE_WITH_VERTEX_COST   1.0 // delete edge with vertex
#define SUBSTITUTE_EDGE_LABEL_COST     1.0 // substitute edge label
#define SUBSTITUTE_EDGE_DIRECTION_COST 1.0 // change directedness of edge
#define REVERSE_EDGE_DIRECTION_COST    1.0 // change direction of directed edge

// MPI message tags
#define MPI_MSG_EVAL     1
#define MPI_MSG_EVAL_SUB 2
#define MPI_MSG_BEST_SUB 3

// MPI_BUFFER_SIZE must be big enough to hold largest substructure (not
// including instances)
#define MPI_BUFFER_SIZE 16384

// Constants for graph matcher.  Special vertex mappings use the upper few
// unsigned long integers.  This assumes graphs will never have this many
// vertices, which is a pretty safe assumption.  The maximum double is used
// for initial costs.
#define MAX_UNSIGNED_LONG ULONG_MAX  // ULONG_MAX defined in limits.h
#define VERTEX_UNMAPPED   MAX_UNSIGNED_LONG
#define VERTEX_DELETED    MAX_UNSIGNED_LONG - 1
#define MAX_DOUBLE        DBL_MAX    // DBL_MAX from float.h

// Label types
#define STRING_LABEL  0
#define NUMERIC_LABEL 1

// General defines
#define LIST_SIZE_INC  100  // initial size and increment for realloc-ed lists
#define TOKEN_LEN     256  // maximum length of token from input graph file
#define FILE_NAME_LEN 512  // maximum length of file names
#define COMMENT       '%'  // comment character for input graph file
#define NUMERIC_OUTPUT_PRECISION 6
#define LOG_2 0.6931471805599452862 // log_e(2) pre-computed

#define SPACE ' '
#define TAB   '\t'
#define NEWLINE '\n'
#define DOUBLEQUOTE '\"'
#define CARRIAGERETURN '\r'

#define TRUE 1
#define FALSE 0


#define SUBDUE_VERSION "5.2.2"



//typedef bool BOOLEAN ;
//typedef unsigned long ULONG;



#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class Params;

class Subdue{
public:
    
    int main (int, char**);
    void ISubdue(Params *);
    Params *GetParameters(int, char **);
    void PrintParameters(Params *);
//    void FreeParameters(Params *);
    Params *PostProcessParameters(Params *parameters);
    
    
    
};




#endif //SUBDUE_SUBDUE_HPP
