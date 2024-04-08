#include <iostream>
#include <iomanip>
#include "1_Lexical_PreProcess.h"
#include "2_Grammatical_Analysis.h"
#include "2_Grammatical_LR1Grammer.h"
#include"3_Semantic_Analysis.h"
#include"4_TargetASM_Generation.h"

using namespace std;

extern void lexAnalysis(const string path);           // �ʷ������ӿں���
extern vector<TOKEN>lexAnswer;						  // 1���ʷ�����������
map<pair<int, string>, pair<string, int>> LR1Table;   // 2��LR1������
std::vector<productItem>productionItems;              // 3������ʽ
SemanticAnalysis semantic;							  // 4���������
					
string START_SIGN = "Program";// ԭ�ķ�����ʼ��

int main()
{
	cout << "/*****************STEP1 LR1 Table Generating*******************/" << endl;
	cout << "���ڷ����ķ�..." << endl;
	LR1_grammer g;
	g.grammer.readGrammer("../infiles/grammer.txt");        // ����C�����ķ��ķ�
	g.grammer.printGrammer("../outfiles/2_Grammatical_Grammer.txt");   // ��ӡ�ķ���������ļ���

	g.generateFirst();                                       // �������з��ŵ�first�� 
	// g.printFirst();											 // ��ӡfirst��

	g.generateClosureFamily();                               // ������Ŀ���壨�����л�����LR1��������ƽ����ֺ�GoTo���֣�
	g.fillGuiYueTable();								     // ��дLR1������Ĺ�Լ����

	LR1Table = g.LR1_table;
	productionItems = g.grammer.productItems;


	g.printLR1Table("../outfiles/2_Grammatical_LR1Table.txt");                // ��ӡLR1������
	cout << "LR1�����ɳɹ���" << endl;

	/***************************�ʷ����﷨���������****************************************/
	cout << "\n/*****************STEP2 Lexical&Grammatical Analysis*******************/" << endl;

	ThreadedList* Root = grammaticalAnalysis(LR1Table, productionItems, semantic);
	cout << "�﷨���������" << endl;
	if (!Root)
	{
		cout << "ERROR: REGULATION FAILED!" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "REGULATION SUCCEEDED!" << endl;
	}
	generateTreeDot(Root);            // �����﷨��


	cout << "\n/*****************STEP3 Semantic Analysis*******************/" << endl;
	semantic.PrintQuaternary("../outfiles/3_Semantic_Quaternary.txt");

	/********************************Ŀ���������************************************/
	cout << "\n/*****************STEP4 TargetCode Analysis*******************/" << endl;
	GenASM asmcode(semantic.quaternary);
	asmcode.TargetCodeGen();
	asmcode.print(BOTH_MODE, "../outfiles/4_TargetCode.txt");
	system("pause");
	return 0;
}