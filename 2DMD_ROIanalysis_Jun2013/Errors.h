// Errors.h

#ifndef __ERRORS_H__
#define __ERRORS_H__

#define IMAGELIB_SUCCESS 0

// File errors
#define IMAGELIB_FILE_OPEN_ERROR -1
#define IMAGELIB_FILE_CREATION_ERROR -2
#define IMAGELIB_FILE_WRITE_ERROR -3
#define IMAGELIB_FILE_READ_ERROR -4

// Memory erros
#define IMAGELIB_MEMORY_ALLOCATION_ERROR -50
#define IMAGELIB_MEMORY_LOCK_ERROR -51

// Argument errors
#define IMAGELIB_NODIB -100

//
#define IMAGELIB_UNSUPPORTED_FILETYPE -200
#define IMAGELIB_HDIB_NULL -201
#define IMAGELIB_LOGICAL_PALETTE_CREATION_ERROR -202
#define IMAGELIB_NO_PALETTE_FOR_HIGH_COLOR -203
#define IMAGELIB_STRETCHDIBITS_ERROR -204
#define IMAGELIB_PALETTE_QUANTIZE_ERROR -205
#define IMAGELIB_ATTEMPT_CHANGE_TO_SAME 206
#define IMAGELIB_ROTATION_VALUE_ERROR -207
#define IMAGELIB_ROTATE_ERROR -208

#endif

