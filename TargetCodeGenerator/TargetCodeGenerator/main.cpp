#include <iostream>
#include <iomanip>
#include "1_Lexical_PreProcess.h"
#include "2_Grammatical_Analysis.h"
#include "2_Grammatical_LR1Grammer.h"
#include"3_Semantic_Analysis.h"
#include"4_TargetASM_Generation.h"

using namespace std;

extern void lexAnalysis(const string path);           // 词法分析接口函数
extern vector<TOKEN>lexAnswer;						  // 1、词法分析处理结果
map<pair<int, string>, pair<string, int>> LR1Table;   // 2、LR1分析表
std::vector<productItem>productionItems;              // 3、产生式
SemanticAnalysis semantic;							  // 4、语义分析
					
string START_SIGN = "Program";// 原文法的起始符

int main()
{
	cout << "/*****************STEP1 LR1 Table Generating*******************/" << endl;
	cout << "正在分析文法..." << endl;
	LR1_grammer g;
	g.grammer.readGrammer("../infiles/grammer.txt");        // 读类C语言文法文法
	g.grammer.printGrammer("../outfiles/2_Grammatical_Grammer.txt");   // 打印文法（输出到文件）

	g.generateFirst();                                       // 生成所有符号的first集 
	// g.printFirst();											 // 打印first集

	g.generateClosureFamily();                               // 生成项目集族（过程中会生成LR1分析表的移进部分和GoTo表部分）
	g.fillGuiYueTable();								     // 填写LR1分析表的归约部分

	LR1Table = g.LR1_table;
	productionItems = g.grammer.productItems;


	g.printLR1Table("../outfiles/2_Grammatical_LR1Table.txt");                // 打印LR1分析表
	cout << "LR1表生成成功！" << endl;

	/***************************词法、语法和语义分析****************************************/
	cout << "\n/*****************STEP2 Lexical&Grammatical Analysis*******************/" << endl;

	ThreadedList* Root = grammaticalAnalysis(LR1Table, productionItems, semantic);
	cout << "语法分析结果：" << endl;
	if (!Root)
	{
		cout << "ERROR: REGULATION FAILED!" << endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		cout << "REGULATION SUCCEEDED!" << endl;
	}
	generateTreeDot(Root);            // 绘制语法树


	cout << "\n/*****************STEP3 Semantic Analysis*******************/" << endl;
	semantic.PrintQuaternary("../outfiles/3_Semantic_Quaternary.txt");

	/********************************目标代码生成************************************/
	cout << "\n/*****************STEP4 TargetCode Analysis*******************/" << endl;
	GenASM asmcode(semantic.quaternary);
	asmcode.TargetCodeGen();
	asmcode.print(BOTH_MODE, "../outfiles/4_TargetCode.txt");
	system("pause");
	return 0;
}