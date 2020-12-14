#include <set>
#include "readfile.h"
bool dirCache = false;
Directory directory;
bool readDirectory(int dd)
{      
      int result = pread(dd,&directory,sizeof(directory),0);
      if (result != -1)
            dirCache = true;
      return dirCache;
}
int insertItem(int fd ,int dd, DataItem item)
{
      int searched = 0, result = 0;
      bool inserted = false;
      if(!dirCache) readDirectory(dd);
      if (directory.depth.valid != 1){
            directory.depth.valid = 1;
            directory.depth.value = 0;
            directory.items[0].ptr = 0;
            result = pwrite(dd, &directory, sizeof(Depth), 0);
      }
      int dirSize = (1 << directory.depth.value);
      int dirIndex = -1;
      for (int i = 0; i < dirSize; i++)
      {
            if (cmpMostNBits(i,item.key,directory.depth.value)){
                  dirIndex = i;
                  break;
            }
      }
      int bucketOffset = directory.items[dirIndex].ptr;
      inserted = pushBucketItem(fd,bucketOffset,item,searched,false);
      if (inserted) return searched;
      updateDirectory(fd,dd,dirIndex);
      //if (item.data == 8) return 0;
      return searched + insertItem(fd,dd,item);
}
int findFreeBucket(int fd){
      int filled,result;
      for (size_t i = sizeof(Bucket); i < FILESIZE; i+= sizeof(Bucket))
      {
            result = pread(fd,&filled,sizeof(int),i-sizeof(int));
            if (filled != 1){
                  return (i-BUCKETSIZE);
            }
      }
      printf("Full buckets cannot insert it\n");
      return -1;
}
void updateBuckets(int fd,int dir1,int dir2){
      int result;
      Bucket bucket1,tempBucket1,tempBucket2;
      result = pread(fd, &bucket1, sizeof(Bucket), directory.items[dir1].ptr);
      int localDepth = bucket1.depth + 1;
      tempBucket1.depth = localDepth;
      tempBucket2.depth = localDepth;
      tempBucket1.valid = 1;
      tempBucket2.valid = 1;
      int b1Itr = 0;
      int b2Itr = 0;
      bool match;
      for (int i = 0; i < RECORDSPERBUCKET; i++)
      {
            // check 
            if (bucket1.dataItem[i].valid == 1){
                  match = cmpMostNBits(dir1,bucket1.dataItem[i].key,localDepth);
                  if(match) {tempBucket1.dataItem[b1Itr] = bucket1.dataItem[i]; b1Itr++;}
                  else {tempBucket2.dataItem[b2Itr] = bucket1.dataItem[i]; b2Itr++;}
            }
      }
      for (int i = b1Itr; i < RECORDSPERBUCKET; i++)
            tempBucket1.dataItem[i].valid = 0;

      for (int i = b2Itr; i < RECORDSPERBUCKET; i++)
            tempBucket2.dataItem[i].valid = 0;

      result = pwrite(fd, &tempBucket1, sizeof(Bucket), directory.items[dir1].ptr);
      result = pwrite(fd, &tempBucket2, sizeof(Bucket), directory.items[dir2].ptr);
}
int readLocalDepth(int fd, int dir){
      int ld;
      pread(fd,&ld,sizeof(int),directory.items[dir].ptr + BUCKETSIZE - (2*sizeof(int)));
      return ld;
}
bool updateDirectory(int fd, int dd,int dirIdx)
{
      int localDepth = readLocalDepth(fd,dirIdx);
      if(localDepth < directory.depth.value)
      {
            int freeBucket = findFreeBucket(fd);
            if (freeBucket == -1)
                  return false;
            int bucketOffset = directory.items[dirIdx].ptr;
            int globalDepth = directory.depth.value;
            int divSize = (1 << (globalDepth - localDepth));
            int divStart = (dirIdx >> (globalDepth-localDepth));
            divStart = (divStart << (globalDepth-localDepth));
            printf("m : %d, %d , %d\n",divStart,divSize,dirIdx);
            for (int i = divStart + (divSize/2); i < divStart + divSize; i+=1)
            {
                  directory.items[i].ptr = freeBucket;
            }
            // update bucket local depth
            updateBuckets(fd,divStart,divStart+(divSize/2));
            pwrite(dd,&directory,sizeof(Directory),0);
            return false;
      }
      // Bucekt split and Directory expansion
      Directory tempDir;
      tempDir.depth = directory.depth;
      tempDir.depth.value ++;
      int extend = 1 << tempDir.depth.value;
      int dirItr = 0;
      for (int i = 0; i < extend; i+=2)
      {
            tempDir.items[i].ptr = directory.items[dirItr].ptr;
            tempDir.items[i+1].ptr = directory.items[dirItr].ptr;
            dirItr++;
      }
      int freeBucket = findFreeBucket(fd);
      tempDir.items[dirIdx*2+1].ptr = freeBucket;
      directory = tempDir;
      int result = pwrite(dd,&tempDir,sizeof(Directory),0);
      updateBuckets(fd, dirIdx*2, (dirIdx*2)+1);
      return true;
}
void updateBucketLocalDepth(int fd, int offset, int newDepth){
      pwrite(fd,&newDepth,sizeof(int),offset+BUCKETSIZE-2*sizeof(int));
}
bool cmpMostNBits(int dir, int key, int n){
      key %= MDIR;
      dir %= MDIR;
      return ((key>>(MAX_DEPTH - n)) == dir);
}
bool pushBucketItem(int fd, int offset, DataItem item,int & searched,bool first=false)
{      
      int result;
      DataItem di;
      printf("Inserting key %d into .. %d \n",item.data,offset);
      for(int i = offset; i < offset+BUCKETSIZE-2*sizeof(int); i += sizeof(DataItem)){
            searched++;
            result = pread(fd,&di,sizeof(DataItem),i);
            if(di.valid != 1 && result != -1){
                  result = pwrite(fd,&item,sizeof(DataItem),i);
                  int valid = 1;
                  result = pwrite(fd,&valid,sizeof(int),offset+BUCKETSIZE-sizeof(int));
                  return true;
                  break;
            }
      }
      return false;
}
int searchItem(int fd, struct DataItem *item, int *count)
{

}
int DisplayFile(int fd)
{
      int result,localdepth,itemsItr=0;
      std::set<int> s;     
      printf("\nDirectoy file, global depth: %d\n",directory.depth.value);
      for (int i = 0; i < (1<<directory.depth.value); i++)
      {
            s.insert(directory.items[i].ptr);
            printf("\tDir: %d, ptr: %d\n",i,directory.items[i].ptr);
      }
      printf("-----------------------------\n");
      std::set<int>::iterator it = s.begin();
      DataItem item;
      int sItr;
      while (it != s.end())
      {
            sItr = *it;
            it++;
            itemsItr=0;
            result = pread(fd,&localdepth,sizeof(int),sItr+BUCKETSIZE-2*sizeof(int));
            printf("Bucket offset %d, localdepth: %d\n",sItr,localdepth);
            for (size_t i = sItr; i < sItr+BUCKETSIZE-(2*sizeof(int)) ; i+=sizeof(DataItem))
            {
                  result = pread(fd,&item,sizeof(DataItem),i);
                  if (item.valid == 1 && result != -1)
                        printf("\tRecord #%d, key: %d, data: %d\n",itemsItr,item.key,item.data);
                  itemsItr++;
            }
      }
      return 1;
}
int deleteDataItem(int fd, int key)
{
	
}
