#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include "1_Lexical_PreProcess.h"

using namespace std;

//LR1��һ����Ŀ
class LR1_item {
public:
	string left_symbol;				//����ʽ��ߵ�symbol
	vector<string> right_symbol;	//����ʽ�ұߵ�symbol
	int index;						// ��Ӧ����ʽ�ı��
	int dotPos;					    //���λ��
	string seek_symbol;				//ǰ��������һ������

	bool operator==(const LR1_item& item);
	bool operator<(const LR1_item& item) const;	//set��find��count��֧�֣�const�ؼ��ֱز�����
};

//LR1�ķ�
class LR1_grammer {
public:
	vector<LR1_item> allItems;				// ������Ŀ
	vector<set<LR1_item>>closureFamily;		// ��Ŀ����
	map<string, set<string>>FIRST;			// FIRST��
	map<pair<int, string>, pair<string, int>> LR1_table;	// LR1������
	CGrammer grammer;						// �ķ�

	void printItems(set<LR1_item> inItems);//��ӡ��Ŀ

	void generateFirst();								// �������ķ����ŵ�FIRST��
	void printFirst();									// ��ӡfirst��
	set<string> getStrFirstSet(vector<string> str);		// ��һ���ķ����Ŵ���FIRST��
	
	void getClosure(set<LR1_item> inItems, set<LR1_item>& outItems); // ��հ�
	bool isSameClosure(set<LR1_item>closure1, set<LR1_item>closure2); // �ж��Ƿ�Ϊ������ͬ�ıհ�

	
	void generateClosureFamily();	// ������Ŀ����
	// ������һ����Ŀ����״̬��
	void generateNextClosure(set<LR1_item> fromItems, set<LR1_item>& toItems, string presentStr);

	void fillGuiYueTable();//�����Ŀ���󣬵��øú������Լ��Ŀ
	void printLR1Table(string filename);
};


