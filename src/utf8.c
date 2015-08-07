/*
 * JSON Parser UTF8 Helper Copyright (C) 2014 by Gandalf Sievers
 * Created by Gandalf Sievers on 05.07.15.
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

#include "utf8.h"

#include <stdlib.h>
#include <strings.h>

unsigned char hexDigitToValue( char d )
{
    char o = 0;
    if ( d > 47 && d < 58 )
    {
        o = d - 48;
    }
    else if ( d > 64 && d < 71 )
    {
        o = d - 55;
    }
    else if ( d > 96 && d < 103 )
    {
        o = d - 87;
    }
    return o;
}

char* parseUTF8char( const char* str, size_t* len )
{
    unsigned char plane[2] = { 0, 0 };
    unsigned char utf8Value[3] = { 0, 0, 0 };
    char* utf8char = NULL;

    *len = 0;

    plane[0] = hexDigitToValue( str[0] ) * 16 + hexDigitToValue( str[1] );
    plane[1] = hexDigitToValue( str[2] ) * 16 + hexDigitToValue( str[3] );

    if(plane[1] > 127 || plane[0] != 0)
    {
        utf8Value[2] = plane[1] & 63;
        utf8Value[2] |= 128;

        utf8Value[1] = plane[1] >> 6;

        if(plane[0] != 0)
        {
            unsigned char tmp = (plane[0] << 2) & 63;
            utf8Value[1] |= tmp;
            utf8Value[1] |= 128;

            if(plane[0] > 15)
            {
                utf8Value[0] = plane[0] >> 4;
                utf8Value[0] |= 224;
            }
            else
            {
                utf8Value[1] |= 64;
            }
        }
        else
        {
            utf8Value[1] |= 192;
        }
    }
    else
    {
        utf8Value[2] = plane[1];
    }

    *len =  (utf8Value[0] != 0) + (utf8Value[1] != 0) + (utf8Value[2] != 0);
    if(*len == 0)
    {
        return NULL;
    }

    utf8char = malloc(sizeof(char) * *len);

    if( utf8Value[0] != 0)
    {
        utf8char[0] = utf8Value[0];
        utf8char[1] = utf8Value[1];
        utf8char[2] = utf8Value[2];
    }
    else if( utf8Value[1] != 0)
    {
        utf8char[0] = utf8Value[1];
        utf8char[1] = utf8Value[2];
    }
    else
    {
        utf8char[0] = utf8Value[2];
    }
    return utf8char;
}

char valueToHexDigit( unsigned char d )
{
    if( d >= 0 && d < 10 )
    {
        return d + 48;
    }
    else if ( d > 9 && d < 16 )
    {
        return d + 55;
    }
    return 0;
}

char* createFixedStr( const char* in, size_t len )
{
    char* str = malloc( sizeof( char ) * len );
    memcpy( str, in, len );
    return str;
}

char* lowerCharToUnicode( unsigned char c, size_t* len )
{
    char* out = NULL;
    unsigned char higherDigit = (unsigned char) c / 16;
    unsigned char lowerDigit = c - (higherDigit * 16);
    *len = 6;

    out = malloc(sizeof(char)*6);
    out[0] = '\\';
    out[1] = 'u';
    out[2] = '0';
    out[3] = '0';

    out[4] = valueToHexDigit( higherDigit );
    out[5] = valueToHexDigit( lowerDigit );

    return out;
}

char* encodeControlChar( char c, size_t* len )
{
    if(c == '"')
    {
        *len = 2;
        return createFixedStr( "\\\"", 2 );
    }
    else if(c == '\\')
    {
        *len = 2;
        return createFixedStr( "\\\\", 2 );
    }
    else if(c == '/')
    {
        *len = 2;
        return createFixedStr( "\\/", 2 );
    }
    else if(c == '\b')
    {
        *len = 2;
        return createFixedStr( "\\b", 2 );
    }
    else if(c == '\f')
    {
        *len = 2;
        return createFixedStr( "\\f", 2 );
    }
    else if(c == '\n')
    {
        *len = 2;
        return createFixedStr( "\\n", 2 );
    }
    else if(c == '\r')
    {
        *len = 2;
        return createFixedStr( "\\r", 2 );
    }
    else if(c == '\t')
    {
        *len = 2;
        return createFixedStr( "\\t", 2 );
    }
    else
    {
        return lowerCharToUnicode( c, len );
    }
}

char* highCharToUnicode( const char* str, size_t inlen, size_t* outlen )
{
    char* out = NULL;
    unsigned char higherDigit = 0;
    unsigned char lowerDigit = 0;
    unsigned char plane[2] = { 0, 0 };


    if( inlen == 3 )
    {
        plane[0] = str[0] << 4;
        plane[0] |= ((str[1] >> 2 ) & 15);
        plane[1] = str[1] << 6;
        plane[1] |= (str[2] & 63);
    }
    else if ( inlen == 2 )
    {
        plane[0] |= ((str[0] >> 2 ) & 15);
        plane[1] = str[0] << 6;
        plane[1] |= (str[1] & 63);
    }
    else
    {
        return NULL;
    }
    *outlen = 6;

    out = malloc(sizeof(char)*6);
    if( out == NULL )
    {
        return NULL;
    }

    out[0] = '\\';
    out[1] = 'u';
    higherDigit = (unsigned char) plane[0] / 16;
    lowerDigit = plane[0] - (higherDigit * 16);

    out[2] = valueToHexDigit( higherDigit );
    out[3] = valueToHexDigit( lowerDigit );

    higherDigit = (unsigned char) plane[1] / 16;
    lowerDigit = plane[1] - (higherDigit * 16);
    out[4] = valueToHexDigit( higherDigit );
    out[5] = valueToHexDigit( lowerDigit );

    return out;
}

char* insertSubString( char* str, size_t* len, char* sub, size_t sublen, size_t pos )
{
    char* tmp = realloc(str, sizeof(char)*( *len + sublen + 1 ) );
    if( tmp != NULL )
    {
        str = tmp;
        memmove( str + pos + sublen , str + pos, *len - pos );
        memcpy(  &str[pos], sub, sublen );
        *len += sublen;
    }
    return str;
}

char* encodeUTF8str( const char* str )
{
    char* out = NULL;
    size_t len = strlen( str );
    size_t i = 0, o = 0;

    out = malloc( sizeof( char ) * ( len + 1 ) );
    memset( out, 0, len + 1 );

    while( str[i] != '\0' )
    {
        unsigned char s = (unsigned char) str[i];
        if( s < 32 || str[i] == '"' || str[i] == '\\' || str[i] == '/' )
        {
            size_t sublen = 0;
            char* substr = encodeControlChar( str[i], &sublen );
            out = insertSubString( out, &len, substr, sublen, o );
            free( substr );
            o += sublen;
            i++;
        }
        else if ( s & 128 )
        {
            char* substr = NULL;
            size_t sublen = 0;
            size_t ulen = 0;

            if( (s & 128) && !(s & 64) )
            {
                ulen = 1;
            }
            else if ( (s & 128) && (s & 64) && !(s & 32) )
            {
                ulen = 2;
            }
            else if ( (s & 128) && (s & 64) && (s & 32) && !(s & 16) )
            {
                ulen = 3;
            }
            else if ( (s & 128) && (s & 64) && (s & 32) && (s & 16) && !(s & 8) )
            {
                ulen = 4;
            }

            if( ulen == 1 || ulen == 4 )
            {
                i += ulen;
                continue;
            }
            substr = highCharToUnicode( &str[i], ulen, &sublen );
            out = insertSubString( out, &len, substr, sublen, o );
            free( substr );
            o += sublen;
            i += ulen;
        }
        else
        {
            out[o] = str[i];
            o++;
            i++;
        }

    }

    return out;
}