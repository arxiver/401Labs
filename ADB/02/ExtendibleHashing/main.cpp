#include "readfile.h"
void insert(int key, int data);
int deleteItem(int key);
struct DataItem search(int key);
int filehandle; //handler for the database file
int dirhandle;  //handler for the database file
int main()
{
   //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE, "extendible");
   dirhandle = createFile(DIRSIZE, "directory");
   insert(1,1);
   insert(16,2);
   search(16);
   search(1);
   insert(28,10);
   search(28);
   deleteItem(28);
   insert(21,14);
   insert(24,15);
   search(228);
   deleteItem(21);
   deleteItem(24);
   deleteItem(1);
   deleteItem(16);
   deleteItem(16);
   
   DisplayFile(filehandle);
   // And Finally don't forget to close the file.
   close(filehandle);
   close(dirhandle);
   return 0;
}

void insert(int key, int data)
{
   struct DataItem item;
   item.data = data;
   item.key = key;
   item.valid = 1;
   int result = insertItem(filehandle,dirhandle, item);
   printf("Insert: [%d: %d] No. of searched records:%d\n",key,data,abs(result));
}

struct DataItem search(int key)
{
   struct DataItem item;
   item.key = key;
   int diff = 0;
   int Offset = searchItem(filehandle,dirhandle,item, diff); 
   printf("Search: No of records searched is %d\n", diff);
   if (Offset < 0) //If offset is negative then the key doesn't exists in the table
   {  item.valid = -1;
      printf("Item not found\n");
   }
   else
   {
   	item.valid = 1;
    printf("Item found at Offset: %d,  Data: %d and key: %d\n", Offset, item.data, item.key);
   }
   	  
   return item;
}

/* Functionality: delete a record with a certain key

   Input:  key
   Output: return 1 on success and -1 on failure
*/
int deleteItem(int key)
{
   struct DataItem item;
   item.key = key;
   int diff = 0;
   int Offset = searchItem(filehandle,dirhandle,item, diff); 
   printf("Delete: No of records searched is %d\n", diff);
   if (Offset < 0) //If offset is negative then the key doesn't exists in the table
   { 
    printf("Item not found\n");
    return -1;
   }

   printf("Item Deleted at Offset: %d,  Data: %d and key: %d\n", Offset, item.data, item.key);
   return deleteOffset(filehandle,Offset);
   	
   
   	  
}
