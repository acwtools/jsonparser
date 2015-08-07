/*
 * JSON Parser Copyright (C) 2014 by Gandalf Sievers
 * Created by Gandalf Sievers on 27.06.15.
 *
 * MIT-License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef __jsonparser__json__
#define __jsonparser__json__

#include <stdlib.h>
#include <strings.h>
#include "utf8.h"
#include "jsonhelper.h"


#define JSON_TYPE_OBJECT 1
#define JSON_TYPE_ARRAY 2
#define JSON_TYPE_KEY 4
#define JSON_TYPE_STRING 8
#define JSON_TYPE_INT 16
#define JSON_TYPE_FLOAT 32
#define JSON_TYPE_BOOL 64

#define JSON_ERROR_OUTOFMEMORY 1
#define JSON_ERROR_KEYCHILDREN 2
#define JSON_ERROR_ILLEGALCHAR 4
#define JSON_ERROR_INVALIDTYPE 8
#define JSON_ERROR_KEYINARRAY 16

struct json
{
    char type;
    union value
    {
        int i;
        double f;
        char* s;
        char b;
    } value;
    struct json** children;
};

struct json* readJSON(const char* string, char* error);
char* writeJSON(struct json* node);

struct json* newJSON(char type);
size_t jsonListLength(struct json** list);

#endif /* defined(__jsonparser__json__) */