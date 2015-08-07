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


#include "json.h"

struct json** _readJSON(const char* string, size_t* pos, char last, int* curly, int* square, char* error);


struct json* newJSON(char type)
{
    struct json* j = malloc(sizeof(struct json));
    memset(j, 0, sizeof(struct json));
    j->type = type;

    return j;
}

char* readJSONStr(const char* string, size_t start, size_t end, char* error)
{
    if(end <= start)
    {
        return NULL;
    }
    else
    {
        char* str = NULL;
        size_t len = end - start;

        str = malloc(sizeof(char)*(len+1));
        if ( str == NULL )
        {
            *error = JSON_ERROR_OUTOFMEMORY;
            return NULL;
        }
        memcpy(str, &string[start], len);
        str[len] = '\0';
        return str;
     }
}

char* parseJSONStr(const char* string, size_t start, size_t end, char* error)
{
    char* str = readJSONStr(string, start, end, error);
    size_t len, startlen = len = end - start;

    if(str != NULL)
    {
        size_t pos = 0;
        while(str[pos] != '\0')
        {
            if(str[pos] == '\\')
            {
                size_t next = pos+1;
                if(str[next] == '"')
                {
                    str[pos] = '"';
                }
                else if(str[next] == '\\')
                {
                    str[pos] = '\\';
                }
                else if(str[next] == '/')
                {
                    str[pos] = '/';
                }
                else if(str[next] == 'b')
                {
                    str[pos] = '\b';
                }
                else if(str[next] == 'f')
                {
                    str[pos] = '\f';
                }
                else if(str[next] == 'n')
                {
                    str[pos] = '\n';
                }
                else if(str[next] == 'r')
                {
                    str[pos] = '\r';
                }
                else if(str[next] == 't')
                {
                    str[pos] = '\t';
                }
                else if(str[next] == 'u')
                {
                    size_t utf8len = 0;
                    size_t lendiff = 0;
                    char* utf8char = parseUTF8char( &str[next+1], &utf8len );

                    memcpy( &str[pos], utf8char, utf8len );
                    free( utf8char );

                    pos += utf8len;
                    lendiff = 6 - utf8len;
                    next = pos + lendiff;

                    memmove(&str[pos], &str[next], len - next + 1);

                    len -= lendiff;
                    pos++;
                    continue;
                }
                memmove( &str[next], &str[next+1], len-next );
                len--;
            }
            pos++;
        }
    }

    if (len < startlen)
    {
        char* tmp = realloc(str, sizeof(char)* len);
        if(tmp != NULL)
        {
            str = tmp;
        }
        else
        {
            *error = JSON_ERROR_OUTOFMEMORY;
        }
    }

    return str;
}

int parseJSONint(const char* string, size_t start, size_t end, char* error)
{
    char* str = readJSONStr(string, start, end, error);
    if(str != NULL)
    {
        int i = atoi(str);
        free(str);
        return i;
    }
    return 0;
}

double parseJSONfloat(const char* string, size_t start, size_t end, char* error)
{
    char* str = readJSONStr(string, start, end, error);
    if(str != NULL)
    {
        double d = strtod(str, NULL);
        free(str);
        return d;
    }
    return 0;
}

struct json* readJSON( const char* string, char* error)
{
    size_t pos = 0;
    int curly = 0, square = 0;

    struct json** list = _readJSON(string, &pos, 0, &curly, &square, error);

    if (curly != 0 || square != 0 || *error != 0)
    {
        jsonDeleteList(list);
        return NULL;
    }

    return list[0];
}

struct json** _readJSON( const char* string, size_t* pos, char last, int* curly, int* square, char* error )
{
    struct json** children = NULL;

    while(string[*pos] != '\0')
    {
        if(string[*pos] == ' ')
        {
            (*pos)++;
        }
        else if(string[*pos] == '{')
        {
            struct json* obj = newJSON(JSON_TYPE_OBJECT);
            (*curly)++;
            (*pos)++;
            obj->children = _readJSON(string, pos, JSON_TYPE_OBJECT, curly, square, error);
            children = jsonPushNode(children, obj, error);
        }
        else if(string[*pos] == '[')
        {
            struct json* arr = newJSON(JSON_TYPE_ARRAY);
            (*square)++;
            (*pos)++;
            arr->children = _readJSON(string, pos, JSON_TYPE_ARRAY, curly, square, error);
            children = jsonPushNode(children, arr, error);
        }
        else if(string[*pos] == '}')
        {
            (*curly)--;
            (*pos)++;
            return children;
        }
        else if(string[*pos] == ']')
        {
            (*square)--;
            (*pos)++;
            return children;
        }
        else if(string[*pos] == '"')
        {
            struct json* str = newJSON( last == JSON_TYPE_OBJECT ? JSON_TYPE_KEY : JSON_TYPE_STRING );
            size_t start = *pos;

            while((string[*pos+1] != '"' || string[*pos] == '\\') && string[*pos+1] != '\0')
            {
                (*pos)++;
            }
            if (string[*pos+1] == '\0' && string[*pos] != '"')
            {
                (*pos)++;
                return NULL;
            }

            start++;
            (*pos)++;

            str->value.s = parseJSONStr(string, start, *pos, error);
            (*pos)++;
            if( last == JSON_TYPE_OBJECT )
            {
                str->children = _readJSON(string, pos, JSON_TYPE_KEY, curly, square, error);
            }
            children = jsonPushNode(children, str, error);

        }
        else if((string[*pos] > 47 && string[*pos] < 58) || string[*pos] == '.' || string[*pos] == '-')
        {
            struct json* number = NULL;
            size_t start = *pos;
            char isint=1;

            while((string[*pos] > 47 && string[*pos] < 58) || string[*pos] == '.' || string[*pos] == '+' || string[*pos] == '-' || string[*pos] == 'e' || string[*pos] == 'E')
            {
                if(string[*pos] == '.' || string[*pos] == 'e' || string[*pos] == 'E')
                {
                    isint=0;
                }
                (*pos)++;
            }

            number= newJSON( isint ? JSON_TYPE_INT : JSON_TYPE_FLOAT );
            if(isint)
            {
                number->value.i = parseJSONint(string, start, *pos, error);
            }
            else
            {
                number->value.f = parseJSONfloat(string, start, *pos, error);
            }
            children = jsonPushNode(children, number, error);

        }
        else if(string[*pos] == 't')
        {
            struct json* b = newJSON( JSON_TYPE_BOOL );
            b->value.b = 1;
            children = jsonPushNode(children, b, error);
            (*pos)+=4;
        }
        else if(string[*pos] == 'f')
        {
            struct json* b = newJSON( JSON_TYPE_BOOL );
            b->value.b = 0;
            children = jsonPushNode(children, b, error);
            (*pos)+=5;
        }
        else if(string[*pos] == ',' && last == JSON_TYPE_KEY)
        {
            (*pos)++;
            return children;
        }
        else if ((string[*pos] == ':' && last == JSON_TYPE_KEY) || (string[*pos] == ','))
        {
            (*pos)++;
        }
        else
        {
            *error = JSON_ERROR_ILLEGALCHAR;
            printf("%c", string[*pos]);
            return children;
        }
    }
    return children;
}

char* _writeJSONStringValue( char* jsonString, const char* str,  size_t* len, char key, char* error)
{
    char* encoded = encodeUTF8str( str );
    size_t olen = strlen(encoded);

    char* tmp = realloc(jsonString, sizeof(char) * (*len + olen + (key !=0 ? 4 : 3) ));
    if (tmp != NULL)
    {
        jsonString = tmp;

        jsonString[*len] = '"';
        memcpy(&jsonString[*len+1], encoded, olen);
        free(encoded);
        *len += olen +1;
        jsonString[*len] = '"';
        (*len)++;
         return jsonString;
    }

    *error = 1;
    return NULL;
}

void _writeJSON(struct json* node, char** jsonString, size_t* len, char* error)
{
    if(!*error)
    {
        switch (node->type)
        {
            case JSON_TYPE_OBJECT:
            case JSON_TYPE_ARRAY:
            {
                size_t childlen = jsonListLength(node->children);
                size_t i = 0;

                char* tmp = realloc(*jsonString, sizeof(char) * (*len+2));

                if ( tmp != NULL )
                {
                    *jsonString = tmp;
                    (*jsonString)[*len] = node->type == JSON_TYPE_OBJECT ? '{' : '[';
                    (*jsonString)[*len+1] = '\0';
                    (*len)++;
                }
                for( ;i < childlen; i++)
                {
                    if(( node->type == JSON_TYPE_OBJECT && node->children[i]->type != JSON_TYPE_KEY ) || ( node->type == JSON_TYPE_ARRAY && node->children[i]->type == JSON_TYPE_KEY ))
                    {
                        *error = JSON_ERROR_INVALIDTYPE;
                        break;
                    }

                    _writeJSON(node->children[i], jsonString, len, error);
                    if (i < childlen-1 )
                    {
                        char* tmp = realloc(*jsonString, sizeof(char) * (*len+2));
                        if ( tmp != NULL )
                        {
                            *jsonString = tmp;
                            (*jsonString)[*len] = ',';
                            (*jsonString)[*len+1] = '\0';
                            (*len)++;
                        }
                        else
                        {
                            *error = JSON_ERROR_OUTOFMEMORY;
                            break;
                        }

                    }
                }

                tmp = realloc(*jsonString, sizeof(char) * (*len+2));
                if ( tmp != NULL )
                {
                    *jsonString = tmp;
                    (*jsonString)[*len] = node->type == JSON_TYPE_OBJECT ? '}' : ']';
                    (*jsonString)[*len+1] = '\0';
                    (*len)++;
                }
                else
                {
                    *error = JSON_ERROR_OUTOFMEMORY;
                }
                break;
            }
            case JSON_TYPE_KEY:
            {
                size_t childlen = jsonListLength(node->children);
                size_t i = 0;

                if (childlen > 1)
                {
                    *error = JSON_ERROR_KEYCHILDREN;
                    return;
                }

                *jsonString = _writeJSONStringValue(*jsonString, node->value.s,  len, 1, error);
                if (*error)
                {
                    return;
                }
                (*jsonString)[*len] = ':';
                (*jsonString)[*len+1] = '\0';
                (*len)++;

                for( ;i < childlen; i++)
                {
                    _writeJSON(node->children[i], jsonString, len, error);
                }

                break;
            }
            case JSON_TYPE_STRING:
            {
                *jsonString = _writeJSONStringValue(*jsonString, node->value.s,  len, 0, error);
                if (*error)
                {
                    return;
                }
                (*jsonString)[*len] = '\0';
                break;
            }
            case JSON_TYPE_FLOAT:
            {
                char* fStr = malloc(sizeof(char)*256);

                if(fStr != NULL)
                {
                    memset(fStr, 0, 256);
                    size_t flen = 0;
                    sprintf(fStr, "%g", node->value.f);
                    flen = strlen(fStr);
                    char* tmp = realloc(*jsonString, sizeof(char) * (*len+flen+1));
                    if ( tmp != NULL )
                    {
                        *jsonString = tmp;

                        memcpy(&(*jsonString)[*len], fStr, flen);
                        *len += flen;
                        (*jsonString)[*len] = '\0';
                    }
                    else
                    {
                        *error = JSON_ERROR_OUTOFMEMORY;
                    }
                    free(fStr);

                }
                else
                {
                    *error = JSON_ERROR_OUTOFMEMORY;
                }
                break;
            }
            case JSON_TYPE_INT:
            {
                char* intStr = malloc(sizeof(char)*256);
                if(intStr != NULL)
                {
                    memset(intStr, 0, 256);
                    size_t ilen = 0;
                    sprintf(intStr, "%i", node->value.i);
                    ilen = strlen(intStr);

                    char* tmp = realloc(*jsonString, sizeof(char) * (*len+ilen+1));
                    if ( tmp != NULL )
                    {
                        *jsonString = tmp;
                        memcpy(&(*jsonString)[*len], intStr, ilen);
                        *len += ilen;
                        (*jsonString)[*len] = '\0';
                    }
                    else
                    {
                        *error = JSON_ERROR_OUTOFMEMORY;
                    }
                    free(intStr);

                }
                else
                {
                    *error = JSON_ERROR_OUTOFMEMORY;
                }
                break;
            }
            case JSON_TYPE_BOOL:
            {
                size_t boolLen = node->value.b == 0 ? 5 : 4;
                char* tmp = realloc(*jsonString, sizeof(char) * (*len+boolLen+1));
                if ( tmp != NULL )
                {
                    *jsonString = tmp;
                    memcpy(&(*jsonString)[*len], node->value.b == 0 ? "false" : "true", boolLen);
                    *len += boolLen;
                    (*jsonString)[*len] = '\0';
                }
                else
                {
                    *error = JSON_ERROR_OUTOFMEMORY;
                }
                break;
            }
            default:
                break;
        }
    }
}

char* writeJSON(struct json* node)
{
    char* jsonString = NULL;
    size_t len = 0;
    char error = 0;

    if (node == NULL)
    {
        return NULL;
    }

    _writeJSON(node, &jsonString, &len, &error);

    if(error)
    {
        return NULL;
    }

    return jsonString;
}
