#ifndef DEBUG
#define DEBUG
#include"buffer.h"
#endif // !DEBUG


string rootdir = ".\\minisql\\";

FileInfo filelist = nullptr;
int fileNum = 0;


//Ѱ�ҿտ�
BlockInfo findBlock()
{
	static int i = 0;//����block����
	FileInfo fileptr = filelist;
	BlockInfo resblock = nullptr;
	while (fileptr)
	{
		BlockInfo blockptr = fileptr->firstBlock;
		if (fileptr->freeNum == 0)//����ļ�����û�пտ���
		{
			fileptr = fileptr->next;
			continue;
		}
			
		while (blockptr)//����ļ������пտ�
		{
			if (blockptr->isfree)//�ҵ�һ���տ�
			{
				resblock = blockptr;
				fileptr->freeNum--;//�տ�����-1
				blockptr->isfree = false;
				remove_block_from_file(resblock);//��������ԭ�ļ����Ƴ�
				break;
			}
			blockptr = blockptr->next;
		}
		if (!resblock)
			cout << "findBlock::freeNum record may be wrong." << endl;
		else 
			return resblock;

		fileptr = fileptr->next;
	}
	//�Ѿ�û�пտ���
	//��������տ�
	if (i < MAX_BLOCK)
	{
		resblock = new struct blockInfo;
		
		if (resblock)
		{
			resblock->call_times = 0;
			resblock->charNum = 0;
			resblock->dirtyBit = 0;
			resblock->file = nullptr;
			resblock->isfree = 1;
			resblock->lock = 0;
			resblock->next = nullptr;
			resblock->cBlock = new char[BLOCK_SIZE];
			if (resblock->cBlock)
				return resblock;
		}
	}

	//��������տ��ˣ���LRU�������п飬�滻��һ��
	int time = 0x7fffffff;
	fileptr = filelist;
	while (fileptr)
	{
		BlockInfo blockptr = fileptr->firstBlock;
		while (blockptr)
		{
			if (blockptr->call_times < time && !blockptr->lock)//��LRU��û�����Ŀ����滻
			{
				time = blockptr->call_times;
				resblock = blockptr;
			}
			blockptr = blockptr->next;
		}
		fileptr = fileptr->next;
	}
	resblock->call_times = 1;
	remove_block_from_file(resblock);
	return resblock;

}

//��filelist����һ���ļ�
FileInfo get_file_info(string fileName, int fileType)
{
	if (!filelist)return nullptr;
	FileInfo fileptr = filelist;

	//���ļ�
	while (fileptr)
	{
		if (fileptr->fileName == fileName && fileptr->type == fileType)
			return fileptr;
		fileptr = fileptr->next;
	}

	//�ļ�������
	cout << "get_file_info::No such file" << endl;
	return nullptr;
}

//��buffer�в���һ����
BlockInfo get_block_in_file(FileInfo file, int blockNum)
{
	if (!file)
		return nullptr;

	BlockInfo blockptr = file->firstBlock;
	
	while (blockptr)
	{
		if (blockptr->blockNum == blockNum)
			return blockptr;

		blockptr = blockptr->next;
	}
	return nullptr;
}

//��buffer�а�һ�����Ƴ���Ӧ�ļ�
void remove_block_from_file(BlockInfo block_to_remove)
{
	FileInfo file = block_to_remove->file;
	BlockInfo last_block = nullptr;
	BlockInfo blockptr = file->firstBlock;

	while (blockptr)//�ڸ��ļ���Ѱ�Ҷ�Ӧ��
	{
		if (blockptr->next == block_to_remove)
			break;
		last_block = blockptr;
		blockptr = blockptr->next;
	}
	if (blockptr)//�ҵ���
	{
		if (last_block)
			file->firstBlock = blockptr->next;
		else
			last_block->next = blockptr->next;

	}
	else
		cout << "remove_block_from_file::Cannot find this block" << endl;
	return;
}

//��������������һ����ҵ��ļ���
void add_block_to_file(BlockInfo block,FileInfo file)
{
	block->file = file;
	block->next = file->firstBlock;
	file->firstBlock = block;
	return;
}

//�Ӵ��̶�һ����
BlockInfo read_block_from_disk(FileInfo file, string db_name, int blocknum, BlockInfo tmpblock)
{
	if (!tmpblock)//û�ṩ�飬����һ��
	{
		tmpblock = findBlock();
		tmpblock->blockNum = blocknum;
		tmpblock->isfree = 0;
	}
	//���ļ�
	string filepath = rootdir + db_name;
	if (file->type == 0)	//��
		filepath += "\\table\\";
	else					//����
		filepath += "\\index\\";
	filepath += file->fileName + ".txt";

	ifstream infile;
	infile.open(filepath);

	if (!infile.is_open())
	{
		cout << "readblock::no such file!" + filepath << endl;
		return nullptr;
	}

	//��ȡ����
	int offset = BLOCK_SIZE * (blocknum);
	infile.seekg(offset, ios::beg);
	if(!tmpblock->cBlock)
		tmpblock->cBlock = new char[BLOCK_SIZE];
	infile.read(tmpblock->cBlock,BLOCK_SIZE);
	infile.close();
	return tmpblock;
}

//��һ����д������
void write_block_to_disk(string db_name, BlockInfo block)
{
	if (!block->dirtyBit || block->isfree || block->charNum == 0)
		return;

	FileInfo file = block->file;
	if (!file)
	{
		cout << "write_block_to_disk::No such file opened!" << endl;
		return;
	}

	//���ļ�
	string filepath = rootdir + db_name;
	if (file->type == 0)	//��
		filepath += "\\table\\";
	else					//����
		filepath += "\\index\\";
	filepath += file->fileName + ".txt";

	//��ȡ����
	int offset = BLOCK_SIZE * (block->blockNum);
	ofstream ofile;
	ofile.open(filepath);
	ofile.seekp(offset, ios::beg);
	string str(block->cBlock);
	ofile.write(block->cBlock, str.length());
	ofile.close();

}

//���ļ��ж�ȡĳ����
BlockInfo get_block_info(string db_name, string table_name, int file_type, int BlockNum)
{
	
	FileInfo fileptr = get_file_info(table_name, file_type);
	if (!fileptr)
	{
		cout << "get_block_info::No such file!" << endl;
		return nullptr;//û�ҵ���Ӧ�ļ�
	}
	

	BlockInfo blockptr = get_block_in_file(fileptr, BlockNum);
	
	if (blockptr && blockptr->lock)//���Ƿ���ס
	{
		cout << "get_block_info::The block has been locked" << endl;
		return blockptr;
	}

	if (blockptr)//�ҵ���blocknum�Ŀ�
	{
		if (!blockptr->isfree)//���ǿտ飬ֱ�ӷ���
		{
			blockptr->call_times++;
			return blockptr;
		}
		else//�ǿտ飬�ȴӴ����ж�ȡ�����ٷ���
		{
			blockptr = read_block_from_disk(fileptr, db_name, BlockNum, blockptr);
			blockptr->call_times = 1;
			return blockptr;
		}
	}


	//�ļ���buffer��û�б��Ϊblocknum�Ŀ�
	cout << "get_block_info::can't find this block in buffer" << endl;
	blockptr = read_block_from_disk(fileptr, db_name, BlockNum, nullptr);
	blockptr->call_times = 1;
	return blockptr;
}

//�ر��ļ�
void closefile(string db_name, FileInfo file)
{
	if (file == nullptr)
	{
		cout << "closefile::No such file!" << endl;
		return;
	}
	BlockInfo blockptr = file->firstBlock;
	BlockInfo tmpblock = blockptr;
	fileNum--;
	while (blockptr)
	{
		write_block_to_disk(db_name, blockptr);
		tmpblock = blockptr;
		blockptr = blockptr->next;
		delete[] tmpblock->cBlock;
		delete tmpblock;
	}

	//���ļ�����ɾ��
	FileInfo fileptr = filelist;
	if (fileptr == file)//filelistͷ���
	{
		filelist = file->next;
		delete file;
		return;
	}

	while (fileptr)
	{
		if (fileptr->next == file)
		{
			fileptr->next = file->next;
			delete file;
			return;
		}
		fileptr = fileptr->next;
	}
}

//��һ���ļ�����filelist
FileInfo add_file_to_list(string table_name, int type, int recordAmount, int recordlen)
{
	if (fileNum >= MAX_ACTIVE_FILE)
	{
		cout << "Cannot add more file to list" << endl;
	}
	FileInfo newfile = new struct fileInfo;
	newfile->next = filelist;
	filelist = newfile;

	newfile->fileName = table_name;
	newfile->firstBlock = nullptr;
	newfile->freeNum = 0;
	newfile->recordAmount = recordAmount;
	newfile->recordLength = recordlen;
	newfile->type = type;

	fileNum++;
	return newfile;
}

//��סһ����
void lock_block(BlockInfo block)
{
	if (block == nullptr)
	{
		cout << "lock_block::Block doesn't exist" << endl;
		return;
	}
	block->lock = 1;
}

//����һ����
void unlock_block(BlockInfo block)
{
	if (block == nullptr)
	{
		cout << "unlock_block::Block doesn't exist" << endl;
		return;
	}
	block->lock = 0;
}

void write_to_block(BlockInfo block, char to_write[])
{
	block->charNum += strlen(to_write);
	strcpy_s(block->cBlock, BLOCK_SIZE, to_write);
}

