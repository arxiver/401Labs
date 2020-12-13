#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_DEPTH 5
#define MDIR 2 ^ MAX_DEPTH        //Number of directory entries
#define MBUCKETS 8                //Number Buckets
#define RECORDSPERBUCKET 2        //No. of records inside each Bucket
#define BUCKETSIZE sizeof(Bucket) //Size of the bucket (in bytes)
#define FILESIZE MDIR * sizeof(DataItem)
// global depth + directory ptrs
#define DIRSIZE sizeof(int) + MDIR * sizeof(DirItem)
#define PRIME 7

struct DirItem
{
   int valid; //) means invalid record, 1 = valid record
   int key;
   int bucketPtr;
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
   int localDepth;
};

//Check the create File
int createFile(int size, char *);

//check the openAddressing File
int deleteItem(int key);
int insertItem(int fd, DataItem item);
int DisplayFile(int fd);
int deleteDataItem(int filehandle, int key);
int searchItem(int filehandle, struct DataItem *item, int *count);

#endif /* READFILE_H_ */
