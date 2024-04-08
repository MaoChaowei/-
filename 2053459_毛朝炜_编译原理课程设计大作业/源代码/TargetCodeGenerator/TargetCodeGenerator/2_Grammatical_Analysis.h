#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "1_Lexical_Analysis.h"
#include "3_Semantic_Analysis.h"
using namespace std;

struct productItem;
//struct Item {
//	string left_symbol;			  //����ʽ��ߵ�symbol
//	vector<string> right_symbol;  //����ʽ�ұߵ�symbol
//};

// ���߱��洢��ǰ�ڵ��ַ���ָ��
struct ThreadedList {
	string symbol;
	string IdIfExists;		// �����identifier���˴�ָʾ֮
	// ��Ϊ���ս������ǰ���ս��Ϊ��Լ�еڼ������ɵģ��ս����Ĭ��Ϊ0��
	// ע�ⲻ�Ǵ��߱�ġ����ӽ��������������ͨ��vector.size()�õ�
	int regulationStep;
	vector<ThreadedList*>threads;

	ThreadedList(int step = 0) { regulationStep = step; }
};

// ̫�������Էֳ���д
#define ANSWER_TYPE      TOKEN
#define TABLE_KEY_TYPE   pair<int, string>
#define TABLE_VALUE_TYPE pair<string, int>
#define ITEM_TYPE		 productItem

void generateTreeDot(ThreadedList* Root);
void deleteThreadedList(ThreadedList* Root);
ThreadedList* grammaticalAnalysis(string fileaddr, map<TABLE_KEY_TYPE, TABLE_VALUE_TYPE>& Table, vector<ITEM_TYPE>& Items, SemanticAnalysis& Semantic);