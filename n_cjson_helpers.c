/*!
 * @file n_cjson_helpers.c
 *
 * Written by Ray Ozzie and Blues Inc. team.
 *
 * Copyright (c) 2019 Blues Inc. MIT License. Use of this source code is
 * governed by licenses granted by the copyright holder including that found in
 * the
 * <a href="https://github.com/blues/note-c/blob/master/LICENSE">LICENSE</a>
 * file.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "n_lib.h"

/*!
 @brief Determine if a field is present in a JSON object.

 @param json The JSON object.
 @param field The field to find.

 @returns True if the field is present, or false if the field is not present or
          the JSON object is NULL.
 */
bool JIsPresent(J *json, const char *field)
{
    if (json == NULL) {
        return false;
    }
    return (JGetObjectItem(json, field) != NULL);
}

/*!
 @brief Get the value of a string field from a JSON object.

 @param json The JSON object.
 @param field The string field to query.

 @returns A pointer to the string if the field exists and is a string, otherwise
          the empty string ("").

 @note The returned string is a pointer to the string contained in the JSON
       object. It is not a copy of the string, so once the JSON object is freed,
       the pointer is no longer valid.
 */
char *JGetString(J *json, const char *field)
{
    if (json == NULL) {
        return (char *) c_nullstring;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return (char *) c_nullstring;
    }
    if (!JIsString(item)) {
        return (char *) c_nullstring;
    }
    if (item->valuestring == NULL) {
        return (char *) c_nullstring;
    }
    return item->valuestring;
}

/*!
 @brief Get the value of an array field from a JSON object.

 @param json The JSON object.
 @param field The array field to query.

 @returns A pointer to the array, which is itself a JSON object (`J *`), if the
          field exists and is an array, otherwise NULL.

 @note The returned JSON object is a pointer to the array contained in the
       parent JSON object. It is not a copy, so once the parent JSON object is
       freed, the pointer is no longer valid.
 */
J *JGetArray(J *json, const char *field)
{
    if (json == NULL) {
        return NULL;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return NULL;
    }
    if (!JIsArray(item)) {
        return NULL;
    }
    return item;
}

/*!
 @brief Get the value of an object field from a JSON object.

 @param json The JSON object.
 @param field The object field to query.

 @returns A pointer to the object, which is itself a JSON object (`J *`), if the
          field exists and is an object, otherwise NULL.

 @note The returned JSON object is a pointer to the object contained in the
       parent JSON object. It is not a copy, so once the parent JSON object is
       freed, the pointer is no longer valid.
 */
J *JGetObject(J *json, const char *field)
{
    if (json == NULL) {
        return NULL;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return NULL;
    }
    if (!JIsObject(item)) {
        return NULL;
    }
    return item;
}

//**************************************************************************/
/*!
    @brief  Return the boolean representation of an item.
    @param   item The JSON item.
    @returns The boolean value.
*/
/**************************************************************************/
bool JBoolValue(J *item)
{
    if (item == NULL) {
        return false;
    }
    return ((item->type & 0xff) == JTrue);
}

//**************************************************************************/
/*!
    @brief  Return the string representation of an item.
    @param   item The JSON item.
    @returns The string value, or empty string, if NULL.
*/
/**************************************************************************/
char *JStringValue(J *item)
{
    if (item == NULL) {
        return (char *)"";
    }
    return item->valuestring;
}

//**************************************************************************/
/*!
    @brief  Return the number representation of an item.
    @param   item The JSON item.
    @returns The number, or 0.0, if NULL.
*/
/**************************************************************************/
JNUMBER JNumberValue(J *item)
{
    if (item == NULL) {
        return 0.0;
    }
    return item->valuenumber;
}

/*!
 @brief Get the floating point value of a number field from a JSON object.

 @param json The JSON object.
 @param field The number field to query.

 @returns The value of the number field if the field exists and is a number,
          otherwise 0.0.

 @note The returned value is the floating point representation of the number.

 @see `JGetInt`
 @see `JGetBool`
 */
JNUMBER JGetNumber(J *json, const char *field)
{
    if (json == NULL) {
        return 0.0;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return 0.0;
    }
    if (!JIsNumber(item)) {
        return 0.0;
    }
    return JNumberValue(item);
}

//**************************************************************************/
/*!
    @brief  Return the integer representation of an item.
    @param   item The JSON item.
    @returns The number, or 0, if NULL.
*/
/**************************************************************************/
JINTEGER JIntValue(J *item)
{
    if (item == NULL) {
        return 0;
    }
    return item->valueint;
}

/*!
 @brief Get the integer value of a number field from a JSON object.

 @param json The JSON object.
 @param field The number field to query.

 @returns The value of the number field if the field exists and is a number,
          otherwise 0.

 @note The returned value is the integer representation of the number.

 @see `JGetBool`
 @see `JGetNumber`
 */
JINTEGER JGetInt(J *json, const char *field)
{
    if (json == NULL) {
        return 0;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return 0;
    }
    if (!JIsNumber(item)) {
        return 0;
    }
    return JIntValue(item);
}

/*!
 @brief Get the value of a boolean field from a JSON object.

 @param json The JSON object.
 @param field The boolean field to query.

 @returns The value of the boolean field if it exists and is a boolean,
          otherwise false.

 @see `JGetInt`
 @see `JGetNumber`
 */
bool JGetBool(J *json, const char *field)
{
    if (json == NULL) {
        return false;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return false;
    }
    if (!JIsBool(item)) {
        return false;
    }
    return JIsTrue(item);
}

//**************************************************************************/
/*!
    @brief  Determine if a JSON object is valid and if a field is not present,
            or null.
    @param   json The JSON response object.
    @param   field The field to return.
    @returns bool. False if the field is not present, or NULL.
*/
/**************************************************************************/
bool JIsNullString(J *json, const char *field)
{
    if (json == NULL) {
        return false;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return true;
    }
    if (!JIsString(item)) {
        return false;
    }
    if (item->valuestring == NULL) {
        return true;
    }
    if (item->valuestring[0] == '\0') {
        return true;
    }
    return false;
}

//**************************************************************************/
/*!
    @brief  Determine if a field exists, is a string and matches a
            provided value.
    @param   json The JSON response object.
    @param   field The field to check.
    @param   teststr The string to test against the returned value.
    @returns bol. Whether the fields match exactly.
*/
/**************************************************************************/
bool JIsExactString(J *json, const char *field, const char *teststr)
{
    if (json == NULL) {
        return false;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return false;
    }
    if (!JIsString(item)) {
        return false;
    }
    if (item->valuestring == NULL) {
        return false;
    }
    if (strlen(teststr) == 0) {
        return false;
    }
    return (strcmp(item->valuestring, teststr) == 0);
}

//**************************************************************************/
/*!
    @brief  Determine if a field exists, is a string and contains a provided
            value.
    @param   json The JSON response object.
    @param   field The field to check.
    @param   substr The string to test against the returned value.
    @returns bol. Whether the provided string is found within the field.
*/
/**************************************************************************/
bool JContainsString(J *json, const char *field, const char *substr)
{
    if (json == NULL) {
        return false;
    }
    J *item = JGetObjectItem(json, field);
    if (item == NULL) {
        return false;
    }
    if (!JIsString(item)) {
        return false;
    }
    if (item->valuestring == NULL) {
        return false;
    }
    if (strlen(substr) == 0) {
        return false;
    }
    return (strstr(item->valuestring, substr) != NULL);
}


/*!
 @brief Add binary data as a Base64-encoded string field to a JSON object.

 @param json The JSON object.
 @param fieldName The name to use for the field.
 @param binaryData A buffer of binary data to encode.
 @param binaryDataLen The length of the binary data in bytes.

 @returns True if the string was successfully encoded and added to the object,
          otherwise false.
 */
bool JAddBinaryToObject(J *json, const char *fieldName, const void *binaryData, uint32_t binaryDataLen)
{
    if (json == NULL) {
        return false;
    }
    unsigned stringDataLen = JB64EncodeLen(binaryDataLen);
    char *stringData = (char *) _Malloc(stringDataLen);
    if (stringData == NULL) {
        return false;
    }
    JB64Encode(stringData, binaryData, binaryDataLen);
    J *stringItem = JCreateStringValue(stringData);
    if (stringItem == NULL) {
        _Free(stringData);
        return false;
    }
    JAddItemToObject(json, fieldName, stringItem);
    return true;
}


/*!
 @brief Decode a Base64-encoded string field in a JSON object and return the
        decoded bytes.

 @param json The JSON object.
 @param fieldName The name of the field.
 @param retBinaryData A pointer to a pointer, used to hold the pointer to the
        decoded bytes.
 @param retBinaryDataLen A pointer to an unsigned integer, used to hold the
        length of the decoded bytes.

 @returns True if the string was successfully decoded and returned, otherwise
          false.

 @note The returned binary buffer must be freed by the user with `JFree` when it
       is no longer needed.

 @note On error, the returned binary buffer and data length shall be set to
       `NULL` and zero (0), respectively.
 */
bool JGetBinaryFromObject(J *json, const char *fieldName, uint8_t **retBinaryData, uint32_t *retBinaryDataLen)
{
    // Initialize the return values to NULL and zero.
    *retBinaryData = NULL;
    *retBinaryDataLen = 0;

    if (json == NULL) {
        return false;
    }

    // In some cases, the caller may already have extracted the string from a different field, in which
    // case "json" will be set to the payload pointer.
    char *payload;
    if (fieldName == NULL) {
        payload = (char *) json;
    } else {
        payload = JGetString(json, fieldName);
    }
    if (payload[0] == '\0') {
        return false;
    }

    // Allocate a buffer for the payload, with an extra 'convenience byte' for null termination.  (see below)
    char *p = (char *) _Malloc(JB64DecodeLen(payload)+1);
    if (p == NULL) {
        return false;
    }
    uint32_t actualLen = JB64Decode(p, payload);

    // As a convenience to the caller, null-terminate the returned buffer in case it's a string.
    // (If we didn't do this, the caller would be forced to alloc another buffer of length+1 and
    // copy it to add the null terminator, while it's easy for us to do it here.)
    p[actualLen] = '\0';

    // Return the binary to the caller
    *retBinaryData = (uint8_t *)p;
    *retBinaryDataLen = actualLen;
    return true;

}

//**************************************************************************/
/*!
    @brief  Get the object name.
    @param   item The JSON object.
    @returns The name, or the empty string, if NULL.
*/
/**************************************************************************/
const char *JGetItemName(const J * item)
{
    if (item == NULL || item->string == NULL) {
        return "";
    }
    return item->string;
}

//**************************************************************************/
/*!
    @brief  Convert an integer to text.
    @param   n The integer to convert.
    @param   s The buffer to hold the text.
    @note The buffer must be large enough because no bounds checking is done.
*/
/**************************************************************************/
void JItoA(JINTEGER n, char *s)
{
    char c;
    // Conversion to unsigned is required to handle the case where n is
    // JINTEGER_MIN. In that case, applying the unary minus operator to the
    // signed version of n overflows and the behavior is undefined. By changing
    // n to be unsigned, the unary minus operator behaves differently, and there
    // is no overflow. See https://stackoverflow.com/q/8026694.
    JUINTEGER unsignedN = n;
    long int i, j;
    if (n < 0) {
        unsignedN = -unsignedN;
    }
    i = 0;
    do {
        s[i++] = unsignedN % 10 + '0';
    } while ((unsignedN /= 10) > 0);
    if (n < 0) {
        s[i++] = '-';
    }
    s[i] = '\0';
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

//**************************************************************************/
/*!
    @brief  Convert text to an integer
    @param   string A null-terminated text buffer
    @returns An integer, or 0 if invalid
*/
/**************************************************************************/
JINTEGER JAtoI(const char *string)
{
    JINTEGER result = 0;
    unsigned int digit;
    int sign;
    while (*string == ' ') {
        string += 1;
    }
    if (*string == '-') {
        sign = 1;
        string += 1;
    } else {
        sign = 0;
        if (*string == '+') {
            string += 1;
        }
    }
    for ( ; ; string += 1) {
        digit = *string - '0';
        if (digit > 9) {
            break;
        }
        result = (10*result) + digit;
    }
    if (sign) {
        result = -result;
    }
    return result;
}

//**************************************************************************/
/*!
    @brief  Convert a buffer/len to a null-terminated c-string
    @param   buffer A buffer containing the bytes to convert.
    @param   len The length of buffer in bytes.
    @returns If buffer is NULL or length 0, the empty string. If allocation
             fails, NULL. On success, the converted c-string. The returned
             string must be freed with NoteFree.
*/
/**************************************************************************/
char *JAllocString(uint8_t *buffer, uint32_t len)
{
    char *buf = _Malloc(len+1);
    if (buf == NULL) {
        return NULL;
    }
    if (len > 0) {
        memcpy(buf, buffer, len);
    }
    buf[len] = '\0';
    return buf;
}

//**************************************************************************/
/*!
    @brief  Return the type of an item, as a string
    @param   item The JSON item.
    @returns The type
*/
/**************************************************************************/
const char *JType(J *item)
{
    if (item == NULL) {
        return "";
    }
    switch (item->type & 0xff) {
    case JTrue:
    case JFalse:
        return "bool";
    case JNULL:
        return "null";
    case JNumber:
        return "number";
    case JRaw:
    case JString:
        return "string";
    case JObject:
        return "object";
    case JArray:
        return "array";
    }
    return "invalid";
}

//**************************************************************************/
/*!
    @brief  Get the type of a field, as an int usable in a switch statement.
    @param   json The JSON object containing the field.
    @param   field The field's name.
    @returns The type of the field on success. JTYPE_NOT_PRESENT on error or if
             the field doesn't exist.
*/
/**************************************************************************/
int JGetType(J *json, const char *field)
{
    if (json == NULL || field == NULL) {
        return JTYPE_NOT_PRESENT;
    }
    return JGetItemType(JGetObjectItem(json, field));
}

// Get the
int JGetItemType(J *item)
{
    const char *v;
    if (item == NULL) {
        return JTYPE_NOT_PRESENT;
    }
    switch (item->type & 0xff) {
    case JTrue:
        return JTYPE_BOOL_TRUE;
    case JFalse:
        return JTYPE_BOOL_FALSE;
    case JNULL:
        return JTYPE_NULL;
    case JNumber:
        if (item->valueint == 0 && item->valuenumber == 0) {
            return JTYPE_NUMBER_ZERO;
        }
        return JTYPE_NUMBER;
    case JRaw:
    case JString: {
        v = item->valuestring;
        if (v == NULL || v[0] == 0) {
            return JTYPE_STRING_BLANK;
        }
        int vlen = strlen(v);
        char *endstr;
        JNUMBER value = JAtoN(v, &endstr);
        if (endstr[0] == 0) {
            if (value == 0) {
                return JTYPE_STRING_ZERO;
            }
            return JTYPE_STRING_NUMBER;
        }
        if (vlen == 4 && (
                    (v[0] == 't' || v[0] == 'T')
                    && (v[1] == 'r' || v[1] == 'R')
                    && (v[2] == 'u' || v[2] == 'U')
                    && (v[3] == 'e' || v[3] == 'E'))) {
            return JTYPE_STRING_BOOL_TRUE;
        }
        if (vlen == 5 && (
                    (v[0] == 'f' || v[0] == 'F')
                    && (v[1] == 'a' || v[1] == 'A')
                    && (v[2] == 'l' || v[2] == 'L')
                    && (v[3] == 's' || v[3] == 'S')
                    && (v[4] == 'e' || v[4] == 'E'))) {
            return JTYPE_STRING_BOOL_FALSE;
        }
        return JTYPE_STRING;
    }
    case JObject:
        return JTYPE_OBJECT;
    case JArray:
        return JTYPE_ARRAY;
    }
    return JTYPE_NOT_PRESENT;
}

// Coalesce to the base types
int JBaseItemType(int type)
{
    switch (type) {
    case JTYPE_BOOL_TRUE:
        return JTYPE_BOOL;
    case JTYPE_BOOL_FALSE:
        return JTYPE_BOOL;
    case JTYPE_NUMBER_ZERO:
        return JTYPE_NUMBER;
    case JTYPE_STRING_BLANK:
        return JTYPE_STRING;
    case JTYPE_STRING_ZERO:
        return JTYPE_STRING;
    case JTYPE_STRING_NUMBER:
        return JTYPE_STRING;
    case JTYPE_STRING_BOOL_TRUE:
        return JTYPE_STRING;
    case JTYPE_STRING_BOOL_FALSE:
        return JTYPE_STRING;
    }
    return type;
}
