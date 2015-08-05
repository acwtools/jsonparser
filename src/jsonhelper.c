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

#include "jsonhelper.h"


struct json* _jsonGetByKey( struct json* object, const char* key, size_t* index )
{
    struct json** child = NULL;
    if( object == NULL || object->type != JSON_TYPE_OBJECT )
    {
        return NULL;
    }

    child = object->children;
    while( *child != NULL  )
    {
        if( (*child)->value.s[0] == key[0] )
        {
            if( strcmp( (*child)->value.s, key ) == 0 )
            {
                return (*child);
            }
        }
        child++;
        (*index)++;
    }

    return NULL;
}


struct json* jsonGetByKey( struct json* object, const char* key )
{
    size_t index = 0;
    struct json* child = _jsonGetByKey(object, key, &index);
    if( child != NULL && child->children != NULL && child->children[0] != NULL )
    {
        return child->children[0];
    }

    return NULL;
}


char jsonRemoveByKey( struct json* object, const char* key )
{
    size_t index = 0;
    struct json* child = _jsonGetByKey(object, key, &index);
    if( child != NULL )
    {
        object->children = jsonRemoveItem(object->children, index, 1);
        return 1;
    }

    return 0;
}


char jsonAddPair(struct json* object, const char* key, struct json* value)
{
    struct json* child = NULL;
    if( object == NULL || object->type != JSON_TYPE_OBJECT )
    {
        return 0;
    }

    child = newJSON(JSON_TYPE_KEY);
    child->value.s = copyValue(key);
    child->children = jsonPushNode(child->children, value);
    object->children = jsonPushNode(object->children, child);

    return 1;
}

struct json** jsonPushNode(struct json** list, struct json* newnode)
{
    size_t size = 0;
    struct json** newlist = NULL;

    if(newnode == NULL)
    {
        return list;
    }

    if(list == NULL)
    {
        list = malloc(sizeof(struct json*)*2);

        if(list == NULL)
        {
            /* memoryFailure(); */
            exit(EXIT_FAILURE);
        }

        list[0] = newnode;
        list[1] = NULL;
        return list;
    }

    size = jsonListLength(list);
    newlist = realloc(list, (sizeof(struct json*)*(size+2)));
    if(newlist == NULL)
    {
        /* memoryFailure(); */
        exit(EXIT_FAILURE);
    }

    list = newlist;
    list[size+1] = list[size];
    list[size] = newnode;

    return list;

}

struct json** jsonMergeList(struct json** left, struct json** right)
{
    size_t leftsize = 1, rightsize = 1, newsize = 0, k = 0, i = 0;
    struct json** tmp = left;
    struct json** newlist = NULL;

    if(left == NULL && right == NULL)
    {
        return NULL;
    }

    if(left != NULL && right == NULL)
    {
        return left;
    }

    if(left == NULL && right != NULL)
    {
        return right;
    }

    while(*tmp != NULL)
    {
        leftsize++;
        tmp++;
    }

    leftsize--;

    tmp = right;
    while(*tmp != NULL)
    {
        rightsize++;
        tmp++;
    }

    newsize = leftsize+rightsize;
    newlist = realloc(left, (sizeof(struct json*) * newsize));
    if(newlist == NULL)
    {
        /* memoryFailure(); */
        exit(EXIT_FAILURE);
    }

    left = newlist;
    for(i = leftsize; i < newsize; i++, k++)
    {
        left[i] = right[k];
    }

    free(right);
    return left;
}

size_t jsonListLength(struct json** list)
{
    size_t listLength = 0;
    struct json** tmp = list;

    if(list == NULL)
    {
        return 0;
    }

    if(*list == NULL)
    {
        return 0;
    }

    while(*tmp != NULL)
    {
        listLength++;
        tmp++;
    }

    return listLength;
}

size_t jsonIndexof(struct json** list, struct json* item, char* error)
{
    size_t index = 0;
    struct json** tmp = list;

    *error=0;
    if(list == NULL)
    {
        *error=1;
        return 0;
    }

    if(*list == NULL)
    {
        *error=1;
        return 0;
    }

    while(*tmp != NULL && *tmp != item)
    {
        index++;
        tmp++;
    }

    if(*tmp == 0)
    {
        *error=1;
        return 0;
    }

    return index;
}

struct json** jsonRemoveItem(struct json** list, size_t start, size_t len)
{
    size_t newsize = 0, listlen = 0, ilen = 0, i = 0, k = 0;
    struct json** newlist = NULL;

    if(len == 0)
    {
        return list;
    }

    listlen = jsonListLength(list);

    if(listlen <= start)
    {
        return list;
    }

    newsize = listlen - len;

    if(newsize < 1)
    {
        jsonDeleteList(list);
        return NULL;
    }

    ilen = start + len;

    if(ilen > listlen)
    {
        return list;
    }

    newlist = malloc(sizeof(struct json*) * (newsize+1));
    if(newlist == NULL)
    {
        /* memoryFailure(); */
        exit(EXIT_FAILURE);
    }

    for(i = 0; i < start; i++)
    {
        newlist[i] = list[i];
    }

    for(k = i; i < ilen; i++)
    {
        jsonDeleteTree(list[i]);
    }

    for(; k <= newsize; k++, i++)
    {
        newlist[k] = list[i];
    }

    free(list);

    return newlist;
}

struct json** jsonInsertItem(struct json** list, size_t start, struct json* item)
{
    size_t listlen = jsonListLength(list), i = 0;
    struct json** newlist = realloc(list, sizeof(struct json*) * (listlen+2));

    if(newlist == NULL)
    {
        /* memoryFailure(); */
        exit(EXIT_FAILURE);
    }

    list = newlist;
    for(i = listlen; i >= start; i--)
    {
        list[i + 1] = list[i];
    }

    list[start] = item;

    return list;
}

void jsonDeleteTree(struct json* node)
{
    if(node->children != NULL)
    {
        size_t i = 0, listlen = jsonListLength(node->children);
        for(; i < listlen; i++)
        {
            jsonDeleteTree(node->children[i]);
        }
        free(node->children);
    }

    if( ( node->type == JSON_TYPE_KEY || node->type == JSON_TYPE_STRING ) && node->value.s != NULL )
    {
        free( node->value.s );
    }

    free(node);
}

void jsonDeleteList(struct json** list)
{
    struct json** tmp = list;
    while((*tmp)!=NULL)
    {
        struct json* current = *tmp;
        tmp++;
        jsonDeleteTree(current);
    }
    free(list);
}

struct json* jsonCopyTree(struct json* node)
{
    struct json* copy = newJSON(node->type);

    if(node->children != NULL)
    {
        size_t i = 0, listlen = jsonListLength(node->children);

        for(; i < listlen; i++)
        {
            copy->children = jsonPushNode(copy->children, jsonCopyTree(node->children[i]));
        }
    }

    if( ( node->type == JSON_TYPE_KEY || node->type == JSON_TYPE_STRING ) && node->value.s != NULL )
    {
        copy->value.s = copyValue( node->value.s );
    }

    else if( node->type == JSON_TYPE_INT )
    {
        copy->value.i = copy->value.i;
    }

    else if( node->type == JSON_TYPE_FLOAT )
    {
        copy->value.f = copy->value.f;
    }
    else if( node->type == JSON_TYPE_BOOL )
    {
        copy->value.b = copy->value.b;
    }

    return copy;
}

struct json** _jsonCopyList(struct json** list, size_t length, size_t start, size_t end, char deep)
{
    struct json** newlist = malloc(sizeof(struct json*) * (length+2));
    size_t i = 0;
    for(; start < end; start++, i++)
    {
        newlist[i] = deep ? jsonCopyTree(list[start]) : list[start];
    }

    newlist[i] = NULL;
    return newlist;
}

struct json** jsonSSlice(struct json** list, size_t start)
{
    return jsonSlice(list, start, jsonListLength(list));
}

struct json** jsonSlice(struct json** list, size_t start, size_t end)
{
    size_t length = start-end;

    return _jsonCopyList(list,  length,  start,  end, 0);
}

struct json** jsonCopyList(struct json** list)
{
    size_t len = jsonListLength(list);
    return _jsonCopyList(list,  len,  0,  len, 1);
}



