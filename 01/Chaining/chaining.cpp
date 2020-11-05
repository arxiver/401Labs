#include "readfile.h"

/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
   return key % MBUCKETS;
}


/* Functionality insert the data item into the correct position
 *          1. use the hash function to determine which bucket to insert into
 *          2. search for the first empty space within the bucket
 *          	2.1. if it has empty space
 *          	           insert the item
 *          	     else
 *          	          use OpenAddressing to insert the record
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which should be inserted into the database
 *
 * Output: No. of record searched
 *
 * Hint: You can check the search function to give you some insights
 * Hint2: Don't forget to set the valid bit = 1 in the data item for the rest of functionalities to work
 * Hint3: you will want to check how to use the pwrite function using man pwrite on the terminal
 * 			 ssize_t pwrite(int fd, const void *buf, size_t count, off_t offset);
 *
 * 	pwrite() writes up to count bytes from the buffer starting  at  buf  to
       the  file  descriptor  fd  at  offset  offset.
 */
int insertItem(int fd,DataItem item){
    struct DataItem data;                            //a variable to read in it the records from the db
    int count = 0;                                   //No of accessed records
    int hashIndex = hashCode(item.key);              //calculate the Bucket index
    int startingOffset = hashIndex * sizeof(Bucket); //calculate the starting address of the bucket
    int Offset = startingOffset;                     //Offset variable which we will use to iterate on the db
	ssize_t result;
	bool written = false;
	for (int i = 0; i < RECORDSPERBUCKET; ++i){
		result = pread(fd, &data, sizeof(DataItem), Offset);
		count++;
		if (result <= 0) //either an error happened in the pread or it hit an unallocated space
		{                // 
			perror("some error occurred in pread");
			return -1;
		}
		else if (data.valid != 1)
		{
			written = true;
			item.valid = 1;
			result = pwrite(fd, &item, sizeof(DataItem), Offset);
			if (result <= 0){
				perror("some error occurred in write");
				return -1;
			}
			return count;
		}
		else
		{   
			Offset += sizeof(DataItem); //move the offset to next record
		}
	}
	if(!written){
		// 1- check whether the overflow chain is connected to this bucket or not
		int startingOverflow = MBUCKETS * sizeof(Bucket);
		int overflowOffset = startingOverflow;
	    struct ChainItem data;
		struct ChainItem newItem;
		newItem.data = item.data;
		newItem.key = item.key;
		newItem.valid = 1;
		newItem.chainPtr = 0;
		while (overflowOffset < FILESIZE && !written)
		{
			count++;
			// fetch first empty place and take it's offset and attach the data to it
			// and attach the address of it to the main bucket
			result = pread(fd, &data, CHAIN_RECORD_SIZE, overflowOffset);
			if (result <= 0) //either an error happened in the pread or it hit an unallocated space
			{                // perror("some error occurred in pread");
				perror("some error occurred in pread");
				return -1;
			}
			else if (data.valid != 1)
			{
				written = true;
				newItem.valid = 1;
				result = pwrite(fd, &newItem, CHAIN_RECORD_SIZE, overflowOffset);
				if (result <= 0) //either an error happened in the pread or it hit an unallocated space
				{                // perror("some error occurred in pread");
					perror("some error occurred in pwrite");
					return -1;
				}
				// connect the chain 
				int ptr;
				int bucketPtrOffset = startingOffset + sizeof(Bucket) - sizeof(int);
				result = pread(fd, &ptr, sizeof(ptr), bucketPtrOffset);
				if (result <= 0){
					perror("some error occurred in pread");
					return -1;
				}
				if (ptr == 0){
					// bucket is never connected before!
					result = pwrite(fd, &overflowOffset , sizeof(overflowOffset), bucketPtrOffset);
					if (result <= 0) //either an error happened in the pread or it hit an unallocated space
					{                // 
						perror("some error occurred in pwrite");
						return -1;
					}
				}
				else {
					// bucket is connected 
					struct ChainItem chainItem ;
					int prevPtr;
					while (ptr != 0) {
						prevPtr = ptr;
						result = pread(fd, &chainItem, sizeof(chainItem), ptr);
						if (result <= 0){
							perror("some error occurred in pread");
							return -1;
						}
						if(chainItem.valid == 1)
							ptr = chainItem.chainPtr;
						else 
							break;
					}
					chainItem.chainPtr = overflowOffset;
					result = pwrite(fd, &chainItem, CHAIN_RECORD_SIZE, prevPtr);
					if (result <= 0){
						perror("some error occurred in pwrite");
						return -1;
					}
				}
				break;
			}
			else 
			{
				overflowOffset += CHAIN_RECORD_SIZE;
			}
		}
	}
    return count;
}


/* Functionality: using a key, it searches for the data item
 *          1. use the hash function to determine which bucket to search into
 *          2. search for the element starting from this bucket and till it find it.
 *          3. return the number of records accessed (searched)
 *
 * Input:  fd: filehandler which contains the db
 *         item: the dataitem which contains the key you will search for
 *               the dataitem is modified with the data when found
 *         count: No. of record searched
 *
 * Output: the in the file where we found the item
 */

int searchItem(int fd,struct DataItem* item,int *count)
{

	//Definitions
	struct DataItem data;   //a variable to read in it the records from the db
	*count = 0;				//No of accessed records
	int rewind = 0;			//A flag to start searching from the first bucket
	int hashIndex = hashCode(item->key);  				//calculate the Bucket index
	int startingOffset = hashIndex*sizeof(Bucket);		//calculate the starting address of the bucket
	int Offset = startingOffset;						//Offset variable which we will use to iterate on the db

	//Main Loop
	RESEEK:
	//on the linux terminal use man pread to check the function manual
	ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
	//one record accessed
	(*count)++;
	//check whether it is a valid record or not
    if(result <= 0) //either an error happened in the pread or it hit an unallocated space
	{ 	 // perror("some error occurred in pread");
		  return -1;
    }
    else if (data.valid == 1 && data.key == item->key) {
    	//I found the needed record
    			item->data = data.data ;
    			return Offset;

    } else { //not the record I am looking for
    		Offset +=sizeof(DataItem);  //move the offset to next record
    		if(Offset >= FILESIZE && rewind ==0 )
    		 { //if reached end of the file start again
    				rewind = 1;
    				Offset = 0;
    				goto RESEEK;
    	     } else
    	    	  if(rewind == 1 && Offset >= startingOffset) {
    				return -1; //no empty spaces
    	     }
    		goto RESEEK;
    }
}


/* Functionality: Display all the file contents
 *
 * Input:  fd: filehandler which contains the db
 *
 * Output: no. of non-empty records
 */
int DisplayFile(int fd){

	struct DataItem data;
	int count = 0;
	int Offset = 0;
	int bucketItr = 0;
	for (bucketItr = 0; bucketItr < MBUCKETS; bucketItr++)
	{
		int endOffset = ((bucketItr+1) * sizeof(Bucket)) - sizeof(int);
		//printf("End offset is %d of bucket itr %d\n\n",endOffset,bucketItr);
		for(Offset = (bucketItr * sizeof(Bucket)) ; Offset < endOffset; Offset += sizeof(DataItem))
		{
			ssize_t result = pread(fd,&data,sizeof(DataItem), Offset);
			if(result < 0)
			{ 	  perror("some error occurred in pread");
				return -1;
			} else if (result == 0 || data.valid == 0 ) { //empty space found or end of file
				printf("Bucket: %d, Offset %d:~\n",Offset/BUCKETSIZE,Offset);
			} else {
				pread(fd,&data,sizeof(DataItem), Offset);
				printf("Bucket: %d, Offset: %d, Data: %d, key: %d\n",Offset/BUCKETSIZE,Offset,data.data,data.key);
						count++;
			}
		}
		int nextRecordOffset;
		struct ChainItem chainItem;

		ssize_t result = pread(fd, &nextRecordOffset, sizeof(nextRecordOffset), endOffset);
		if(result <= 0){
			return -1;
		}
		printf("Bucket chainPtr: %d\n",nextRecordOffset);
		while (nextRecordOffset != 0)
		{
			if(result < 0)
			{ 	  
				perror("some error occurred in pread");
				return -1;
			} else if (result == 0 || chainItem.valid == 0 ) { //empty space found or end of file
				printf("Bucket: %d, Offset %d:~\n",bucketItr,nextRecordOffset);
				break;
			} else {
				pread(fd,&chainItem,CHAIN_RECORD_SIZE, nextRecordOffset);
				printf("Bucket: %d, Offset: %d, Data: %d, key: %d, chainPtr: %d\n",bucketItr,nextRecordOffset,chainItem.data,chainItem.key,chainItem.chainPtr);
				nextRecordOffset = chainItem.chainPtr;
				count++;
			}
		}
		printf("\n");
		
	}
	return count;
}


/* Functionality: Delete item at certain offset
 *
 * Input:  fd: filehandler which contains the db
 *         Offset: place where it should delete
 *
 * Hint: you could only set the valid key and write just and integer instead of the whole record
 */
int deleteOffset(int fd, int Offset)
{
	struct DataItem dummyItem;
	dummyItem.valid = 0;
	dummyItem.key = -1;
	dummyItem.data = 0;
	int result = pwrite(fd,&dummyItem,sizeof(DataItem), Offset);
	return result;
}

