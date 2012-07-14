//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <glf/io/config.hpp>
#include <glf/utils.hpp>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cfloat>
#include <cassert>
#include <fstream>

namespace 
{
	//--------------------------------------------------------------------------
	//	Copyright (c) 2009 Dave Gamble
	//
	//	Permission is hereby granted, free of charge, to any person obtaining a copy
	//	of this software and associated documentation files (the "Software"), to deal
	//	in the Software without restriction, including without limitation the rights
	//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	//	copies of the Software, and to permit persons to whom the Software is
	//	furnished to do so, subject to the following conditions:
	//
	//	The above copyright notice and this permission notice shall be included in
	//	all copies or substantial portions of the Software.
	//
	//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	//	THE SOFTWARE.
	//--------------------------------------------------------------------------

	/* cJSON Types: */
	#define cJSON_False 0
	#define cJSON_True 1
	#define cJSON_NULL 2
	#define cJSON_Number 3
	#define cJSON_String 4
	#define cJSON_Array 5
	#define cJSON_Object 6
	#define cJSON_IsReference 256

	/* The cJSON structure: */
//	typedef struct cJSON 
//	{
//		struct cJSON *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
//		struct cJSON *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

//		int type;					/* The type of the item, as above. */

//		char *valuestring;			/* The item's string, if type==cJSON_String */
//		int valueint;				/* The item's number, if type==cJSON_Number */
//		double valuedouble;			/* The item's number, if type==cJSON_Number */

//		char *string;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
//	} cJSON;
	typedef glf::io::ConfigNode cJSON;

	typedef struct cJSON_Hooks 
	{
		  void *(*malloc_fn)(size_t sz);
		  void (*free_fn)(void *ptr);
	} cJSON_Hooks;

	/* Supply malloc, realloc and free functions to cJSON */
	extern void cJSON_InitHooks(cJSON_Hooks* hooks);


	/* Supply a block of JSON, and this returns a cJSON object you can interrogate. Call cJSON_Delete when finished. */
	extern cJSON *cJSON_Parse(const char *value);
	/* Render a cJSON entity to text for transfer/storage. Free the char* when finished. */
	extern char  *cJSON_Print(cJSON *item);
	/* Render a cJSON entity to text for transfer/storage without any formatting. Free the char* when finished. */
	extern char  *cJSON_PrintUnformatted(cJSON *item);
	/* Delete a cJSON entity and all subentities. */
	extern void   cJSON_Delete(cJSON *c);

	/* Returns the number of items in an array (or object). */
	extern int	  cJSON_GetArraySize(cJSON *array);
	/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
	extern cJSON *cJSON_GetArrayItem(cJSON *array,int item);
	/* Get item "string" from object. Case insensitive. */
	extern cJSON *cJSON_GetObjectItem(cJSON *object,const char *string);

	/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when cJSON_Parse() returns 0. 0 when cJSON_Parse() succeeds. */
	extern const char *cJSON_GetErrorPtr();

	/* These calls create a cJSON item of the appropriate type. */
	extern cJSON *cJSON_CreateNull();
	extern cJSON *cJSON_CreateTrue();
	extern cJSON *cJSON_CreateFalse();
	extern cJSON *cJSON_CreateBool(int b);
	extern cJSON *cJSON_CreateNumber(double num);
	extern cJSON *cJSON_CreateString(const char *string);
	extern cJSON *cJSON_CreateArray();
	extern cJSON *cJSON_CreateObject();

	/* These utilities create an Array of count items. */
	extern cJSON *cJSON_CreateIntArray(int *numbers,int count);
	extern cJSON *cJSON_CreateFloatArray(float *numbers,int count);
	extern cJSON *cJSON_CreateDoubleArray(double *numbers,int count);
	extern cJSON *cJSON_CreateStringArray(const char **strings,int count);

	/* Append item to the specified array/object. */
	extern void cJSON_AddItemToArray(cJSON *array, cJSON *item);
	extern void	cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item);
	/* Append reference to item to the specified array/object. Use this when you want to add an existing cJSON to a new cJSON, but don't want to corrupt your existing cJSON. */
	extern void cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item);
	extern void	cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item);

	/* Remove/Detatch items from Arrays/Objects. */
	extern cJSON *cJSON_DetachItemFromArray(cJSON *array,int which);
	extern void   cJSON_DeleteItemFromArray(cJSON *array,int which);
	extern cJSON *cJSON_DetachItemFromObject(cJSON *object,const char *string);
	extern void   cJSON_DeleteItemFromObject(cJSON *object,const char *string);

	/* Update array items. */
	extern void cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem);
	extern void cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem);

	#define cJSON_AddNullToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateNull())
	#define cJSON_AddTrueToObject(object,name)	cJSON_AddItemToObject(object, name, cJSON_CreateTrue())
	#define cJSON_AddFalseToObject(object,name)		cJSON_AddItemToObject(object, name, cJSON_CreateFalse())
	#define cJSON_AddNumberToObject(object,name,n)	cJSON_AddItemToObject(object, name, cJSON_CreateNumber(n))
	#define cJSON_AddStringToObject(object,name,s)	cJSON_AddItemToObject(object, name, cJSON_CreateString(s))

	static const char *ep;

	const char *cJSON_GetErrorPtr() {return ep;}

	static int cJSON_strcasecmp(const char *s1,const char *s2)
	{
		if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
		for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
		return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
	}

	static void *(*cJSON_malloc)(size_t sz) = malloc;
	static void (*cJSON_free)(void *ptr) = free;

	static char* cJSON_strdup(const char* str)
	{
		  size_t len;
		  char* copy;

		  len = strlen(str) + 1;
		  if (!(copy = (char*)cJSON_malloc(len))) return 0;
		  memcpy(copy,str,len);
		  return copy;
	}

	void cJSON_InitHooks(cJSON_Hooks* hooks)
	{
		if (!hooks) { /* Reset hooks */
		    cJSON_malloc = malloc;
		    cJSON_free = free;
		    return;
		}

		cJSON_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
		cJSON_free	 = (hooks->free_fn)?hooks->free_fn:free;
	}

	/* Internal constructor. */
	static cJSON *cJSON_New_Item()
	{
		cJSON* node = (cJSON*)cJSON_malloc(sizeof(cJSON));
		if (node) memset(node,0,sizeof(cJSON));
		return node;
	}

	/* Delete a cJSON structure. */
	void cJSON_Delete(cJSON *c)
	{
		cJSON *next;
		while (c)
		{
			next=c->next;
			if (!(c->type&cJSON_IsReference) && c->child) cJSON_Delete(c->child);
			if (!(c->type&cJSON_IsReference) && c->valuestring) cJSON_free(c->valuestring);
			if (c->string) cJSON_free(c->string);
			cJSON_free(c);
			c=next;
		}
	}

	/* Parse the input text to generate a number, and populate the result into item. */
	static const char *parse_number(cJSON *item,const char *num)
	{
		double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

		/* Could use sscanf for this? */
		if (*num=='-') sign=-1,num++;	/* Has sign? */
		if (*num=='0') num++;			/* is zero */
		if (*num>='1' && *num<='9')	do	n=(n*10.0)+(*num++ -'0');	while (*num>='0' && *num<='9');	/* Number? */
		if (*num=='.') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */
		if (*num=='e' || *num=='E')		/* Exponent? */
		{	num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
			while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
		}

		n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
		item->valuedouble=n;
		item->valueint=(int)n;
		item->type=cJSON_Number;
		return num;
	}

	/* Render the number nicely from the given item into a string. */
	static char *print_number(cJSON *item)
	{
		char *str;
		double d=item->valuedouble;
		if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
		{
			str=(char*)cJSON_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
			if (str) sprintf(str,"%d",item->valueint);
		}
		else
		{
			str=(char*)cJSON_malloc(64);	/* This is a nice tradeoff. */
			if (str)
			{
				if (fabs(floor(d)-d)<=DBL_EPSILON)			sprintf(str,"%.0f",d);
				else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)	sprintf(str,"%e",d);
				else										sprintf(str,"%f",d);
			}
		}
		return str;
	}

	/* Parse the input text into an unescaped cstring, and populate item. */
	static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
	static const char *parse_string(cJSON *item,const char *str)
	{
		const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc;
		if (*str!='\"') {ep=str;return 0;}	/* not a string! */
	
		while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */
	
		out=(char*)cJSON_malloc(len+1);	/* This is how long we need for the string, roughly. */
		if (!out) return 0;
	
		ptr=str+1;ptr2=out;
		while (*ptr!='\"' && *ptr)
		{
			if (*ptr!='\\') *ptr2++=*ptr++;
			else
			{
				ptr++;
				switch (*ptr)
				{
					case 'b': *ptr2++='\b';	break;
					case 'f': *ptr2++='\f';	break;
					case 'n': *ptr2++='\n';	break;
					case 'r': *ptr2++='\r';	break;
					case 't': *ptr2++='\t';	break;
					case 'u':	 /* transcode utf16 to utf8. DOES NOT SUPPORT SURROGATE PAIRS CORRECTLY. */
						sscanf(ptr+1,"%4x",&uc);	/* get the unicode char. */
						len=3;if (uc<0x80) len=1;else if (uc<0x800) len=2;ptr2+=len;
					
						switch (len) {
							case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
							case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
							case 1: *--ptr2 =(uc | firstByteMark[len]);
						}
						ptr2+=len;ptr+=4;
						break;
					default:  *ptr2++=*ptr; break;
				}
				ptr++;
			}
		}
		*ptr2=0;
		if (*ptr=='\"') ptr++;
		item->valuestring=out;
		item->type=cJSON_String;
		return ptr;
	}

	/* Render the cstring provided to an escaped version that can be printed. */
	static char *print_string_ptr(const char *str)
	{
		const char *ptr;char *ptr2,*out;int len=0;unsigned char token;
	
		if (!str) return cJSON_strdup("");
		ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}
	
		out=(char*)cJSON_malloc(len+3);
		if (!out) return 0;

		ptr2=out;ptr=str;
		*ptr2++='\"';
		while (*ptr)
		{
			if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
			else
			{
				*ptr2++='\\';
				switch (token=*ptr++)
				{
					case '\\':	*ptr2++='\\';	break;
					case '\"':	*ptr2++='\"';	break;
					case '\b':	*ptr2++='b';	break;
					case '\f':	*ptr2++='f';	break;
					case '\n':	*ptr2++='n';	break;
					case '\r':	*ptr2++='r';	break;
					case '\t':	*ptr2++='t';	break;
					default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
				}
			}
		}
		*ptr2++='\"';*ptr2++=0;
		return out;
	}
	/* Invote print_string_ptr (which is useful) on an item. */
	static char *print_string(cJSON *item)	{return print_string_ptr(item->valuestring);}

	/* Predeclare these prototypes. */
	static const char *parse_value(cJSON *item,const char *value);
	static char *print_value(cJSON *item,int depth,int fmt);
	static const char *parse_array(cJSON *item,const char *value);
	static char *print_array(cJSON *item,int depth,int fmt);
	static const char *parse_object(cJSON *item,const char *value);
	static char *print_object(cJSON *item,int depth,int fmt);

	/* Utility to jump whitespace and cr/lf */
	static const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

	/* Parse an object - create a new root, and populate. */
	cJSON *cJSON_Parse(const char *value)
	{
		cJSON *c=cJSON_New_Item();
		ep=0;
		if (!c) return 0;       /* memory fail */

		if (!parse_value(c,skip(value))) {cJSON_Delete(c);return 0;}
		return c;
	}

	/* Render a cJSON item/entity/structure to text. */
	char *cJSON_Print(cJSON *item)				{return print_value(item,0,1);}
	char *cJSON_PrintUnformatted(cJSON *item)	{return print_value(item,0,0);}

	/* Parser core - when encountering text, process appropriately. */
	static const char *parse_value(cJSON *item,const char *value)
	{
		if (!value)						return 0;	/* Fail on null. */
		if (!strncmp(value,"null",4))	{ item->type=cJSON_NULL;  return value+4; }
		if (!strncmp(value,"false",5))	{ item->type=cJSON_False; return value+5; }
		if (!strncmp(value,"true",4))	{ item->type=cJSON_True; item->valueint=1;	return value+4; }
		if (*value=='\"')				{ return parse_string(item,value); }
		if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
		if (*value=='[')				{ return parse_array(item,value); }
		if (*value=='{')				{ return parse_object(item,value); }

		ep=value;return 0;	/* failure. */
	}

	/* Render a value to text. */
	static char *print_value(cJSON *item,int depth,int fmt)
	{
		char *out=0;
		if (!item) return 0;
		switch ((item->type)&255)
		{
			case cJSON_NULL:	out=cJSON_strdup("null");	break;
			case cJSON_False:	out=cJSON_strdup("false");break;
			case cJSON_True:	out=cJSON_strdup("true"); break;
			case cJSON_Number:	out=print_number(item);break;
			case cJSON_String:	out=print_string(item);break;
			case cJSON_Array:	out=print_array(item,depth,fmt);break;
			case cJSON_Object:	out=print_object(item,depth,fmt);break;
		}
		return out;
	}

	/* Build an array from input text. */
	static const char *parse_array(cJSON *item,const char *value)
	{
		cJSON *child;
		if (*value!='[')	{ep=value;return 0;}	/* not an array! */

		item->type=cJSON_Array;
		value=skip(value+1);
		if (*value==']') return value+1;	/* empty array. */

		item->child=child=cJSON_New_Item();
		if (!item->child) return 0;		 /* memory fail */
		value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
		if (!value) return 0;

		while (*value==',')
		{
			cJSON *new_item;
			if (!(new_item=cJSON_New_Item())) return 0; 	/* memory fail */
			child->next=new_item;new_item->prev=child;child=new_item;
			value=skip(parse_value(child,skip(value+1)));
			if (!value) return 0;	/* memory fail */
		}

		if (*value==']') return value+1;	/* end of array */
		ep=value;return 0;	/* malformed. */
	}

	/* Render an array to text */
	static char *print_array(cJSON *item,int depth,int fmt)
	{
		char **entries;
		char *out=0,*ptr,*ret;int len=5;
		cJSON *child=item->child;
		int numentries=0,i=0,fail=0;
	
		/* How many entries in the array? */
		while (child) numentries++,child=child->next;

		/* Allocate an array to hold the values for each */
		entries=(char**)cJSON_malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		memset(entries,0,numentries*sizeof(char*));
		/* Retrieve all the results: */
		child=item->child;
		while (child && !fail)
		{
			ret=print_value(child,depth+1,fmt);
			entries[i++]=ret;
			if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
			child=child->next;
		}
	
		/* If we didn't fail, try to malloc the output string */
		if (!fail) out=(char*)cJSON_malloc(len);
		/* If that fails, we fail. */
		if (!out) fail=1;

		/* Handle failure. */
		if (fail)
		{
			for (i=0;i<numentries;i++) if (entries[i]) cJSON_free(entries[i]);
			cJSON_free(entries);
			return 0;
		}
	
		/* Compose the output array. */
		*out='[';
		ptr=out+1;*ptr=0;
		for (i=0;i<numentries;i++)
		{
			strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
			if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
			cJSON_free(entries[i]);
		}
		cJSON_free(entries);
		*ptr++=']';*ptr++=0;
		return out;	
	}

	/* Build an object from the text. */
	static const char *parse_object(cJSON *item,const char *value)
	{
		cJSON *child;
		if (*value!='{')	{ep=value;return 0;}	/* not an object! */
	
		item->type=cJSON_Object;
		value=skip(value+1);
		if (*value=='}') return value+1;	/* empty array. */
	
		item->child=child=cJSON_New_Item();
		if (!item->child) return 0;
		value=skip(parse_string(child,skip(value)));
		if (!value) return 0;
		child->string=child->valuestring;child->valuestring=0;
		if (*value!=':') {ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) return 0;
	
		while (*value==',')
		{
			cJSON *new_item;
			if (!(new_item=cJSON_New_Item()))	return 0; /* memory fail */
			child->next=new_item;new_item->prev=child;child=new_item;
			value=skip(parse_string(child,skip(value+1)));
			if (!value) return 0;
			child->string=child->valuestring;child->valuestring=0;
			if (*value!=':') {ep=value;return 0;}	/* fail! */
			value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
			if (!value) return 0;
		}
	
		if (*value=='}') return value+1;	/* end of array */
		ep=value;return 0;	/* malformed. */
	}

	/* Render an object to text. */
	static char *print_object(cJSON *item,int depth,int fmt)
	{
		char **entries=0,**names=0;
		char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
		cJSON *child=item->child;
		int numentries=0,fail=0;
		/* Count the number of entries. */
		while (child) numentries++,child=child->next;
		/* Allocate space for the names and the objects */
		entries=(char**)cJSON_malloc(numentries*sizeof(char*));
		if (!entries) return 0;
		names=(char**)cJSON_malloc(numentries*sizeof(char*));
		if (!names) {cJSON_free(entries);return 0;}
		memset(entries,0,sizeof(char*)*numentries);
		memset(names,0,sizeof(char*)*numentries);

		/* Collect all the results into our arrays: */
		child=item->child;depth++;if (fmt) len+=depth;
		while (child)
		{
			names[i]=str=print_string_ptr(child->string);
			entries[i++]=ret=print_value(child,depth,fmt);
			if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
			child=child->next;
		}
	
		/* Try to allocate the output string */
		if (!fail) out=(char*)cJSON_malloc(len);
		if (!out) fail=1;

		/* Handle failure */
		if (fail)
		{
			for (i=0;i<numentries;i++) {if (names[i]) cJSON_free(names[i]);if (entries[i]) cJSON_free(entries[i]);}
			cJSON_free(names);cJSON_free(entries);
			return 0;
		}
	
		/* Compose the output: */
		*out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
		for (i=0;i<numentries;i++)
		{
			if (fmt) for (j=0;j<depth;j++) *ptr++='\t';
			strcpy(ptr,names[i]);ptr+=strlen(names[i]);
			*ptr++=':';if (fmt) *ptr++='\t';
			strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
			if (i!=numentries-1) *ptr++=',';
			if (fmt) *ptr++='\n';*ptr=0;
			cJSON_free(names[i]);cJSON_free(entries[i]);
		}
	
		cJSON_free(names);cJSON_free(entries);
		if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
		*ptr++='}';*ptr++=0;
		return out;	
	}

	/* Get Array size/item / object item. */
	int    cJSON_GetArraySize(cJSON *array)							{cJSON *c=array->child;int i=0;while(c)i++,c=c->next;return i;}
	cJSON *cJSON_GetArrayItem(cJSON *array,int item)				{cJSON *c=array->child;  while (c && item>0) item--,c=c->next; return c;}
	cJSON *cJSON_GetObjectItem(cJSON *object,const char *string)	{cJSON *c=object->child; while (c && cJSON_strcasecmp(c->string,string)) c=c->next; return c;}

	/* Utility for array list handling. */
	static void suffix_object(cJSON *prev,cJSON *item) {prev->next=item;item->prev=prev;}
	/* Utility for handling references. */
	static cJSON *create_reference(cJSON *item) {cJSON *ref=cJSON_New_Item();if (!ref) return 0;memcpy(ref,item,sizeof(cJSON));ref->string=0;ref->type|=cJSON_IsReference;ref->next=ref->prev=0;return ref;}

	/* Add item to array/object. */
	void   cJSON_AddItemToArray(cJSON *array, cJSON *item)						{cJSON *c=array->child;if (!item) return; if (!c) {array->child=item;} else {while (c && c->next) c=c->next; suffix_object(c,item);}}
	void   cJSON_AddItemToObject(cJSON *object,const char *string,cJSON *item)	{if (!item) return; if (item->string) cJSON_free(item->string);item->string=cJSON_strdup(string);cJSON_AddItemToArray(object,item);}
	void	cJSON_AddItemReferenceToArray(cJSON *array, cJSON *item)						{cJSON_AddItemToArray(array,create_reference(item));}
	void	cJSON_AddItemReferenceToObject(cJSON *object,const char *string,cJSON *item)	{cJSON_AddItemToObject(object,string,create_reference(item));}

	cJSON *cJSON_DetachItemFromArray(cJSON *array,int which)			{cJSON *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return 0;
		if (c->prev) c->prev->next=c->next;if (c->next) c->next->prev=c->prev;if (c==array->child) array->child=c->next;c->prev=c->next=0;return c;}
	void   cJSON_DeleteItemFromArray(cJSON *array,int which)			{cJSON_Delete(cJSON_DetachItemFromArray(array,which));}
	cJSON *cJSON_DetachItemFromObject(cJSON *object,const char *string) {int i=0;cJSON *c=object->child;while (c && cJSON_strcasecmp(c->string,string)) i++,c=c->next;if (c) return cJSON_DetachItemFromArray(object,i);return 0;}
	void   cJSON_DeleteItemFromObject(cJSON *object,const char *string) {cJSON_Delete(cJSON_DetachItemFromObject(object,string));}

	/* Replace array/object items with new ones. */
	void   cJSON_ReplaceItemInArray(cJSON *array,int which,cJSON *newitem)		{cJSON *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return;
		newitem->next=c->next;newitem->prev=c->prev;if (newitem->next) newitem->next->prev=newitem;
		if (c==array->child) array->child=newitem; else newitem->prev->next=newitem;c->next=c->prev=0;cJSON_Delete(c);}
	void   cJSON_ReplaceItemInObject(cJSON *object,const char *string,cJSON *newitem){int i=0;cJSON *c=object->child;while(c && cJSON_strcasecmp(c->string,string))i++,c=c->next;if(c){newitem->string=cJSON_strdup(string);cJSON_ReplaceItemInArray(object,i,newitem);}}

	/* Create basic types: */
	cJSON *cJSON_CreateNull()						{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_NULL;return item;}
	cJSON *cJSON_CreateTrue()						{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_True;return item;}
	cJSON *cJSON_CreateFalse()						{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_False;return item;}
	cJSON *cJSON_CreateBool(int b)					{cJSON *item=cJSON_New_Item();if(item)item->type=b?cJSON_True:cJSON_False;return item;}
	cJSON *cJSON_CreateNumber(double num)			{cJSON *item=cJSON_New_Item();if(item){item->type=cJSON_Number;item->valuedouble=num;item->valueint=(int)num;}return item;}
	cJSON *cJSON_CreateString(const char *string)	{cJSON *item=cJSON_New_Item();if(item){item->type=cJSON_String;item->valuestring=cJSON_strdup(string);}return item;}
	cJSON *cJSON_CreateArray()						{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_Array;return item;}
	cJSON *cJSON_CreateObject()						{cJSON *item=cJSON_New_Item();if(item)item->type=cJSON_Object;return item;}

	/* Create Arrays: */
	cJSON *cJSON_CreateIntArray(int *numbers,int count)				{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
	cJSON *cJSON_CreateFloatArray(float *numbers,int count)			{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
	cJSON *cJSON_CreateDoubleArray(double *numbers,int count)		{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
	cJSON *cJSON_CreateStringArray(const char **strings,int count)	{int i;cJSON *n=0,*p=0,*a=cJSON_CreateArray();for(i=0;a && i<count;i++){n=cJSON_CreateString(strings[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
}

namespace glf
{
	namespace io
	{
		//----------------------------------------------------------------------
		ConfigLoader::ConfigLoader():
		root(NULL)
		{
		
		}
		//----------------------------------------------------------------------
		ConfigLoader::~ConfigLoader()
		{
			if(root!=NULL)
				cJSON_Delete(root);
		}
		//----------------------------------------------------------------------
		ConfigNode* ConfigLoader::Load(const std::string& _filename)
		{
			if(root!=NULL)
				cJSON_Delete(root);

			// Load file
			std::ifstream file(_filename.c_str());
			if(!file)
			{
				Error("File does not exist : %s",_filename.c_str());
				exit(-1);
			}
			assert(file);
			std::size_t dataSize = glf::GetFileSize(file);
			char* fileContent = new char[dataSize];
			file.read(fileContent,sizeof(char)*dataSize);

			// Parse data
			root = cJSON_Parse(fileContent);
			if(root==NULL)
			{
				Error("Error during parsing (file: %s): \n %s",_filename.c_str(),cJSON_GetErrorPtr());
				exit(-1);
			}
			delete[] fileContent;

			return root;
		}
		//----------------------------------------------------------------------
		ConfigNode* ConfigLoader::GetNode(	ConfigNode* _node,
											const std::string& _tag) const
		{
			assert(_node != NULL);
			ConfigNode* node = cJSON_GetObjectItem(_node,_tag.c_str());
			if(node == NULL)
			{
				Warning("This ConfigNode does not exist : %s",_tag.c_str());
			}
			return node;
		}
		//----------------------------------------------------------------------
		bool ConfigLoader::GetBool(			ConfigNode* _node,
											const std::string& _tag,
											bool _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
				return node->valueint>0;
			else
				return _default;
		}
		//----------------------------------------------------------------------
		int	ConfigLoader::GetInt(			ConfigNode* _node,
											const std::string& _tag,
											int _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
				return node->valueint;
			else
				return _default;
		}
		//----------------------------------------------------------------------
		float ConfigLoader::GetFloat(		ConfigNode* _node,
											const std::string& _tag,
											float _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
				return float(node->valuedouble);
			else
				return _default;
		}
		//----------------------------------------------------------------------
		std::string	ConfigLoader::GetString(ConfigNode* _node,
											const std::string& _tag,
											const std::string& _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
				return node->valuestring;
			else
				return _default;
		}
		//----------------------------------------------------------------------
		ConfigNode* ConfigLoader::GetNode(	ConfigNode* _node,
											int _i) const
		{
			assert(_node != NULL);
			assert(cJSON_GetArraySize(_node)>_i);
			return cJSON_GetArrayItem(_node,_i);
		}
		//----------------------------------------------------------------------
		int ConfigLoader::GetCount(ConfigNode* _node) const
		{
			assert(_node != NULL);
			return cJSON_GetArraySize(_node);
		}
		//----------------------------------------------------------------------
		glm::ivec2 ConfigLoader::GetIVec2(ConfigNode* _node,
										const std::string& _tag,
										glm::ivec2 _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
			{
				glm::ivec2 v;
				assert(GetCount(node)==2);
				v.x = float(GetNode(node,0)->valueint);
				v.y = float(GetNode(node,1)->valueint);
				return v;
			}
			else
				return _default;
		}
		//----------------------------------------------------------------------
		glm::vec2 ConfigLoader::GetVec2(ConfigNode* _node,
										const std::string& _tag,
										glm::vec2 _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
			{
				glm::vec2 v;
				assert(GetCount(node)==2);
				v.x = float(GetNode(node,0)->valuedouble);
				v.y = float(GetNode(node,1)->valuedouble);
				return v;
			}
			else
				return _default;
		}
		//----------------------------------------------------------------------
		glm::vec3 ConfigLoader::GetVec3(ConfigNode* _node,
										const std::string& _tag,
										glm::vec3 _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
			{
				glm::vec3 v;
				assert(GetCount(node)==3);
				v.x = float(GetNode(node,0)->valuedouble);
				v.y = float(GetNode(node,1)->valuedouble);
				v.z = float(GetNode(node,2)->valuedouble);
				return v;
			}
			else
				return _default;
		}
		//----------------------------------------------------------------------
		glm::vec4 ConfigLoader::GetVec4(ConfigNode* _node,
										const std::string& _tag,
										glm::vec4 _default) const
		{
			assert(_node != NULL);
			ConfigNode* node = GetNode(_node,_tag.c_str());
			if(node!=NULL)
			{
				glm::vec4 v;
				assert(GetCount(node)==4);
				v.x = float(GetNode(node,0)->valuedouble);
				v.y = float(GetNode(node,1)->valuedouble);
				v.z = float(GetNode(node,2)->valuedouble);
				v.w = float(GetNode(node,3)->valuedouble);
				return v;
			}
			else
				return _default;
		}
		//----------------------------------------------------------------------
	}
}
