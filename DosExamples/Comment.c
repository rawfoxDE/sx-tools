/***********************************************************/
/*                                                         */
/* Amiga C Encyclopedia (ACE) V3.0      Amiga C Club (ACC) */
/* -------------------------------      ------------------ */
/*                                                         */
/* Book:    ACM System                  Amiga C Club       */
/* Chapter: AmigaDOS                    Tulevagen 22       */
/* File:    Example5.c                  181 41  LIDINGO    */
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

/* This example demonstrates how to attach a short comment to a file. */
/* A short file called "Letter.doc" will be created, and a short      */
/* comment will be attached.                                          */
/* To see the comment use the CLI command "List".                     */


/* Declares BOOL: */
#include <exec/types.h>
/* Declares the FileHandle structure: */
#include <libraries/dos.h>


void main();

void main()
{
  struct FileHandle *file_handle;
  char letter[ 8 ] = { 'D', 'e', 'a', 'r', ' ', 'S', 'i', 'r' };
  long bytes_written;
  BOOL ok;


  /* Try to open file "Letter.doc" as a new file:     */
  /* (If the file does not exist, it will be created. */
  /* If it, on the the other hand, exist, it will be  */
  /* overwritten.)                                    */
  file_handle = (struct FileHandle *)
    Open( "RAM:Letter.doc", MODE_NEWFILE );
  
  /* Have we opened the file successfully? */
  if( file_handle == NULL )
  {
    printf( "Could not open the file!\n" );
    exit();
  }



  /* We have now opened a file, and are ready to start writing: */
  bytes_written = Write( file_handle, letter, sizeof( letter ) );

  if( bytes_written != sizeof( letter ) )
  {
    printf( "Could not save the documment!\n" );
    exit();
  }
  else
    printf( "The documment was successfully saved!\n" );



  /* Attach a short comment: */
  ok = SetComment( "RAM:Letter.doc", "A very short letter" );

  /* Check if the comment was successfully attached: */
  if( !ok )
    printf( "Could not attach the comment!\n" );
  else
    printf( "The comment was successfull attached to the file!\n" );



  /* Close the file: */
  Close( file_handle );
}
