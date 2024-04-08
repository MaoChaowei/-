#pragma once
#include"3_Semantic_Analysis.h"
#include<map>
#include<set>
using namespace std;

# define REGNUM 32					//寄存器数量
/*-----mips栈设计-----*/
/*参考unix v6++中对栈的处理：
  栈底$fp 栈顶$sp,
  栈从高地址向低地址生长
  自下而上：参数区，返回地址$ra,old fs（fs指向）,局部变量区
*/
#define PARAM_TO_FP 8					//首个参数相对于FP偏移值
#define LOCALVAR_TO_FP -4				//首个局部变量相对FP偏移值
#define DATA_ADDR  0x10010000		//数据段起始地址
#define STACK_ADDR 0x10040000		//堆栈段的起始地址


//输出模式
#define CMD_MODE 1
#define FILE_MODE 2
#define BOTH_MODE 3

//寄存器类，记录各寄存器的基本信息，按照MIPS标准定义
struct Reg {
	int idx;								//寄存器标号
	string name;							//名称
	int ftime;								//空闲的时间->LRU算法
	bool operator < (const Reg& b) {	
		return this->ftime < b.ftime;
	}
};

//目标代码生成类
//封装目标代码生成相关的数据结构和方法
class GenASM {
public:
	vector<Quatenary> quatenary;						//四元式
	vector<string> asmCode;								//目标代码

	vector<Reg> regInfo;								//记录各寄存器状态
	string RVALUE[REGNUM];								//寄存器使用情况
	map<string, set<string>> AVALUE;					//当前变量所在寄存器的名称
	
	map<string, int> localVarTable;						//局部符号表（局部变量和形参统一管理），记录偏移量 利用$sp访问，栈向低地址生长
	map<string, int> globalVarTable;					//全局符号表
	set<string> labels;									//存储需要加label的四元式序号

	string procedureLable = "";							//当前过程块的名字
	int param_to_fp= PARAM_TO_FP;						//首个参数相对于FP偏移值
	int localvar_to_fp = LOCALVAR_TO_FP;				//首个局部变量相对FP偏移值
	int globalvar_to_data = DATA_ADDR;

//成员函数
	//构造函数
	GenASM(vector<Quatenary> quatenary);
	//目标代码生成入口函数
	void TargetCodeGen();
	//各四元式的具体ASM代码生成
	void SingleStepGen(Quatenary quat);
	//分配寄存器
	int AllocateReg(string var);
	//最近最久未使用的寄存器
	int GetLRU();
	int getRegIdx(string reg);
	//写入文件或打印
	void print(int mode, string file_path);
	void AddNote(Quatenary quat);

};