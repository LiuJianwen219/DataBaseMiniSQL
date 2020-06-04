#ifndef __BASE_H__
#define __BASE_H__

#include <iostream>
#include <vector>
#include <iomanip>
#include <cassert>
#include "Element.h"
#include "buffer.h"
#include "Table.h"

//enum Type { INT, CHAR, FLOAT };/*��������*/

//����where���ж�
typedef enum {
	LESS,
	LESS_OR_EQUAL,
	EQUAL,
	GREATER_OR_EQUAL,
	GREATER,
	NOT_EQUAL
} Operator;

class SelectCondition {
public:
	int attributeIndex;//��ǰ������Եڼ����ֶΣ�Լ����0��ʼ
	Operator opt;
	Element value;//����><=������ֻ��Ҫ�õ�value
};

//Ԫ��
class Tuple {
private:
	std::vector<Element> data;
	bool isDeleted_;
	int index;//��ǰ���Ԫ�飬�ڱ��еĵڼ��У���0��ʼ

public:
	Tuple() : isDeleted_(false) {};
	Tuple(int elementNum) {
		Tuple();
		data.resize(elementNum);
	}

	//��������
	Tuple(const Tuple& copytuple) {
		data = copytuple.data; 
	//	attr = copytuple.attr; 
	}

	Tuple(const std::vector<Element>& tuples) {
		for (auto it : tuples) {
			push_back_Data(it);
		}
	}
	bool operator==(const Tuple& t) {
		if (t.data.size() != data.size()) {
			return false;
		}
		for (auto i = 0; i < data.size(); i++) {
			if (data[i] != t.data[i]) return false;
		}
		return true;
	}

	//����Ԫ��
	//������������insert����ʱ��˳��һ��һ����data������
	void push_back_Data(const Element& d) { this->data.push_back(d); }

	//��ӡһ��Ԫ������ݣ���˳��������м�ļ���������
	void Printdata(const std::vector<int>& width) {
		int i = 0;
		for (auto it = data.begin(); it != data.end(); it++, i++) {
			std::cout << "|";
			if ((*it).type == INT)
				std::cout << std::left << std::setw(width[i]) << (*it).m_int;
			else if ((*it).type == FLOAT)
				std::cout << std::left << std::setw(width[i]) << (*it).m_float;
			else
				std::cout << std::left << std::setw(width[i]) << (*it).m_char;
		}
		std::cout << "|\n";
	}
	int getWidth(int i) {
		assert(i >= 0 && i < data.size());
		if (data[i].type == INT) {
			auto item = std::to_string(data[i].m_int);
			return item.size();
		}
		else if (data[i].type == FLOAT) {
			auto item = std::to_string(data[i].m_float);
			return item.size();
		}
		else {
			assert(false);
		}
	}

	//��������
	std::vector<Element> getData() const { return this->data; }
	bool getIsDeleted() { return isDeleted_; }
	void setIsDeleted(bool s) { isDeleted_ = s; }
	int getIndex() { return index; }
	void setIndex(int i) { index = i; }
};

/*�������ݿ�*/
class DataBase {
public:
	std::string dataBaseName;
	std::vector<Table> tables;//�洢��ǰ���ݿ������е����ݱ�
	DataBase(std::string name) :dataBaseName(name) {}
};

/*�������ݱ�---����catalog�е�Table����*/
/*class Table {
public:
	std::string tableName;				//����
	std::vector<TableAttr> tableAttrs;	//һ�����ж������
	//int attrNum;						//������
	int primaryKeyIndex;				//���ݱ��е�����
	std::string primaryKey;				//��������
	//vector<BlockInfo> data;			//���ݱ��е�����
	int blockNum;						//ռ�õ�block��
	int recordNum;						//��¼����
	int recordLength;					//ÿ����¼�ĳ���

	Table() : blockNum(0), recordNum(0), recordLength(0) {}
	Table(std::string name, std::string primaryKey) : tableName(name), primaryKey(primaryKey) {}
	~Table() {}
};*/

/*�������ĳһ������---����catalog�е�Table����*/
/*class TableAttr {
public:
	std::string attrName;//������
	Type type;//��������
};*/

/*BlockInfo��FileInfoΪ����buffer���ඨ��*/

/*class FileInfo {
public:
	int type;
	std::string fileName;
	int recordAmout;
	int freeNum;
	int recordLength;
	FileInfo* next;
	BlockInfo* first;
};

class BlockInfo {
public:
	int blockNum;
	bool dirtyBit;
	BlockInfo* next;
	FileInfo file;
	int charNum;
	char* cBlock;
	int call_times;
	int lock;
	bool isfree;
};*/

#endif // BASE