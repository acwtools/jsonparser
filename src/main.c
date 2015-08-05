/*
 * JSON Parser Helper Functions Copyright (C) 2014 by Gandalf Sievers
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

#include <stdlib.h>
#include <stdio.h>
#include "json.h"


int main(int argc, char** argv)
{
    
    const char* js = "{\"int\" : 1, \"float\" : 1.4, \"bool\" : true, \"str\" : \"sad\", \"array\" : [1, \"asdas\", 4.5e-2, 3], \"nested\" : {\"key\" : \"value\"}}";
    
    char error = 0;
    struct json* j = readJSON(js, &error);
    if ( j != NULL )
    {
        char* out = writeJSON(j);

        struct json* fl = jsonGetByKey(j, "float");
        struct json* ar = jsonGetByKey(j, "array");
        struct json* ne = newJSON(JSON_TYPE_INT);
        struct json* nefl = NULL;
        
        printf("%s\n", out);
        free(out);
        
        ne->value.i = 14;
        
        if (fl != NULL)
        {
            printf("%f\n", fl->value.f);
        }
        
        if (ar != NULL)
        {
            ar->children = jsonPushNode(ar->children, ne);
        }
        
        if (jsonRemoveByKey(j, "str"))
        {
            printf("removed str\n");
        }
        
        nefl = newJSON(JSON_TYPE_FLOAT);
        nefl->value.f = 7.3;
        
        jsonAddPair(j, "newfloat", nefl);
        
        out = writeJSON(j);
        printf("%s\n", out);
        free(out);
    
        jsonDeleteTree(j);
    
        
        
    }
    else
    {
        printf("Invalid JSON");
    }
    
    exit(0);
}