#include <ssulib.h>
#include <device/console.h>
#include <device/io.h>
#include <syscall.h>
#include <filesys/file.h>

// void memcpy(void* from, void* to, uint32_t len)
// {
// 	uint32_t *p1 = (uint32_t*)from;
// 	uint32_t *p2 = (uint32_t*)to;
// 	int i, e;

// 	e = len/sizeof(uint32_t);
// 	for(i = 0; i<e; i++)
// 		p2[i] = p1[i];

// 	e = len%sizeof(uint32_t);
// 	if( e != 0)
// 	{
// 		uint8_t *p3 = (uint8_t*)p1;
// 		uint8_t *p4 = (uint8_t*)p2;

// 		for(i = 0; i<e; i++)
// 			p4[i] = p3[i];
// 	}
// }

int strncmp(char* b1, char* b2, int len)
{
	int i;

	for(i = 0; i < len; i++)
	{
		char c = b1[i] - b2[i];
		if(c)
			return c;
		if(b1[i] == 0)
			return 0;
	}
	return 0;
}

bool getkbd(char *buf, int len) 
{
	char ch;
	int offset = 0;

	len--;

	for(; offset < len && buf[offset] != 0; offset++)
		if(buf[offset] == '\n')
		{
			for(;offset>=0;offset--)
				buf[offset] = 0;
			offset++;
			break;
		}

	while ( (ch = ssuread()) >= 0)
	{
		if(ch == '\b' && offset == 0)
		{
			set_cursor();
			continue;
		}
		printk("%c",ch);
		set_cursor();
		if (ch == '\b')
		{
			buf[offset-1] = 0;
			offset--;
		}
		else if (ch == '\n')
		{
			buf[offset] = ch;
			return FALSE;
		}
		else
		{
			buf[offset] = ch;
			offset++;
		}
		if(offset == len) offset--;
	}

	/*
	   {
	   if (ch == '\b')
	   {
	   if(offset == 0)
	   {
	   set_cursor();
	   continue;
	   }
	   buf[offset-1] = 0;
	   offset -= 2;
	   printk("%c",ch);
	   }
	   else if (ch == '\n')
	   {
	   buf[offset] = ch;
	   printk("%c",ch);
	   return FALSE;
	   }
	   else
	   {
	   buf[offset] = ch;
	   printk("%c",ch);
	   }

	   if(offset < len) offset++;
	   }*/

	return TRUE;
}


int getToken(char* buf, char token[][BUFSIZ], int max_tok)
{
	int num_tok = 0;
	int off_tok = 0;
	while(num_tok < max_tok && *buf != '\n')
	{
		if(*buf == ' ') 
		{
			token[num_tok][off_tok] = 0;
			while(*buf == ' ') buf++;
			off_tok = 0;
			num_tok++;
		}
		else
		{
			token[num_tok][off_tok] = *buf;
			off_tok++;
			buf++;
		}
	}
	token[num_tok][off_tok] = 0;
	num_tok++;


	return num_tok;
}

int generic_read(int fd, void *buf, size_t len)
{
	struct ssufile *cursor;
	uint16_t *pos =  &(cur_process->file[fd]->pos);

	if( (cursor = cur_process->file[fd]) == NULL)
		return -1;

	if (~cursor->flags & O_RDONLY)
		return -1;

	if (*pos + len > cursor->inode->sn_size);
	len = cursor->inode->sn_size - *pos;

	file_read(cur_process->file[fd]->inode,*pos,buf,len);
	*pos += len;
	//printk("in generic read : %d \n", *pos);
	return len;
}

int generic_write(int fd, void *buf, size_t len)
{
	struct ssufile *cursor;
	uint16_t *pos =  &(cur_process->file[fd]->pos);

	if( (cursor = cur_process->file[fd]) == NULL)
		return -1;

	if (~cursor->flags & O_WRONLY)
		return -1;

	*pos=file_write(cur_process->file[fd]->inode,*pos,buf,len);
	*pos += len;
	//printk("in generic write : %d \n", *pos);
	return len;
}
//added
int generic_lseek(int fd, int offset, int whence, int aux)
{
	//SEEK_SET 0
	//SEEK_CUR 1
	//SEEK_END -1
	//
	//E 0
	//RE 1
	//A 2
	//C 3
	struct ssufile *cursor;
	uint16_t *pos = &(cur_process->file[fd]->pos);
	int tmp_pos = *pos;
	uint32_t f_size = cur_process->file[fd]->inode->sn_size;
	int j = 0;

	if( (cursor = cur_process->file[fd]) == NULL)
		return -1;

	if( ~cursor->flags & O_RDONLY)
		return -1;

	//whence flag 처리
	//offset이 음수일때는??
	if(whence == -1)
		tmp_pos = cursor->inode->sn_size + offset;

	if(whence == 0)
		tmp_pos = offset;

	if(whence == 1)
		tmp_pos = tmp_pos + offset;

	if(aux == -1)
		if( tmp_pos < 0 || tmp_pos > cursor->inode->sn_size)
			return -1;

	printk("before pos : %d\n", tmp_pos);

	if(aux == E){
		printk("before e size : %d\n", cur_process->file[fd]->inode->sn_size); 
		printk("*pos : %d\n", tmp_pos);
		if(tmp_pos > f_size){
			for(int i = 0; i < tmp_pos - f_size; i++){
				printk("for i : %d\n", i);
				file_write(cursor->inode, f_size + i, "", 1);
			}
		}
		printk("after e size : %d\n", cur_process->file[fd]->inode->sn_size); 
	}

	if(aux == RE){
		if(tmp_pos < 0){
			for(int i = tmp_pos; i < 0; i++){
				file_write(cursor->inode, f_size+j, "", 1);
				j++;
			}
			for(int i = 0; i < f_size; i++){
				file_read(cursor->inode, j+i, buf, wjeh 
				
			}
		}
	}

	if(aux == A) {

	}

	if(aux == C) {
		if(tmp_pos < 0){
			printk("1. pos %d f_size %d\n", tmp_pos, f_size);
			tmp_pos += f_size;
		}
		else if(tmp_pos > f_size){
			printk("2. pos %d f_size %d\n", tmp_pos, f_size);
			tmp_pos = tmp_pos - f_size;
		}
	}

	*pos = tmp_pos;

	return *pos;
}
