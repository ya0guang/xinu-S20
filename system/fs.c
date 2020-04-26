#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
int inode_id_init = 10086;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD]; // open file table
int next_open_fd = 0;

#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (((fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock)
{
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2)
  {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in)
{
  int bl, inn;
  int inode_off;

  if (dev != 0)
  {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes)
  {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;
}

/* write inode indicated by pointer to device */
int fs_put_inode_by_num(int dev, int inode_number, struct inode *in)
{
  int bl, inn;

  if (dev != 0)
  {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes)
  {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn * sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}

/* create file system on device; write file system block and block bitmask to
 * device */
int fs_mkfs(int dev, int num_inodes)
{
  int i;

  if (dev == 0)
  {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else
  {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1)
  {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else
  {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ((i % 8) != 0)
  {
    i++;
  }
  fsd.freemaskbytes = i / 8;

  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR)
  {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }

  /* zero the free mask */
  for (i = 0; i < fsd.freemaskbytes; i++)
  {
    fsd.freemask[i] = '\0';
  }

  fsd.inodes_used = 0;

  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

/* print information related to inodes*/
void fs_print_fsd(void)
{

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b)
{
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b)
{
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return (((fsd.freemask[mbyte] << mbit) & 0x80) >> 7);
  return OK;
}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b)
{
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void)
{ // print block bitmask
  int i, j;

  for (i = 0; i < fsd.freemaskbytes; i++)
  {
    for (j = 0; j < 8; j++)
    {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ((i % 8) == 7)
    {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_get_entry_index(char *filename)
{
  int i;
  int dir_index = FILENAMELEN + 1;

  for (i = 0; i < DIRECTORY_SIZE; i += 1)
  {
    //DEBUG
    // kprintf("strncmp of %s, %s: %d\n", fsd.root_dir.entry[i].name, filename, strncmp(fsd.root_dir.entry[i].name, filename, FILENAMELEN));
    if (strncmp(fsd.root_dir.entry[i].name, filename, FILENAMELEN) == 0)
    {
      dir_index = i;
      break;
    }
  }

  if (dir_index > DIRECTORY_SIZE)
  {
    kprintf("No such file or directory. \n");
    return SYSERR;
  }

  return dir_index;
}

//return fd
int fs_open(char *filename, int flags)
{
  int i, fd;
  int dir_index;

  //TODO: OPEN TWICE

  // find the entry index in root_dir
  dir_index = fs_get_entry_index(filename);
  if (dir_index == SYSERR)
  {
    kprintf("OPEN FAIL\n");
    return SYSERR;
  }

  //set the open file table
  fd = next_open_fd++;
  oft[fd].state = FSTATE_OPEN;
  oft[fd].fileptr = 0;
  oft[fd].flag = flags;
  oft[fd].de = &fsd.root_dir.entry[dir_index];
  fs_get_inode_by_num(0, oft[fd].de->inode_num, &oft[fd].in);

  return fd;
}

int fs_close(int fd)
{
  //TODO: Validity Check

  //write changes of inode into the block
  fs_put_inode_by_num(0, oft[fd].de->inode_num, &oft[fd].in);
  oft[fd].state = FSTATE_CLOSED;

  return OK;
}

int fs_create(char *filename, int mode)
{
  int i, fd;
  int free_inode_number = fsd.ninodes + 1;
  int free_dir_index = DIRECTORY_SIZE + 1;
  struct inode inode_get, inode_new;

  // find there exist a free inode for new file
  // if (fsd.ninodes <= fsd.inodes_used + 1)
  // {
  //   return SYSERR;
  // }
  // inode 0 will never be used!
  for (i = 1; i < fsd.ninodes; i += 1)
  {
    fs_get_inode_by_num(0, i, &inode_get);
    // printf("id: %d, type: %d, nlink: %d, device: %d, size: %d, blocks: ", inode_get.id, inode_get.type, inode_get.nlink, inode_get.device, inode_get.size);
    // for (j = 0; j < INODEBLOCKS; j += 1)
    // {
    //   printf(" %d ", inode_get.blocks[j]);
    // }
    if (inode_get.nlink == 0)
    {
      free_inode_number = i;
      break;
    }
  }
  //find a free directory entry
  for (i = 0; i < DIRECTORY_SIZE; i += 1)
  {
    if (fsd.root_dir.entry[i].inode_num == 0)
    {
      free_dir_index = i;
      break;
    }
  }

  if ((free_dir_index > DIRECTORY_SIZE) || (free_inode_number > fsd.ninodes))
  {
    kprintf("No free dir entry or inode,\n");
    return SYSERR;
  }

  // set dir entry
  fsd.root_dir.entry[free_dir_index].inode_num = free_inode_number;
  strncpy(fsd.root_dir.entry[free_dir_index].name, filename, FILENAMELEN);

  inode_new.id = inode_id_init++;
  inode_new.nlink = 1;
  inode_new.device = 0;
  inode_new.size = 0;
  inode_new.type = INODE_TYPE_FILE;

  fs_put_inode_by_num(0, free_inode_number, &inode_new);

  // open it in oft
  fd = next_open_fd++;
  oft[fd].state = FSTATE_OPEN;
  oft[fd].fileptr = 0;
  oft[fd].de = &fsd.root_dir.entry[free_dir_index];
  oft[fd].in = inode_new;
  oft[fd].flag = O_RDWR;

  //DEBUG
  // kprintf("oft[%d]: in.id: %d, filename: %s, dirent num: %d \n",
  //        fd, oft[fd].in.id, oft[fd].de->name, oft[fd].de->inode_num);

  return fd;
}

int fs_seek(int fd, int offset)
{
  //TODO: Validity check
  oft[fd].fileptr += offset;
  return OK;
}

int fs_read(int fd, void *buf, int nbytes)
{
  //TODO: Validity check
  char read_buf[fsd.blocksz * INODEBLOCKS];
  int read_size = 0;
  int to_read = 0;
  int read_blks = 0;
  int blk_index = 0;

  while (read_size != oft[fd].in.size)
  {
    if ((oft[fd].in.size - read_size) > 512)
    {
      to_read = 512;
    }
    else
    {
      to_read = oft[fd].in.size - read_size;
    }

    // read from block
    blk_index = oft[fd].in.blocks[read_blks++];
    bs_bread(0, blk_index, 0, &read_buf[read_size], to_read);
    read_size += to_read;
  }

  memcpy(buf, &read_buf[oft[fd].fileptr], nbytes);
  oft[fd].fileptr += nbytes;

  return nbytes;
}

// return number of bytes written
int fs_write(int fd, void *buf, int nbytes)
{
  int remain = nbytes;
  int free_blk_num = fsd.nblocks + 1;
  int bytes_write;
  void *bufptr = buf;
  int i = INODEBLOCKS + 2;
  int inode_blk_index = 0;
  int fp = oft[fd].fileptr;

  //TODO: find the position of fileptr and start write

  if ((oft[fd].flag == O_RDONLY) || (oft[fd].in.type == INODE_TYPE_DIR))
  {
    kprintf("Writing a Read Only file / dir is NOT allowed! \n");
    return SYSERR;
  }

  while (remain > 0)
  {
    //find a free block
    for (; i <= fsd.nblocks; i += 1)
    {
      if (fs_getmaskbit(i) == 0)
      {
        free_blk_num = i;
        break;
      }
    }

    if (i > fsd.nblocks)
    {
      kprintf("No Free Block to write! \n");
      return SYSERR;
    }

    //determain the length about to write
    if (remain >= 512)
    {
      bytes_write = 512;
      remain = remain - 512;
    }
    else
    {
      bytes_write = remain;
      remain = 0;
    }

    bs_bwrite(0, free_blk_num, 0, bufptr, bytes_write);
    oft[fd].fileptr += bytes_write;
    oft[fd].in.blocks[inode_blk_index++] = free_blk_num;
    oft[fd].in.size += bytes_write;
    fs_setmaskbit(free_blk_num);
  }

  //write inode to disk
  fs_put_inode_by_num(0, oft[fd].de->inode_num, &oft[fd].in);
  fp = oft[fd].fileptr - fp;

  //DEBUG INFO
  // kprintf("Bytes Written: %d \n", fp);

  return fp;
}

int fs_link(char *src_filename, char *dst_filename)
{
  int i;
  int src_dir_index;
  int inode_num;
  int free_dir_index = DIRECTORY_SIZE + 1;
  struct inode new_inode;

  // find a free dir entry
  for (i = 0; i < DIRECTORY_SIZE; i += 1)
  {
    if (fsd.root_dir.entry[i].inode_num == 0)
    {
      free_dir_index = i;
      break;
    }
  }
  if (free_dir_index > DIRECTORY_SIZE)
  {
    kprintf("No free dir entry or inode,\n");
    return SYSERR;
  }

  // Get the entry of scr_filename
  src_dir_index = fs_get_entry_index(src_filename);
  if (src_dir_index > DIRECTORY_SIZE)
  {
    kprintf("Link Fail\n");
    return SYSERR;
  }

  // setup the new entry
  inode_num = fsd.root_dir.entry[src_dir_index].inode_num;
  fsd.root_dir.entry[free_dir_index].inode_num = inode_num;
  strncpy(fsd.root_dir.entry[free_dir_index].name, dst_filename, FILENAMELEN);

  //inode nlink ++
  fs_get_inode_by_num(0, inode_num, &new_inode);
  new_inode.nlink += 1;
  fs_put_inode_by_num(0, inode_num, &new_inode);
}

int fs_unlink(char *filename)
{
  int i, dir_index, inode_num;
  struct inode inode_current;

  dir_index = fs_get_entry_index(filename);
  if (dir_index == SYSERR)
  {
    kprintf("unlink ERROR: no such file or dir\n");
    return SYSERR;
  }

  // set inode_num to 0
  inode_num = fsd.root_dir.entry[dir_index].inode_num;
  fsd.root_dir.entry[dir_index].inode_num = 0;

  // operation on inode
  fs_get_inode_by_num(0, inode_num, &inode_current);
  inode_current.nlink -= 1;
  // delete inode info
  if (inode_current.nlink == 0)
  {
    inode_current.id = 0;
    inode_current.type = 0;
    for (i = 0; i < INODEBLOCKS; i += 1)
    {
      if (inode_current.blocks[i] == 0)
      {
        break;
      }
      else
      {
        fs_clearmaskbit(inode_current.blocks[i]);
      }
    }
    inode_current.size = 0;
  }

  return OK;
}
#endif /* FS */