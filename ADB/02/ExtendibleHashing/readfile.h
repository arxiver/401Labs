#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_DEPTH 5
#define MDIR (1 << MAX_DEPTH)        //Number of directory entries
#define RECORDSPERBUCKET 2        //No. of records inside each Bucket
#define BUCKETSIZE sizeof(Bucket) //Size of the bucket (in bytes)
#define FILESIZE MDIR * BUCKETSIZE
// global depth + directory ptrs
#define DIRSIZE sizeof(Directory)
#define PRIME 7

struct DirItem
{
   int ptr;
};
struct Depth {
   int valid;
   int value;
};
struct Directory {
   Depth depth;
   struct DirItem items[MDIR];
};
//Data Record inside the file
struct DataItem
{
   int valid; //) means invalid record, 1 = valid record
   int data;
   int key;
};

//Each bucket contains number of records
struct Bucket
{
   struct DataItem dataItem[RECORDSPERBUCKET];
   int depth;
   int valid;
};

//Check the create File
int createFile(int size, char *);

//check the openAddressing File
int deleteItem(int key);
int insertItem(int fd, int dd, DataItem item);
int DisplayFile(int fd);
int deleteDataItem(int filehandle, int key);
int searchItem(int filehandle, struct DataItem *item, int *count);

bool cmpMostNBits(int a,int b, int n);
bool updateDirectory(int fd, int dd, int overflow);
void updateBuckets(int fd,int dir1,int dir2);
bool pushBucketItem(int fd, int offset, DataItem item,int & searched,bool);
void updateBucketLocalDepth(int fd, int offset, int newDepth);
int readLocalDepth(int fd, int dir);
int findFreeBucket(int fd);
#endif /* READFILE_H_ */
