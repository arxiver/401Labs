#include "readfile.h"
void insert(int key, int data);
int deleteItem(int key);
struct DataItem *search(int key);
int filehandle; //handler for the database file
int dirhandle;  //handler for the database file
int main()
{
   //here we create a sample test to read and write to our database file
   //1. Create Database file or Open it if it already exists, check readfile.cpp
   filehandle = createFile(FILESIZE, "chaining");
   dirhandle = createFile(FILESIZE, "directory");
   //2. Display the database file, check openAddressing.cpp
   DisplayFile(filehandle);

   //3. Add some data in the table
   insert(1, 20);
   insert(2, 70);
   insert(42, 80);
   insert(4, 25);
   // And Finally don't forget to close the file.
   close(filehandle);
   return 0;
}

/* functionality: insert the (key,data) pair into the database table
                  and print the number of comparisons it needed to find
    Input: key, data
    Output: print statement with the no. of comparisons
*/
void insert(int key, int data)
{
   struct DataItem item;
   item.data = data;
   item.key = key;
   item.valid = 1;
   int result = insertItem(filehandle, item);
   printf("Insert: No. of searched records:%d\n", abs(result));
}

/* Functionality: search for a data in the table using the key

   Input:  key
   Output: the return data Item
*/
struct DataItem *search(int key)
{
   struct DataItem *item = (struct DataItem *)malloc(sizeof(struct DataItem));
   item->key = key;
   int diff = 0;
   int Offset = searchItem(filehandle, item, &diff); //this function is implemented for you in openAddressing.cpp
   printf("Search: No of records searched is %d\n", diff);
   if (Offset < 0) //If offset is negative then the key doesn't exists in the table
      printf("Item not found\n");
   else
      printf("Item found at Offset: %d,  Data: %d and key: %d\n", Offset, item->data, item->key);
   return item;
}

/* Functionality: delete a record with a certain key

   Input:  key
   Output: return 1 on success and -1 on failure
*/
int deleteItem(int key)
{
   int count = deleteDataItem(filehandle, key);
   if (count != -1)
      printf("Delete: No of records searched is %d\n", count);
   else
      printf("Cannot delete this key is not found!");
   return count;
}
