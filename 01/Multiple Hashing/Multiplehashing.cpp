#include "readfile.h"

/* Hash function to choose bucket
 * Input: key used to calculate the hash
 * Output: HashValue;
 */
int hashCode(int key){
   return key % MBUCKETS;
}

//PRIME is a prime smaller than the Number of BUCKETS.
int hashCode2(int key){
   return  (PRIME - (key % PRIME));
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
    int rewind = 0;                                  //A flag to start searching from the first bucket
    int hashIndex = hashCode(item.key);              //calculate the Bucket index
    int hashIndex2 = hashCode2(item.key);               
    int i=0; //for BUCKETS
	int j=0; //for RECORD
    while (i < MBUCKETS)
    {
    	int Offset = ((hashIndex+(i*hashIndex2))%MBUCKETS)*sizeof(Bucket);		
		Offset += j*sizeof(DataItem);	
        ssize_t result = pread(fd, &data, sizeof(DataItem), Offset);
        count++;
        if (result <= 0) //either an error happened in the pread or it hit an unallocated space
        {                // perror("some error occurred in pread");
            return -1;
        }
        else if (data.valid == 1)
        {
        	j+=1;
    		j=j%RECORDSPERBUCKET;
    		if (j%2==0)
    		{
    			i +=1;
    		}  
        }
        else
        {   
            item.valid = 1;
            pwrite(fd, &item, sizeof(DataItem), Offset);
            break;
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
	int hashIndex = hashCode(item->key);  				//calculate the Bucket index
	int hashIndex2 = hashCode2(item->key);
	int i=0; //for BUCKETS
	int j=0; //for RECORD
	//Main Loop
	RESEEK:
	int Offset = ((hashIndex+i*hashIndex2)%MBUCKETS)*sizeof(Bucket);		//calculate the starting address of the bucket
	Offset += j*sizeof(DataItem);	
	
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
    		j+=1;
    		j=j%RECORDSPERBUCKET;
    		if (j%2==0)
    		{
    			i +=1;
    		}  
    	   	if(i >= MBUCKETS) {
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
	for(Offset =0; Offset< FILESIZE;Offset += sizeof(DataItem))
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

