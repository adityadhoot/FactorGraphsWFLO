// ################################################################################
//
// name:          header.cc      
//
// author:        Martin Pelikan
//
// purpose:       prints out the header saying the name of the product, its author,
//                the date of its release, and the file with input parameters (if 
//                any)
//
// last modified: August 2000
//
// ################################################################################

#include <stdio.h>

#include "header.h"
#include "startUp.h"

// ================================================================================
//
// name:          printTheHeader
//
// function:      prints out the product name, its author, its version, and the
//                date of its realease
//
// parameters:    out..........output stream
//
// returns:       (int) 0
//
// ================================================================================

int printTheHeader(FILE *out)
{
  // is output stream non-null?

  if (out==NULL)
    return 0;

  // print the header to the output file

  fprintf(out,"\n====================================================\n");
  fprintf(out," Bayesian Optimization Algorithm with Decision Graphs\n");
  fprintf(out," and an incorporated complexity measure\n");
  fprintf(out,"\n");
  fprintf(out," Version 1.1 (Released in August 2000)\n");
  fprintf(out," Copyright (c) 2000 Martin Pelikan\n");
  fprintf(out," Author: Martin Pelikan\n");
  fprintf(out,"-----------------------------------------------\n");
  fprintf(out," Parameter values from: %s\n",(getParamFilename())? getParamFilename():"(default)");
  fprintf(out,"===============================================\n");
  fprintf(out,"\n");

  // get back

  return 0;
}
