#ifndef __ELEMENT_H__
#define __ELEMENT_H__
#include "FieldType.h"
#include <string>

/*������һ��Ԫ�أ������������Լ����ݵĴ�ţ�ͨ��Ԫ����������ʵ�ֲ�ͬ�ıȽϷ�����*/
class Element {
public:
	NumType type;
	int m_int;
	float m_float;
	std::string m_char;
	int length; // ע�� type=CHAR ʱ������� length = ����ܱ����ִ�ת����+1����Ϊ���һ����'\0'
	// ���� length = 4����� INT �� FLOAT��

	bool operator<(Element &e);
	bool operator==(const Element &e);
	bool operator>(Element &e);
	bool operator>=(Element &e);
	bool operator<=(Element &e);
	bool operator!=(const Element &e);
	
	friend std::ostream& operator<<(std::ostream& out, Element &e);

	//Element(std::string s, int char_num);
	Element(std::string s, int l = 1) {
		type = CHAR;
		m_char = s;
		length = l;
	}
	Element(float f, int l = 4) {
		type = FLOAT;
		m_float = f;
		length = l;
	}
	Element(int i, int l = 4) {
		type = INT;
		m_int = i;
		length = l;
	}
	Element();
	~Element();

	void setType(NumType t) { type = t; }
	void setLength(int l) { length = l; }

	void elementToChar(char* c) {
		if (type == INT) {//int������4�ֽ�
			memcpy(c, &m_int, length);
		}
		else if (type == FLOAT) {//float������4�ֽ�
			memcpy(c, &m_float, length);
		}
		else {//��ܹ��洢length��'a'
			for (int i = 0; i < length; i++) {
				c[i] = m_char[i];
			}
		}
	}
	void charToElement(char* c) {
		if (type == INT) {//int������4�ֽ�
			memcpy(&m_int, c, length);
		}
		else if (type == FLOAT) {//float������4�ֽ�
			memcpy(&m_float, c, length);
		}
		else {//��ܹ��洢length��'a'
			m_char = string(c, length);//���Ǵ洢�Ŀռ���Ҫ+1
		}
	}
	void printElement() {
		if (type == INT) {//int������4�ֽ�
			cout << m_int;
		}
		else if (type == FLOAT) {//float������4�ֽ�
			cout << m_float;
		}
		else {//��ܹ��洢length��'a'
			cout << m_char;
		}
	}
};

#endif