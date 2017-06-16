
/*-----------------------------------------------------------------------------
 * Name:    retarget_io.c
 * Purpose: Retarget I/O
 * Rev.:    1.1.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED
 
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
   
   
   #warning i need it if .... i use vector<> etc ... boh
   
//#define RETARGET_SYS
#define RTE_Compiler_IO_STDERR
#define RTE_Compiler_IO_STDERR_User
 
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <rt_sys.h>

 

 
 
#ifndef STDIN_ECHO
#define STDIN_ECHO      0       /* STDIN: echo to STDOUT */
#endif
#ifndef STDOUT_CR_LF
#define STDOUT_CR_LF    0       /* STDOUT: add CR for LF */
#endif
#ifndef STDERR_CR_LF
#define STDERR_CR_LF    0       /* STDERR: add CR for LF */
#endif
 
 
 
 

 
 
/**
  Put a character to the stderr
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
#if   defined(RTE_Compiler_IO_STDERR)
#if   defined(RTE_Compiler_IO_STDERR_User)
extern int stderr_putchar (int ch);
#elif defined(RTE_Compiler_IO_STDERR_ITM)

#elif defined(RTE_Compiler_IO_STDERR_BKPT)

#endif
#endif
 
 
#ifdef __MICROLIB

 
#else  /* __MICROLIB */
 

#if (defined(RTE_Compiler_IO_STDIN)  || \
     defined(RTE_Compiler_IO_STDOUT) || \
     defined(RTE_Compiler_IO_STDERR) || \
     defined(RTE_Compiler_IO_File))
#define RETARGET_SYS
 
/* IO device file handles. */
#define FH_STDIN    0x8001
#define FH_STDOUT   0x8002
#define FH_STDERR   0x8003
// User defined ...
 
/* Standard IO device name defines. */
const char __stdin_name[]  = ":STDIN";
const char __stdout_name[] = ":STDOUT";
const char __stderr_name[] = ":STDERR";

#endif


__attribute__((weak))
FILEHANDLE _sys_open (const char *name, int openmode) {

  (void)openmode;
    
  if (name == NULL) {
    return (-1);
  }
 
  if (name[0] == ':') {
    if (strcmp(name, ":STDIN") == 0) {
      return (FH_STDIN);
    }
    if (strcmp(name, ":STDOUT") == 0) {
      return (FH_STDOUT);
    }
    if (strcmp(name, ":STDERR") == 0) {
      return (FH_STDERR);
    }
    return (-1);
  }
  
  return (-1);
}

int _sys_close (FILEHANDLE fh) {

  switch (fh) {
    case FH_STDIN:
      return (0);
    case FH_STDOUT:
      return (0);
    case FH_STDERR:
      return (0);
  }

  return (-1);
}

__attribute__((weak))
int _sys_write (FILEHANDLE fh, const uint8_t *buf, uint32_t len, int mode) {

  int ch;
    
  switch (fh) {
    case FH_STDIN:
      return (-1);
    case FH_STDOUT:
      return (0);
    case FH_STDERR: 
      for (; len; len--) {
        ch = *buf++; 
        stderr_putchar(ch);
      } 
      return (0);
  } 

  return (-1);  
}

__attribute__((weak))
int _sys_read (FILEHANDLE fh, uint8_t *buf, uint32_t len, int mode) {
    return -1;
}

__attribute__((weak))
int _sys_istty (FILEHANDLE fh) {
    
  switch (fh) {
    case FH_STDIN:
      return (1);
    case FH_STDOUT:
      return (1);
    case FH_STDERR:
      return (1);
  }
 
  return (0);
}


__attribute__((weak))
int _sys_seek (FILEHANDLE fh, long pos) {

  switch (fh) {
    case FH_STDIN:
      return (-1);
    case FH_STDOUT:
      return (-1);
    case FH_STDERR:
      return (-1);
  }

  return (-1);
}


__attribute__((weak))
long _sys_flen (FILEHANDLE fh) {
    
  switch (fh) {
    case FH_STDIN:
      return (0);
    case FH_STDOUT:
      return (0);
    case FH_STDERR:
      return (0);
  }    

    return 0;
}


extern int stderr_putchar (int ch)
{
    return ch;
}

// nothing else .....
 
 
///**
//  Defined in rt_sys.h, this function opens a file.
// 
//  The _sys_open() function is required by fopen() and freopen(). These
//  functions in turn are required if any file input/output function is to
//  be used.
//  The openmode parameter is a bitmap whose bits mostly correspond directly to
//  the ISO mode specification. Target-dependent extensions are possible, but
//  freopen() must also be extended.
// 
//  \param[in] name     File name
//  \param[in] openmode Mode specification bitmap
// 
//  \return    The return value is –1 if an error occurs.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//FILEHANDLE _sys_open (const char *name, int openmode) {
//#if (!defined(RTE_Compiler_IO_File))
//  (void)openmode;
//#endif
// 
//  if (name == NULL) {
//    return (-1);
//  }
// 
//  if (name[0] == ':') {
//    if (strcmp(name, ":STDIN") == 0) {
//      return (FH_STDIN);
//    }
//    if (strcmp(name, ":STDOUT") == 0) {
//      return (FH_STDOUT);
//    }
//    if (strcmp(name, ":STDERR") == 0) {
//      return (FH_STDERR);
//    }
//    return (-1);
//  }
// 
//#ifdef RTE_Compiler_IO_File

//#else
//  return (-1);
//#endif
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function closes a file previously opened
//  with _sys_open().
//  
//  This function must be defined if any input/output function is to be used.
// 
//  \param[in] fh File handle
// 
//  \return    The return value is 0 if successful. A nonzero value indicates
//             an error.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//int _sys_close (FILEHANDLE fh) {
// 
//  switch (fh) {
//    case FH_STDIN:
//      return (0);
//    case FH_STDOUT:
//      return (0);
//    case FH_STDERR:
//      return (0);
//  }
// 
//#ifdef RTE_Compiler_IO_File

//#else
//  return (-1);
//#endif
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function writes the contents of a buffer to a file
//  previously opened with _sys_open().
// 
//  \note The mode parameter is here for historical reasons. It contains
//        nothing useful and must be ignored.
// 
//  \param[in] fh   File handle
//  \param[in] buf  Data buffer
//  \param[in] len  Data length
//  \param[in] mode Ignore this parameter
// 
//  \return    The return value is either:
//             - a positive number representing the number of characters not
//               written (so any nonzero return value denotes a failure of
//               some sort)
//             - a negative number indicating an error.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//int _sys_write (FILEHANDLE fh, const uint8_t *buf, uint32_t len, int mode) {
//#if (defined(RTE_Compiler_IO_STDOUT) || defined(RTE_Compiler_IO_STDERR))
//  int ch;
//#elif (!defined(RTE_Compiler_IO_File))
//  (void)buf;
//  (void)len;
//#endif
//  (void)mode;
// 
//  switch (fh) {
//    case FH_STDIN:
//      return (-1);
//    case FH_STDOUT:
//#ifdef RTE_Compiler_IO_STDOUT
//      for (; len; len--) {
//        ch = *buf++;
//#if (STDOUT_CR_LF != 0)
//        if (ch == '\n') stdout_putchar('\r');
//#endif
//        stdout_putchar(ch);
//      }
//#endif
//      return (0);
//    case FH_STDERR:
//#ifdef RTE_Compiler_IO_STDERR
//      for (; len; len--) {
//        ch = *buf++;
//#if (STDERR_CR_LF != 0)
//        if (ch == '\n') stderr_putchar('\r');
//#endif
//        stderr_putchar(ch);
//      }
//#endif
//      return (0);
//  }
// 
//#ifdef RTE_Compiler_IO_File
//#ifdef RTE_Compiler_IO_File_FS
//  return (__sys_write(fh, buf, len));
//#endif
//#else
//  return (-1);
//#endif
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function reads the contents of a file into a buffer.
// 
//  Reading up to and including the last byte of data does not turn on the EOF
//  indicator. The EOF indicator is only reached when an attempt is made to read
//  beyond the last byte of data. The target-independent code is capable of
//  handling:
//    - the EOF indicator being returned in the same read as the remaining bytes
//      of data that precede the EOF
//    - the EOF indicator being returned on its own after the remaining bytes of
//      data have been returned in a previous read.
// 
//  \note The mode parameter is here for historical reasons. It contains
//        nothing useful and must be ignored.
// 
//  \param[in] fh   File handle
//  \param[in] buf  Data buffer
//  \param[in] len  Data length
//  \param[in] mode Ignore this parameter
// 
//  \return     The return value is one of the following:
//              - The number of bytes not read (that is, len - result number of
//                bytes were read).
//              - An error indication.
//              - An EOF indicator. The EOF indication involves the setting of
//                0x80000000 in the normal result.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//int _sys_read (FILEHANDLE fh, uint8_t *buf, uint32_t len, int mode) {
//#ifdef RTE_Compiler_IO_STDIN
//  int ch;
//#elif (!defined(RTE_Compiler_IO_File))
//  (void)buf;
//  (void)len;
//#endif
//  (void)mode;
// 
//  switch (fh) {
//    case FH_STDIN:
//#ifdef RTE_Compiler_IO_STDIN
//      ch = stdin_getchar();
//      if (ch < 0) {
//        return ((int)(len | 0x80000000U));
//      }
//      *buf++ = (uint8_t)ch;
//#if (STDIN_ECHO != 0)
//      stdout_putchar(ch);
//#endif
//      len--;
//      return ((int)(len));
//#else
//      return ((int)(len | 0x80000000U));
//#endif
//    case FH_STDOUT:
//      return (-1);
//    case FH_STDERR:
//      return (-1);
//  }
// 
//#ifdef RTE_Compiler_IO_File
//#ifdef RTE_Compiler_IO_File_FS
//  return (__sys_read(fh, buf, len));
//#endif
//#else
//  return (-1);
//#endif
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function writes a character to the console. The
//  console might have been redirected. You can use this function as a last
//  resort error handling routine.
//  
//  The default implementation of this function uses semihosting.
//  You can redefine this function, or __raise(), even if there is no other
//  input/output. For example, it might write an error message to a log kept
//  in nonvolatile memory.
// 
//  \param[in] ch character to write
//*/
//#if   defined(RTE_Compiler_IO_TTY)
//#if   defined(RTE_Compiler_IO_TTY_User)
//extern void ttywrch (int ch);
//__attribute__((weak))
//void _ttywrch (int ch) {
//  ttywrch(ch);
//}
//#elif defined(RTE_Compiler_IO_TTY_ITM)
//__attribute__((weak))
//void _ttywrch (int ch) {
//  ITM_SendChar(ch);
//}
//#elif defined(RTE_Compiler_IO_TTY_BKPT)
//__attribute__((weak))
//void _ttywrch (int ch) {
//  (void)ch;
//  __asm("BKPT 0");
//}
//#endif
//#endif
// 
 
///**
//  Defined in rt_sys.h, this function determines if a file handle identifies
//  a terminal.
// 
//  When a file is connected to a terminal device, this function is used to
//  provide unbuffered behavior by default (in the absence of a call to
//  set(v)buf) and to prohibit seeking.
// 
//  \param[in] fh File handle
// 
//  \return    The return value is one of the following values:
//             - 0:     There is no interactive device.
//             - 1:     There is an interactive device.
//             - other: An error occurred.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//int _sys_istty (FILEHANDLE fh) {
// 
//  switch (fh) {
//    case FH_STDIN:
//      return (1);
//    case FH_STDOUT:
//      return (1);
//    case FH_STDERR:
//      return (1);
//  }
// 
//  return (0);
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function puts the file pointer at offset pos from
//  the beginning of the file.
// 
//  This function sets the current read or write position to the new location pos
//  relative to the start of the current file fh.
// 
//  \param[in] fh  File handle
//  \param[in] pos File pointer offset
// 
//  \return    The result is:
//             - non-negative if no error occurs
//             - negative if an error occurs
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//int _sys_seek (FILEHANDLE fh, long pos) {
//#if (!defined(RTE_Compiler_IO_File))
//  (void)pos;
//#endif
// 
//  switch (fh) {
//    case FH_STDIN:
//      return (-1);
//    case FH_STDOUT:
//      return (-1);
//    case FH_STDERR:
//      return (-1);
//  }
// 
//#ifdef RTE_Compiler_IO_File
//#ifdef RTE_Compiler_IO_File_FS
//  return (__sys_seek(fh, (uint32_t)pos));
//#endif
//#else
//  return (-1);
//#endif
//}
//#endif
 
 
///**
//  Defined in rt_sys.h, this function returns the current length of a file.
// 
//  This function is used by _sys_seek() to convert an offset relative to the
//  end of a file into an offset relative to the beginning of the file.
//  You do not have to define _sys_flen() if you do not intend to use fseek().
//  If you retarget at system _sys_*() level, you must supply _sys_flen(),
//  even if the underlying system directly supports seeking relative to the
//  end of a file.
// 
//  \param[in] fh File handle
// 
//  \return    This function returns the current length of the file fh,
//             or a negative error indicator.
//*/
//#ifdef RETARGET_SYS
//__attribute__((weak))
//long _sys_flen (FILEHANDLE fh) {
// 
//  switch (fh) {
//    case FH_STDIN:
//      return (0);
//    case FH_STDOUT:
//      return (0);
//    case FH_STDERR:
//      return (0);
//  }
// 
//#ifdef RTE_Compiler_IO_File
//#ifdef RTE_Compiler_IO_File_FS
//  return (__sys_flen(fh));
//#endif
//#else
//  return (0);
//#endif
//}
//#endif
// 
 
#endif  /* __MICROLIB */

