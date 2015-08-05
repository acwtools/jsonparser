/*
 * JSON Parser Helper Functions Copyright (C) 2014 by Gandalf Sievers
 * Created by Gandalf Sievers on 13.07.15.
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

#ifndef __jsonparser__jsonhelper__
#define __jsonparser__jsonhelper__

#include <stdlib.h>
#include <string.h>
#include "json.h"
#include "helper.h"

struct json* jsonGetByKey( struct json* object, const char* key );
char jsonRemoveByKey( struct json* object, const char* key );
char jsonAddPair(struct json* object, const char* key, struct json* value);
struct json** jsonPushNode(struct json** list, struct json* newnode);
size_t jsonListLength(struct json** list);
struct json** jsonMergeList(struct json** left, struct json** right);
size_t jsonIndexof(struct json** list, struct json* item, char* error);
struct json** jsonRemoveItem(struct json** list, size_t start, size_t len);
struct json** jsonInsertItem(struct json** list, size_t start, struct json* item);
void jsonDeleteTree(struct json* node);
void jsonDeleteList(struct json** list);
struct json* jsonCopyTree(struct json* node);
struct json** jsonSSlice(struct json** list, size_t start);
struct json** jsonSlice(struct json** list, size_t start, size_t end);
struct json** jsonCopyList(struct json** list);


#endif /* defined(__jsonparser__jsonhelper__) */
