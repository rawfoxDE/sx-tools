/***********************************************************/
/*                                                         */
/* Amiga C Encyclopedia (ACE) V3.0      Amiga C Club (ACC) */
/* -------------------------------      ------------------ */
/*                                                         */
/* Book:    ACM System                  Amiga C Club       */
/* Chapter: AmigaDOS                    Tulevagen 22       */
/* File:    Example3.c                  181 41  LIDINGO    */
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

/* This example demonstrates how to rename files and directories. It  */
/* will rename the file Example 1 created (called "HighScore.dat") to */
/* "Numbers.dat". It will also rename the directory Example 2 created */
/* ("MyDirectory") to "NewDirectory".                                 */ 


/* This file declares the type BOOL: */
#include <exec/types.h>
#include <dos/dos.h>
#include <stdio.h>

void main();
void Rename(argv[],argc);
void main()
{
  BOOL ok;

  /* Rename the file: */
  ok = Rename( "RAM:testdoor", "RAM:RenamedTestdoor" );

  /* Check if the file was successfully renamed: */
  if( !ok )
    printf( "The file could not be renamed!\n" );

}
