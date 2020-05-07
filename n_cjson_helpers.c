// Copyright 2018 Inca Roads LLC.  All rights reserved.
// Use of this source code is governed by licenses granted by the
// copyright holder including that found in the LICENSE file.

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "n_lib.h"

// Return true if a field is present in the response
bool JIsPresent(J *rsp, const char *field) {
	if (rsp == NULL)
		return false;
	return (JGetObjectItem(rsp, field) != NULL);
}

// Return a string from the specified JSON object, or "" if it's not present
char *JGetString(J *rsp, const char *field) {
	if (rsp == NULL)
		return (char *) c_nullstring;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return (char *) c_nullstring;
	if (!JIsString(item))
		return (char *) c_nullstring;
	if (item->valuestring == NULL)
		return (char *) c_nullstring;
	return item->valuestring;
}

// Return an object
J *JGetObject(J *rsp, const char *field) {
	if (rsp == NULL)
		return NULL;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return NULL;
	if (!JIsObject(item))
		return NULL;
	return item;
}

// Return a number from the specified JSON object, or 0 if it's not present
JNUMBER JGetNumber(J *rsp, const char *field) {
	if (rsp == NULL)
		return 0.0;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return 0.0;
	if (!JIsNumber(item))
		return 0.0;
	return item->valuenumber;
}

// Return a number from the JSON object, or 0 if it's not present
int JGetInt(J *rsp, const char *field) {
	if (rsp == NULL)
		return 0.0;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return 0.0;
	if (!JIsNumber(item))
		return 0.0;
	return item->valueint;
}

// Return a bool from the JSON object, or false if it's not present
bool JGetBool(J *rsp, const char *field) {
	if (rsp == NULL)
		return false;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return false;
	if (!JIsBool(item))
		return 0.0;
	return JIsTrue(item);
}

// Return true if the object is valid and if the field is not present or if it's null
bool JIsNullString(J *rsp, const char *field) {
	if (rsp == NULL)
		return false;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return true;
	if (!JIsString(item))
		return false;
	if (item->valuestring == NULL)
		return true;
	if (item->valuestring[0] == '\0')
		return true;
	return false;
}

// Return true if a field exists, and is a string, and is exactly text specified
bool JIsExactString(J *rsp, const char *field, const char *teststr) {
	if (rsp == NULL)
		return false;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return false;
	if (!JIsString(item))
		return false;
	if (item->valuestring == NULL)
		return false;
	if (strlen(teststr) == 0)
		return false;
	return (strcmp(item->valuestring, teststr) == 0);
}

// Return true if a field exists, and is a string, and contains the text as a substring
bool JContainsString(J *rsp, const char *field, const char *substr) {
	if (rsp == NULL)
		return false;
	J *item = JGetObjectItem(rsp, field);
	if (item == NULL)
		return false;
	if (!JIsString(item))
		return false;
	if (item->valuestring == NULL)
		return false;
	if (strlen(substr) == 0)
		return false;
	return (strstr(item->valuestring, substr) != NULL);
}

// Add a binary object as a Base64-encoded string
bool JAddBinaryToObject(J *req, const char *fieldName, const void *binaryData, uint32_t binaryDataLen) {
	unsigned stringDataLen = JB64EncodeLen(binaryDataLen);
	char *stringData = (char *) malloc(stringDataLen);
	if (stringData == NULL)
		return false;
	JB64Encode(stringData, binaryData, binaryDataLen);
	J *stringItem = JCreateStringReference(stringData);
	if (stringItem == NULL) {
		free(stringData);
		return false;
	}
	JAddItemToObject(req, fieldName, stringItem);
	return true;
}
