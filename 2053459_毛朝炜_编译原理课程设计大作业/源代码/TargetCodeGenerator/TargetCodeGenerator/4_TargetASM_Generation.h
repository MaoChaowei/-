#pragma once
#include"3_Semantic_Analysis.h"
#include<map>
#include<set>
using namespace std;

# define REGNUM 32					//�Ĵ�������
/*-----mipsջ���-----*/
/*�ο�unix v6++�ж�ջ�Ĵ���
  ջ��$fp ջ��$sp,
  ջ�Ӹߵ�ַ��͵�ַ����
  ���¶��ϣ������������ص�ַ$ra,old fs��fsָ��,�ֲ�������
*/
#define PARAM_TO_FP 8					//�׸����������FPƫ��ֵ
#define LOCALVAR_TO_FP -4				//�׸��ֲ��������FPƫ��ֵ
#define DATA_ADDR  0x10010000		//���ݶ���ʼ��ַ
#define STACK_ADDR 0x10040000		//��ջ�ε���ʼ��ַ


//���ģʽ
#define CMD_MODE 1
#define FILE_MODE 2
#define BOTH_MODE 3

//�Ĵ����࣬��¼���Ĵ����Ļ�����Ϣ������MIPS��׼����
struct Reg {
	int idx;								//�Ĵ������
	string name;							//����
	int ftime;								//���е�ʱ��->LRU�㷨
	bool operator < (const Reg& b) {	
		return this->ftime < b.ftime;
	}
};

//Ŀ�����������
//��װĿ�����������ص����ݽṹ�ͷ���
class GenASM {
public:
	vector<Quatenary> quatenary;						//��Ԫʽ
	vector<string> asmCode;								//Ŀ�����

	vector<Reg> regInfo;								//��¼���Ĵ���״̬
	string RVALUE[REGNUM];								//�Ĵ���ʹ�����
	map<string, set<string>> AVALUE;					//��ǰ�������ڼĴ���������
	
	map<string, int> localVarTable;						//�ֲ����ű��ֲ��������β�ͳһ��������¼ƫ���� ����$sp���ʣ�ջ��͵�ַ����
	map<string, int> globalVarTable;					//ȫ�ַ��ű�
	set<string> labels;									//�洢��Ҫ��label����Ԫʽ���

	string procedureLable = "";							//��ǰ���̿������
	int param_to_fp= PARAM_TO_FP;						//�׸����������FPƫ��ֵ
	int localvar_to_fp = LOCALVAR_TO_FP;				//�׸��ֲ��������FPƫ��ֵ
	int globalvar_to_data = DATA_ADDR;

//��Ա����
	//���캯��
	GenASM(vector<Quatenary> quatenary);
	//Ŀ�����������ں���
	void TargetCodeGen();
	//����Ԫʽ�ľ���ASM��������
	void SingleStepGen(Quatenary quat);
	//����Ĵ���
	int AllocateReg(string var);
	//������δʹ�õļĴ���
	int GetLRU();
	int getRegIdx(string reg);
	//д���ļ����ӡ
	void print(int mode, string file_path);
	void AddNote(Quatenary quat);

};