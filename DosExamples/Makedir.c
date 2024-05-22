/***********************************************************/
/*                                                         */
/* Amiga C Encyclopedia (ACE) V3.0      Amiga C Club (ACC) */
/* -------------------------------      ------------------ */
/*                                                         */
/* Book:    ACM System                  Amiga C Club       */
/* Chapter: AmigaDOS                    Tulevagen 22       */
/* File:    Example2.c                  181 41  LIDINGO    */
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

/* This example demonstrates how to create a directory called */
/* "MyDirectory" on the RAM disk.                             */


#include <libraries/dos.h>


void main();

void main()
{
  /* Declare a FileLock structure: */
  struct FileLock *lock;

  /* Create a directory on the RAM disk: (The directory will */
  /* be locked with an exclusive lock, and must therefore be */
  /* unlocked before the program terminates.)                */
  lock = (struct FileLock *) CreateDir( "RAM:MyDirectory" );

  /* If there is no lock, no directory has been created. In  */
  /* that case, inform the user about the problem and leave: */
  if( lock == NULL )
  {
    printf( "ERROR Could NOT create the new directory!\n" );
    exit( 0 );
  }

  /* Unlock the directory: */
  UnLock( lock );
}
