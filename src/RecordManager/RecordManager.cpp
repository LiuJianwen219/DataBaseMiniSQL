#include "recordManager.h"

extern CatalogManager catalogManager;

/**
	 ���߼��ϡ���
	 1��recordManger�Ĳ���Ӧ���ǽ�����"��"��������ϵġ�Ҳ���ǶԱ���в�����
	 2������word�汾�ġ�Record Managerģ�����˵�������棬
	    ����catalog manger����ȡ��������¼��������Ϣ��������¼�Ŀ�ţ�������Buffer Manager �Ĺ��ܡ���
	 3����catalog manager�����Ѿ��Ѵӱ����õ���ĺ���д���ˣ�ֱ�ӵ��þ����жϲ�Ϊ�ռ��ɡ�����Ҫ�����жϱ�Ĵ������
	 4����������ΪӦ�ð�buffer�����FileInfo��ΪTable�������һ����Ա��Ҫ�õ����ݵ�ʱ����ȥ��buffer����õ���
	 5����лbufferͬѧ��������ֱ�Ӵ�tableName��index�о͵õ�������Ҫ�����ݡ�
 */


//TODO:��ѯʱ������Ҫ��������������δ֪���ӿ�

//�Ƿ�Ӧ����buffer�е�filelist��ϵ����
//Table* RecordManager::isExistTable(string tableName)
//{
//	int n = tables.size();
//	for (int i = 0; i < n; i++) {
//		if (tables[i].tableName == tableName) {
//			return &(tables[i]);
//		}
//	}
//	return NULL;
//}

////TODO:createTable �� dropTable��catalogManager��Ҳ���ڣ����߿�����Ҫ��һ��ͳһ��ֱ�Ӳ���CatalogManager�еĺ���
//bool RecordManager::createTable(string tableName, string primaryKey, vector<Attribute> attributeVector)
//{
//	if (isExistTable(tableName) != NULL) {
//		cerr << "ͬ���ı���Ѿ�����" << endl;
//		return false;
//	}
//	Table table(tableName, primaryKey, attributeVector);
//	this->tables.push_back(table);
//	return true;
//}

//bool RecordManager::dropTable(string tableName)
//{
//	Table* table = isExistTable(tableName);
//	if (table == NULL) {
//		cerr << "�����ڱ��" << tableName << endl;
//		return false;
//	}
//	for (auto it = this->tables.begin(); it != this->tables.end(); ++it) {
//		if (it->tableName == tableName) {
//			this->tables.erase(it);
//			return true;
//		}
//	}
//	return false;
//}

//insertValue����ֵΪfalse��ʾ��������Ϊ��ȷִ��
bool RecordManager::insertValue(string tableName, Tuple tuple)
{
	Table table = catalogManager.get_table(tableName);//�������������ķ���ֵ����ɷ���ָ��
	//if (table == NULL) {// �����޷��ж��Ƿ��������
	//	cerr << "û�������" << endl;
	//	return false;
	//}
	if (isMatchTheAttribute(&table, &tuple) == false) {
		cerr << "Tuple attributes do not match the table" << endl;
		return false;
	}

	if (isConflictTheUnique(&table, &tuple) == true) {
		cerr << "the primary key conflict" << endl;
		return false;
	}

	char* tmpChar = (char*)malloc((table.rowLength + 2) * sizeof(char));
	tupleToChar(tuple, tmpChar);
	bool writeResult = writeToIndex(table.tableName, table.rowNum, tmpChar, table.rowLength, "db_name?", 0);
	free(tmpChar);
	if (writeResult == false) {
		cerr << "д��bufferʧ��" << endl;
		return false;
	}
	else {
		cout << "д��buffer�ɹ�" << endl;
		table.rowNum += 1;
	}
	return true;
}

bool RecordManager::isMatchTheAttribute(Table* table, Tuple* tuple)
{
	std::vector<Element> data = tuple->getData();
	if (data.size() != table->attributeNum) {
		cerr << "Attribute number not match!" << endl;
		return false;
	}
	for (int i = 0; i < data.size(); ++i) {
		if (data[i].type != table->attributeVector[i].type.get_type()) {
			cerr << "Attribute type not match!" << endl;
			return false;
		}
	}
	return true;
}

bool RecordManager::isConflictTheUnique(Table* table, Tuple* tuple)
{
	for (int i = 0; i < table->rowNum; ++i) {
		Tuple tmpTuple = getTupleByIndex(table, i);
		if (isConflict(&tmpTuple, tuple, table->primaryKeyIndex) == true) {
			cerr << "primary key conflict" << endl;
			return true;//��Ϊ�����˳�ͻ
		}
	}
	return false;
}

bool RecordManager::isConflict(Tuple* tuple1, Tuple* tuple2, int index)
{
	if (tuple1->getIsDeleted() || tuple2->getIsDeleted()) {//������һ��Ԫ�鱻ɾ�����򲻿��ܷ�����ͻ
		return false;
	}
	return tuple1->getData()[index] == tuple2->getData[index];
}

Tuple RecordManager::getTupleByIndex(Table* table, int index)
{
	Tuple tuple;
	char* tmpData = (char*)malloc((table->rowLength + 2) * sizeof(char));
	if (tmpData == NULL) {
		cerr << "can't get memory in isConflictTheUnique()" << endl;
		return tuple;
	}
	int tmpLength = table->rowLength;
	// ��֪��db_name��ʲô����
	readData(table->tableName, "db_name?", index * table->rowLength, tmpData, tmpLength, 0);
	Tuple tmpTuple = charToTuple(table, tmpData);
	free(tmpData);
	return tuple;
}

//�����������������߶��������and���͵Ĳ�ѯ������or�ǲ��еģ�
vector<Tuple> RecordManager::searchQuery(string tableName, vector<SelectCondition> selectConditions)
{
	vector<Tuple> ans;
	Table table = catalogManager.get_table(tableName);//�������������ķ���ֵ����ɷ���ָ��
	//if (table == NULL) {// �����޷��ж��Ƿ��������
	//	cerr << "û�������" << endl;
	//	return ans;
	//}
	for (int i = 0; i < table.rowNum; i++) {
		Tuple tmp = getTupleByIndex(&table, i);
		if (tmp.getIsDeleted()) continue;
		bool valid = true;
		for (auto it = selectConditions.begin(); it != selectConditions.end(); ++it) {
			if (judgeCondition(tmp, *it) == false) {
				valid = false;
				break;
			}
		}
		if (valid) {
			ans.push_back(tmp);
		}
	}
	return ans;
}
//A<10 or B<10
vector<Tuple> RecordManager::searchQuery(string tableName)
{
	vector<Tuple> ans;
	Table table = catalogManager.get_table(tableName);//�������������ķ���ֵ����ɷ���ָ��
	//if (table == NULL) {// �����޷��ж��Ƿ��������
	//	cerr << "û�������" << endl;
	//	return ans;
	//}
	for (int i = 0; i < table.rowNum; i++) {
		Tuple tmp = getTupleByIndex(&table, i);
		if (tmp.getIsDeleted()) continue;
		ans.push_back(tmp);
	}
	return ans;
}

bool RecordManager::judgeCondition(Tuple tuple, SelectCondition condition)
{
	if (tuple.getIsDeleted() == true) {
		return false;//A<10       B<10
	}
	else if (condition.opt == LESS) {
		if (tuple.getData()[condition.attributeIndex] < condition.value)
			return true;
	}
	else if (condition.opt == LESS_OR_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] <= condition.value)
			return true;
	}
	else if (condition.opt == EQUAL) {
		if (tuple.getData()[condition.attributeIndex] == condition.value)
			return true;
	}
	else if (condition.opt == GREATER_OR_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] >= condition.value)
			return true;
	}
	else if (condition.opt == GREATER) {
		if (tuple.getData()[condition.attributeIndex] > condition.value)
			return true;
	}
	else if (condition.opt == NOT_EQUAL) {
		if (tuple.getData()[condition.attributeIndex] != condition.value)
			return true;
	}

	//ɾ�����е�����Ƚϣ���Ҫ����ģ�飬������������Ĳ�������Ϊ�����Ƚϲ�����
	//����and �Ļ�������������Ų�����
	//else if (condition.opt == INTERVAL) {
	//	if (condition.value.first <= tuple.data[condition.attributeIndex] && tuple.data[condition.attributeIndex] < condition.value.second)
	//		return true;
	//}
	return false;
}



int RecordManager::deleteTuple(string tableName, vector<SelectCondition> selectConditions)
{
	Table table = catalogManager.get_table(tableName);//�������������ķ���ֵ����ɷ���ָ��
	//if (table == NULL) {// �����޷��ж��Ƿ��������
	//	cerr << "û�������" << endl;
	//	return ans;
	//}
	vector<Tuple> searchResult = searchQuery(tableName, selectConditions);
	if (searchResult.size() == 0) {
		cerr << "no tuple need to be deleted, please check the conditions" << endl;
		return 0;
	}
	int deleteNum = searchResult.size();
	for (int i = 0; i < deleteNum; i++) {
		Tuple tuple = searchResult[i];
		char* tmpChar = (char*)malloc(sizeof(char));
		*tmpChar = '0';
		bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1, "db_name?", 0);//�޸���ǰ���ɾ�����
		free(tmpChar);
		if (writeResult == false) {
			cerr << "write to buffer error" << endl;
			return -1;
		}
		//table.rowNum -= 1;//���ǲ��޸������������
	}
	return deleteNum;
}

int RecordManager::deleteTuple(string tableName)
{
	Table table = catalogManager.get_table(tableName);//�������������ķ���ֵ����ɷ���ָ��
	//if (table == NULL) {// �����޷��ж��Ƿ��������
	//	cerr << "û�������" << endl;
	//	return ans;
	//}
	vector<Tuple> searchResult = searchQuery(tableName);
	if (searchResult.size() == 0) {
		cerr << "no tuple need to be deleted, please check the conditions" << endl;
		return 0;
	}
	int deleteNum = searchResult.size();
	for (int i = 0; i < deleteNum; i++) {
		Tuple tuple = searchResult[i];
		char* tmpChar = (char*)malloc(sizeof(char));
		*tmpChar = '0';
		bool writeResult = writeToIndex(tableName, tuple.getIndex(), tmpChar, 1, "db_name?", 0);//�޸���ǰ���ɾ�����
		free(tmpChar);
		if (writeResult == false) {
			cerr << "write to buffer error" << endl;
			return -1;
		}
		//table.rowNum -= 1;//���ǲ��޸������������
	}
	return deleteNum;
}
