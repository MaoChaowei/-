#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

//������
#define FILE_OPEN_ERROE 1
#define LEXICAL_ERROR_UNDEFINED_WORD 2
#define GRAMMAR_ERROR 3
#define GRAMMATICAL_ERROR_UNDEFINED_WORD 4
#define GRAMMATICAL_ERROR_CANNOT_ANALYSIS 5
#define SEMANTIC_ERROR_NO_MAIN 6
#define SEMANTIC_ERROR_REDEFINED 7
#define SEMANTIC_ERROR_UNDEFINED 8
#define SEMANTIC_ERROR_PARAMETER_NUM 9
//#define SEMANTIC_ERROR_UNDEFINED 10
#define SEMANTIC_ERROR_NO_RETURN 11

//�����������
struct SemanticSymbol
{
	string token;//��������
	string value;//ֵ
	int row;//Դ�����е�λ��
	int col;
	int tableIdx;//�������ڱ������
	int idx;//�����ڱ��е�����
};

//��ʶ�������Ϣ��������������������ʱ�����������ľ�����Ϣ
struct IdentifierInfo
{
	//��ʶ������:��������������ʱ����������������ֵ
	enum idType { FUN, VAR, TMP, CST, RET };

	idType idType;//��ʶ��������
	string specifierType;//��(��)������/������������
	string identifierName;//��ʶ������/����ֵ
	int funcParameterNum;//������������
	int funcEntry;//������ڵ�ַ(��Ԫʽ�ı��)
	int funcTableIdx;//�����ĺ������ű�����������ķ��ű��б��е�����
};

//���ű�
struct SemanticSymbolTable
{
	//���ֱ�����ͣ��ֱ�Ϊȫ�ֱ��������鼶����ʱ��
	enum TableType { GlobalTable, FunctionTable, BlockTable, TempTable };

	TableType table_type;//�������
	vector<IdentifierInfo> table;//���ű�
	string table_name;//����

	//���캯��
	SemanticSymbolTable(const TableType type, const string name);
	//���ݱ�������Ѱ��һ������ û�ҵ��򷵻�-1�����򷵻ر����ڱ��еı��
	int FindSymbol(const string name);
	//����һ�����������ؼ����λ�ã���֮ǰ�Ѿ������򷵻�-1��
	int AddSymbol(const IdentifierInfo id);
};

//��Ԫʽ
struct Quatenary
{
	int idx;//��Ԫʽ���
	string operatorType;//��������
	string op1;//������1
	string op2;//������2
	string result;//���
};

//�������
class SemanticAnalysis
{
public:
	vector<Quatenary> quaternary;//��Ԫʽ
	int mainIdx;//main������Ӧ����Ԫʽ���
	int backpatchLevel;//������
	vector<int> backpatchList;//�����б�
	int nextQuaternaryIdx;//��һ����Ԫʽ���
	int tmpVarCount;//��ʱ��������
	vector<SemanticSymbol> symbolList;//����������̵ķ�����
	vector<SemanticSymbolTable> tables;//�������з��ű�
	vector<int> currentTableStack;//��ǰ�������Ӧ�ķ��ű�����ջ

	//���캯��
	SemanticAnalysis();
	//������Ϣ����symbol_list
	void AddSymbol2List(const SemanticSymbol symbol);
	//��������
	void Analysis(const string production_left, const vector<string> production_right);
	//��ӡ��Ԫʽ��
	void PrintQuaternary(const string file_path);

private:
	//�����������
	void Semantic_Analysis_Program(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_ExtDef(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_VarSpecifier(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_FunSpecifier(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_FunDec(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_CreateFunTable_m(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_ParamDec(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Block(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Def(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_AssignStmt(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Exp(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_AddSubExp(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Item(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Factor(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_CallStmt(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_CallFunCheck(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Args(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_ReturnStmt(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_Relop(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_IfStmt(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_IfStmt_m1(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_IfStmt_m2(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_IfNext(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_IfStmt_next(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_WhileStmt(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_WhileStmt_m1(const string production_left, const vector<string> production_right);
	void Semantic_Analysis_WhileStmt_m2(const string production_left, const vector<string> production_right);

};