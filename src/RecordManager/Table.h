#ifndef _TABLE_H_
#define _TABLE_H_
	#include<iostream>
	#include<string>
	#include<vector>
	#include"attribute.h"
	#include"Index.h"
	#include"buffer.h"

	using namespace std;
	class Table {
	public:
		string tableName;
		string primaryKey;
		int primaryKeyIndex;
		vector<Attribute> attributeVector;
		vector<Index>	indexVector;
		//vector<Tuple>   tuples;
		//char* data; // �����������е�����
		//FileInfo file;
		int indexNum;
		int attributeNum;
		int rowNum; // ��ʾ�ܹ��ж�����������Ч
		int rowLength; // ��ʾÿ�����ݵĳ����Ƕ����ֽ�
		Table() { ; }
		Table(string tableName, string primaryKey, vector<Attribute> attributeVector) {
			this->tableName = tableName;
			this->primaryKey = primaryKey;
			//this->primaryKeyIndex = primaryKeyIndex;
			this->indexVector = vector<Index>();
			this->indexNum = 0;

			this->attributeVector = attributeVector;
			this->attributeNum = attributeVector.size();

			this->rowNum = 0;

			this->rowLength = sizeof(char);//��0���ֽ�������¼�����Ƿ�ɾ��
			this->rowLength += sizeof(int);//��1~4���ֽ�������¼�����Ƿ�ɾ��
			for (int i = 0; i < attributeVector.size(); i++) {
				if (attributeVector[i].attributeName == primaryKey) {
					attributeVector[i].isUnique = true;
					primaryKeyIndex = i;
				}
				this->rowLength += attributeVector[i].type.get_length();
			}
		}
		//Table(string tableName, string primaryKey, vector<Attribute> attributeVector, vector<Index> indexVector, int rowNum) {
		//	this->tableName = tableName;
		//	this->primaryKey = primaryKey;
		//	this->attributeVector = attributeVector;
		//	this->indexVector = indexVector;
		//	this->indexNum = indexVector.size();
		//	this->attributeVector = attributeVector;
		//	this->attributeNum = attributeVector.size();
		//	this->rowNum = rowNum;
		//	for (int i = 0; i < attributeVector.size(); i++) {
		//		this->rowLength += attributeVector[i].type.get_length();
		//	}
		//}

	private:

	};
#endif // !_TABLE_H_

