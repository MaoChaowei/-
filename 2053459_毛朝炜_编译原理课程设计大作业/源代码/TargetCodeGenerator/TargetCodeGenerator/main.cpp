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

/****默认分析代码文件的地址****/
string fileaddr = "../infiles/sourceProgram.txt";
string grammeraddr = "../infiles/grammer.txt";

int main()
{
	string in;
	cout << "/===============================================================/\n\n";
	cout << "-----------------类c编译器---BY-2053459-毛朝炜-------------------\n\n";
	cout << "/===============================================================/\n\n";
	cout << "+-----欢迎使用，本次编译的“文法”、“类c程序”默认配置如下-----+\n\n";
	cout << "文法txt文件：" << grammeraddr<<endl;
	cout << "类ctxt文件：" << fileaddr<<endl<<endl;
	cout << "是否需要更换测试文件？(Y/N)";

	while (1) {
		cin >> in;
		if (in == "Y" || in == "y") {
			cout << "请输入文法文件所在绝对地址：";
			cin >> grammeraddr;
			cout << "请输入类c程序所在绝对地址：";
			cin >> fileaddr;
			break;
		}
		else if (in == "N" || in == "n")
			break;
		else
			in.clear();
	}
	cout << "\n+---------------收到，下面开始本次编译，请耐心等待--------------+\n";
	cout << "~~编译过程将同步在命令行\n  您能在项目的outfile文件夹下找到各阶段的最终结果~~\n\n";
	cout << "/*****************STEP1 LR1 Table Generating*******************/" << endl;
	cout << "正在分析文法..." << endl;
	LR1_grammer g;
	g.grammer.readGrammer(grammeraddr);        // 读类C语言文法文法
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

	ThreadedList* Root = grammaticalAnalysis(fileaddr,LR1Table, productionItems, semantic);
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