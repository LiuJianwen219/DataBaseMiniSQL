#ifndef DEBUG
#define DEBUG
#include"buffer.h"
#endif // !DEBUG
#include<cstring>
using namespace std;


int main()
{
	string db_name("database");
	string filename1("table1");
	string filename2("table2");
	string filename3("table3");

	//��filelist�в���һ����Ϊfilename1��type=0������0����¼����¼��20���ļ�������������ļ���fileinfo
	//����ֻ��������ʹ�õ�һЩ�������Ҫ��catalog���
	//filelist��Ҫ���ڿ����ļ���������ֹͬʱ����̫���ļ�������Ϊ5��
	FileInfo file1 = add_file_to_list(filename1, 0, 0, 20);
	//����������������
	FileInfo file2 = add_file_to_list(filename2, 0, 0, 30);
	FileInfo file3 = add_file_to_list(filename2, 0, 0, 40);

	//��ȡfilelist�����е��ļ��������������
	//FileInfo file1 = get_file_info(filename1, 0);

	//����һ���տ�
	BlockInfo emptyblock = findBlock();

	//���д��ʲô
	char ss[30] = "abcdefghijklmnopqrstuvwxyz";

	//������տ�ŵ�file1����
	add_block_to_file(emptyblock, file1);
	emptyblock->dirtyBit = 1;
	emptyblock->isfree = 0;
	emptyblock->blockNum = 0;
	
	write_to_block(emptyblock, ss);

	closefile(db_name, file1);

	return 0;
}