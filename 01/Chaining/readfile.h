/*
 * readfile.h
 *
 *  Created on: Sep 20, 2016
 *      Author: dina
 */

#ifndef READFILE_H_
#define READFILE_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


#define MBUCKETS  10					//Number of BUCKETS
#define RECORDSPERBUCKET 2				//No. of records inside each Bucket
#define BUCKETSIZE sizeof(Bucket)		//Size of the bucket (in bytes)
#define CHAIN_RECORDS_COUNT 20
#define CHAIN_RECORD_SIZE sizeof(ChainItem)
#define CHAIN_SIZE CHAIN_RECORDS_COUNT*CHAIN_RECORD_SIZE
#define FILESIZE (BUCKETSIZE*MBUCKETS)+CHAIN_SIZE    //Size of the file (Main buckets + overflow buckets)
#define PRIME  7

//Data Record inside the file
struct DataItem {
   int valid;    //) means invalid record, 1 = valid record
   int data;     
   int key;
};

struct ChainItem {
   int valid;    //) means invalid record, 1 = valid record
   int data;     
   int key;
   int chainPtr;
};

struct BucketPtr {
   int valid;
   int ptr;
};

//Each bucket contains number of records
struct Bucket {
	struct DataItem  dataItem[RECORDSPERBUCKET];
   struct BucketPtr bucketPtr;
};

//Check the create File
int createFile(int size, char *);

//check the openAddressing File
int deleteItem(int key);
int insertItem(int fd,DataItem item);
int DisplayFile(int fd);
int deleteDataItem(int filehandle, int key);
int searchItem(int filehandle,struct DataItem* item,int *count);




#endif /* READFILE_H_ */
