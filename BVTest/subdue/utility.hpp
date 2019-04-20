//---------------------------------------------------------------------------
// utility.c
//
// Miscellaneous utility functions.
//
// Subdue 5
//---------------------------------------------------------------------------
#ifndef utility_hpp
#define utility_hpp

#include <iostream>
#include "Subdue.hpp"
#include "Params.hpp"
#include <string>
#include <unordered_map>


typedef bool BOOLEAN ;
typedef unsigned long ULONG ;

using namespace std;

class utility{
    public :
		


    //---------------------------------------------------------------------------
    // NAME:    MaximumNodes
    //
    // INPUTS:  ULONG n
    //
    // RETURN:  n^MATCH_SEARCH_THRESHOLD_EXPONENT
    //
    // PURPOSE: Compute the maximum number of nodes (mappings) considered
    // by InexactGraphMatch before switching from exhaustive to greedy
    // search.  If MATCH_SEARCH_THRESHOLD_EXPONENT = 0.0, then assume
    // exhaustive matching desired and retun maximum ULONG.
    //---------------------------------------------------------------------------
    
    static ULONG MaximumNodes(ULONG n)
    {
        if (MATCH_SEARCH_THRESHOLD_EXPONENT == 0.0)
            return MAX_UNSIGNED_LONG;
        else
            return(ULONG) pow((double) n, (double) MATCH_SEARCH_THRESHOLD_EXPONENT);
    }
    //---------------------------------------------------------------------------
    // NAME: OutOfMemoryError
    //
    // INPUTS: (char *context)
    //
    // RETURN: (void)
    //
    // PURPOSE: Print out of memory error with context, and then exit.
    //---------------------------------------------------------------------------
    static void OutOfMemoryError(string context)
    {
        std::cout <<"ERROR: out of memory allocating %s.\n" << context<< endl;
        exit(1);
    }
    
    
    //---------------------------------------------------------------------------
    // NAME: PrintBoolean
    //
    // INPUTS: (BOOLEAN boolean)
    //
    // RETURN: (void)
    //
    // PURPOSE: Print BOOLEAN input as 'TRUE' or 'FALSE'.
    //---------------------------------------------------------------------------
    
    static void PrintBoolean(BOOLEAN boolean)
    {
        if (boolean)
            cout << "true" << endl;
        else
            cout << "false" << endl;
    }
    //---------------------------------------------------------------------------
    // NAME:    ReadToken
    //
    // INPUTS:  (char *token) - string into which token is copied
    //          (FILE *fp) - file pointer from which token is read
    //          (ULONG *pLineNo) - pointer to line counter in calling function
    //
    // RETURN:  (int) - length of token read
    //
    // PURPOSE: Read the next token from the given file.  A token is
    // defined as a string of non-whitespace characters, where whitespace
    // includes spaces, tabs, newlines, comments, and EOF.
    //---------------------------------------------------------------------------
    static int ReadToken(char *token, FILE *fp, ULONG *pLineNo)
    {
        char ch;
        int i = 0;
        
        // skip whitespace and comments
        ch = fgetc(fp);
        while ((ch == SPACE) || (ch == TAB) || (ch == CARRIAGERETURN) ||
               (ch == NEWLINE) || (ch == COMMENT))
        {
            if (ch == NEWLINE)
                (*pLineNo)++;
            if (ch == COMMENT)
            {
                while ((ch != NEWLINE) && (ch != (char)EOF))  // skip to end of line
                    ch = fgetc(fp);
                if (ch == NEWLINE)
                    (*pLineNo)++;
            }
            if (ch != (char)EOF)
                ch = fgetc(fp);
        }
        
        // read token
        if (ch == DOUBLEQUOTE)
        { // read until reaching another double quote
            do
            {
                token[i++] = ch;
                ch = fgetc(fp);
            } while ((ch != (char)EOF) && (ch != DOUBLEQUOTE));
            if (ch == DOUBLEQUOTE)
                token[i++] = ch;
            ch = fgetc(fp);
        }
        else
        { // read until reaching whitespace
            while ((ch != (char)EOF) && (ch != SPACE) && (ch != TAB) &&
                   (ch != CARRIAGERETURN) && (ch != NEWLINE) && (ch != COMMENT))
            {
                token[i++] = ch;
                ch = fgetc(fp);
            }
        }
        token[i] = '\0';
        
        // if token ended by NEWLINE, increment lineNo
        if (ch == NEWLINE)
            (*pLineNo)++;
        
        // if token ended by comment, go ahead and skip comment
        if (ch == COMMENT)
        {
            while ((ch != NEWLINE) && (ch != (char)EOF))
                ch = fgetc(fp);
            if (ch == NEWLINE)
                (*pLineNo)++;
        }
        
        return i;
    }
    
    //---------------------------------------------------------------------------
    // NAME:    ReadInteger
    //
    // INPUTS:  (FILE *fp) - file pointer from which number is read
    //          (ULONG *pLineNo) - pointer to line counter in calling function
    //
    // RETURN:  (ULONG) - integer read
    //
    // PURPOSE: Read an unsigned long integer from the given file.
    //---------------------------------------------------------------------------
    
    static ULONG ReadInteger(FILE *fp, ULONG *pLineNo)
    {
        ULONG i;
        char token[TOKEN_LEN];
        char *endptr;
        
        ReadToken(token, fp, pLineNo);
        i = strtoul(token, &endptr, 10);
        if (*endptr != '\0')
        {
            fprintf(stderr, "Error: expecting integer in line %lu.\n",
                    *pLineNo);
            exit(1);
        }
        return i;
    }
    
    
    //---------------------------------------------------------------------------
    // NAME: Log2Factorial
    //
    // INPUTS: (ULONG number)
    //         (Parameters *parameters)
    //
    // RETURN: (double) - lg (number !)
    //
    // PURPOSE: Computes the log base 2 of the factorial of the given
    // number.  Since this is done often, a cache is maintained in
    // parameters->log2Factorial.  NOTE: this procedure assumes that
    // parameters->log2Factorial[0..1] already set before first call.
    //---------------------------------------------------------------------------
    
    static double Log2Factorial(ULONG number, Params *parameters)
    {
        ULONG i;
        ULONG newSize;
        
        if (number >= parameters->log2FactorialSize)
        {
            // add enough room to array to encompass desired value and then some
            newSize = number + LIST_SIZE_INC;
            parameters->log2Factorial = (double *)
            realloc(parameters->log2Factorial, newSize * sizeof(double));
            if (parameters->log2Factorial == NULL)
                OutOfMemoryError("Log2Factorial");
            // compute new values
            for (i = parameters->log2FactorialSize; i < newSize; i++)
            {
                parameters->log2Factorial[i] =
                Log2(i) + parameters->log2Factorial[i - 1];
            }
            parameters->log2FactorialSize = newSize;
        }
        return parameters->log2Factorial[number];
    }
    
    
    //---------------------------------------------------------------------------
    // NAME: Log2
    //
    // INPUTS: (ULONG number)
    //
    // RETURN: (double)
    //
    // PURPOSE: Computes the log base 2 of the given number.
    //---------------------------------------------------------------------------
    
    static double Log2(ULONG number)
    {
        if (number <= 0)
            return 0.0;
        else
            return log((double) number) / LOG_2;
    }
    
    //---------------------------------------------------------------------------
    // NAME: AddVertexIndex
    //
    // INPUTS: (ULONG *vertexIndices) - array of indices to augment
    //         (ULONG n) - size of new array
    //         (ULONG index) - index to add to nth element of array
    //
    // RETURN: (ULONG *) - new vertex index array
    //
    // PURPOSE: Reallocate the given vertex index array and store the new
    // index in the nth element of the array.  This is used to build the
    // array of indices into the positive and negative examples graphs.
    //---------------------------------------------------------------------------
    
    static ULONG *AddVertexIndex(ULONG *vertexIndices, ULONG n, ULONG index)
    {
        vertexIndices = (ULONG *) realloc(vertexIndices, sizeof(ULONG) * n);
        if (vertexIndices == NULL)
            OutOfMemoryError("AddVertexIndex:vertexIndices");
        vertexIndices[n - 1] = index;
        return vertexIndices;
    }
    
    //---------------------------------------------------------------------------
    // NAME:  compare_ints
    //
    // INPUTS:  (const void *pa) - first value
    //          (const void *pb) - second value
    //
    // RETURN:  int - result of comparison (-1: >; 1: >; 0: =)
    //
    // PURPOSE:  Return the result of comparing two input values.
    //---------------------------------------------------------------------------
    
    static int compare_ints(const void *pa, const void *pb)
    {
        const int *a = (const int *) pa;
        const int *b = (const int *) pb;
        
        if (*a < *b)
            return -1;
        else if (*a > *b)
            return 1;
        else
            return 0;
    }
    
    //-------------------------------------------------------------------------
    // NAME:  GetOutputFileName
    //
    // INPUTS:  (char *suffix) - output file name suffix (before "_")
    //          (ULONG index) - output file name index (between "_" and ".txt");
    //
    // RETURN:  (char *) - name of output file
    //
    // PURPOSE: Generate name of output file for current increment.
    //-------------------------------------------------------------------------
    
    static char *GetOutputFileName(char *suffix, ULONG index)
    {
        char *fileName;
        char str[20];
        
        fileName = (char *)malloc(sizeof(char) * 50);
        
        sprintf(str, "_%d", (int) index);
        strcpy(fileName, suffix);
        strcat(fileName, str);
        strcat(fileName, ".txt");
        
        return fileName;
    }
    
    
    //------------------------------------------------------------------------------
    // NAME: SortIndices
    //
    // INPUT: (ULONG *A) - array being sorted
    //        (long p) - start index
    //        (long r) - end index
    //
    // RETURN:  void
    //
    // PURPOSE:  Implementation of the QuickSort algorithm.
    //------------------------------------------------------------------------------
    
    static void SortIndices(ULONG *A, long p, long r)
    {
        long q;
        
        if (p < r)
        {
            q = Partition(A, p, r);
            SortIndices(A, p, q-1);
            SortIndices(A, q+1, r);
        }
    }
    
    
    //------------------------------------------------------------------------------
    // NAME: Partition
    //
    // INPUT: (ULONG *A) - input array
    //        (long p) - start index
    //        (ulong r) - end index
    //
    // RETURN:  long
    //
    // PURPOSE:  Partition algorithm for QuickSort.
    //------------------------------------------------------------------------------
    
    static long Partition(ULONG *A, long p, long r)
    {
        ULONG x;
        long i,j;
        ULONG temp;
        
        x = A[r];
        i = p-1;
        for(j=p; j<=(r-1); j++)
        {
            if(A[j] <= x)
            {
                i++;
                // swap A[i], A[j]
                temp = A[i];
                A[i] = A[j];
                A[j] = temp;
            }
        }
        temp = A[i+1];
        A[i+1] = A[r];
        A[r] = temp;
        
        return (i+1);
    }

    
};

#endif
    

    

    

