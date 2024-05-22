/***********************************************************/
/*                                                         */
/* Amiga C Encyclopedia (ACE) V3.0      Amiga C Club (ACC) */
/* -------------------------------      ------------------ */
/*                                                         */
/* Book:    ACM System                  Amiga C Club       */
/* Chapter: AmigaDOS                    Tulevagen 22       */
/* File:    Example4.c                  181 41  LIDINGO    */
/* Author:  Anders Bjerin               SWEDEN             */
/* Date:    92-05-02                                       */
/* Version: 1.10                                           */
/*                                                         */
/*   Copyright 1992, Anders Bjerin - Amiga C Club (ACC)    */
/*                                                         */
/* Registered members may use this program freely in their */
/*     own commercial/noncommercial programs/articles.     */
/*                                                         */
/***********************************************************/

/* This example demonstrates how to delete files and directories. It */
/* will delete the file Example 1 and directory Example 2 created.   */
/* (The file and directory are supposed to have been renamed by      */
/* Example 3.)                                                       */


/* This file declares the type BOOL: */
#include <exec/types.h>


void main();
void main()
{
  BOOL ok;

  /* Delete the file: */
  ok = DeleteFile("ram:testdoor");

  /* Check if the file was successfully deleted: */
  if( !ok )
    printf( "The file could not be deleted!\n" );

  else
  printf(" The File was successfully deleted ! \n\r");
}

