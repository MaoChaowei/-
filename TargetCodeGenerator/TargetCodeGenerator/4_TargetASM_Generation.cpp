#include "4_TargetASM_Generation.h"
#include <fstream>
#include<ctype.h>

/* boundary
 先只考虑int型
 局部变量和参数传递只使用t0~t9
 还未考虑全局变量的实现，应当存在data段中，用全局符号表维护
 AVALUE管理了所有变量，支持变量的重名
*/


//构造函数
GenASM::GenASM(vector<Quatenary> quatenary)
{
	//存四元式
	this->quatenary = quatenary;
	//	因为处理(j,-,-,39)语句要在处理第39条四元式前加上标签，所以预先将要加标签的四元式序号都记录下来。
	int len = quatenary.size();
	for (int i = 0; i < len; i++) {
		if (quatenary[i].operatorType[0] == 'j')
			labels.insert(quatenary[i].result);
	}
	
	vector<string> registers = {							//MIPS共32个寄存器
		"$zero",											//$0 常量0(constant value 0)
		"$at",												//$1 保留给汇编器(Reserved for assembler)
		"$v0","$v1",										//$2-$3 函数调用返回值(values for results and expression evaluation)
		"$a0","$a1","$a2","$a3",							//$4-$7 函数调用参数(arguments)
		"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",	//$8-$15 暂时的(或随便用的)
		"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",	//$16-$23 保存的(或如果用，需要SAVE/RESTORE的)(saved)
		"$t8","$t9",										//$24-$25 暂时的(或随便用的)
		"$k0","k1",											//操作系统／异常处理保留，至少要预留一个 
		"$gp",												//$28 全局指针(Global Pointer)
		"$sp",												//$29 堆栈指针(Stack Pointer)
		"$fp",												//$30 帧指针(Frame Pointer)
		"$ra"												//$31 返回地址(return address)
	};
	//初始化寄存器
	for (int i = 0; i < REGNUM; i++) {
		Reg temp = { i,registers[i] ,0 };
		this->regInfo.push_back(temp);
	}
	//初始化寄存器使用状态RVALUE
	RVALUE[0] = "0";//$0 常量0(constant value 0)
	for (int i = 0; i < REGNUM; i++) {
		RVALUE[i] = "null";	//初始为空
	}
	//为立即数准备一个位置，便于管理为常数分配的寄存器
	AVALUE.insert({ "123imm",{} });

}

//目标代码生成
void GenASM::TargetCodeGen()
{
	//初始化栈结构
	asmCode.push_back("addi $sp,$zero,"+to_string(STACK_ADDR));
	asmCode.push_back("addi $fp,$zero,"+to_string(STACK_ADDR+4));//!!!!!
	//遍历各四元式
	int len = quatenary.size();
	for (int i = 0; i < len; i++) {
		//ftime更新 用了则置为0，否则在每次分析前自增
		for (vector<Reg>::iterator iter = regInfo.begin(); iter != regInfo.end(); iter++)
				iter->ftime++;
		
		//目标代码生成
		SingleStepGen(quatenary[i]);
	}
	return;
}


//各四元式的具体ASM代码生成
void GenASM::SingleStepGen(Quatenary quat)
{
	//添加汇编注释
	AddNote(quat);

	//特殊处理--j、j>...跳转类：延后加标签
	if (labels.find(to_string(quat.idx)) != labels.end()) {
		string str = "Label_" + to_string(quat.idx) + ":";
		asmCode.push_back(str);
	}

	//(program,-,-,-)...过程声明类：建立新栈帧栈的结构
	if (quat.op1 == "-" && quat.op2 == "-" && quat.result == "-") {
		//maintain
		//获得一个干净的局部变量表，仅在本块中使用，故直接刷新即可。
		localVarTable.clear();
		procedureLable = quat.operatorType;//设置过程名
		param_to_fp = PARAM_TO_FP;//设置栈结构
		localvar_to_fp = LOCALVAR_TO_FP;

		//generate code:
		//1\  标签
		string str = quat.operatorType + ":";
		asmCode.push_back(str);
		//2\  存ra
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw $ra,0($sp)");
		//3\   存fp、把fp做为当前栈帧的栈底
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw $fp,0($sp)");
		asmCode.push_back("move $fp,$sp");
	}
	//(call,demo,-,T10)...过程调用：跳转
	else if (quat.operatorType == "call") {
		//保存现场:将所有局部变量最近使用的寄存器值（newest）写回到内存
		int maxtime = 1000000;
		string save_reg = "";
		for (auto iter : localVarTable) {
			if (!AVALUE.at(iter.first).empty()) {
				for (auto p : AVALUE.at(iter.first)) {
					int idx = getRegIdx(p);
					if (regInfo[idx].ftime < maxtime) {
						maxtime = regInfo[getRegIdx(p)].ftime;
						save_reg = p;
					}
				}
				asmCode.push_back("sw " + save_reg + ","+to_string(iter.second)+"($fp)");
				maxtime = 1000000;
			}
		}
		//调用jal跳转
		asmCode.push_back("jal " + quat.op1);
		//恢复现场：将所有局部变量内存中的值写回到该局部变量占用的所有寄存器中
		for (auto iter : localVarTable) {
			if (!AVALUE.at(iter.first).empty()) {
				for (auto r : AVALUE.at(iter.first)) {
					asmCode.push_back("lw " + r + "," + to_string(iter.second) + "($fp)");
					int idx = getRegIdx(r);
					RVALUE[idx] = iter.first;
				}
			}
		}
		//存结果,注意只支持返回一个四字节的结果，存在v1中。
		int ans_idx = AllocateReg(quat.result);
		asmCode.push_back("move " + regInfo[ans_idx].name + ",$v1");
	}

	//(return,main_return_value,-,main)...函数返回：取得返回值，撤栈，跳转回
	else if (quat.operatorType == "return") {
		//1、有返回值 则存返回值->v1
		if (quat.op1 != "-") {
			string ans = *(AVALUE.at(quat.op1).begin());
			asmCode.push_back("move $v1," + ans);
		}
		//2、恢复AVALUE和RVALUE：释放局部变量占用的寄存器
		for (auto it = localVarTable.begin(); it != localVarTable.end(); it++) {
			auto p = AVALUE.find(it->first);
			if (p != AVALUE.end()) {//虽然判断 但局部变量应当加入了AVALUE中
				for (auto Preg = p->second.begin(); Preg != p->second.end(); Preg++) {
					for (int i = 0; i < REGNUM; i++) {
						if (regInfo[i].name == *Preg)
							RVALUE[i] = "null";
					}
				}
				AVALUE.erase(it->first);
			}
		}

		//3、撤栈
		//sp=fp，sp抹掉参数区域,fp=old fp
		asmCode.push_back("move $sp,$fp");//清局变区域
		asmCode.push_back("addi $sp,$sp," + to_string(param_to_fp));//清参数区域
		asmCode.push_back("lw $ra,4($fp)");//取出返回地址
		asmCode.push_back("lw $fp,0($fp)");//回到old fp：完成撤栈
		//4、jr跳转 注意：本身是main函数就不跳转了，直接结束（可以加上结束的汇编指令）。
		if (procedureLable != "main")
			asmCode.push_back("jr $ra");
		//5、恢复相关属性变量(步骤名 栈初始结构 局部变量符号表)
		procedureLable = "";
		localVarTable.clear();
		param_to_fp = PARAM_TO_FP;
		localvar_to_fp = LOCALVAR_TO_FP;
	}

	//(defpar,-,-,a)...形参声明：将参数加入到局部变量表中，配合param四元式完成参数传递。
	else if (quat.operatorType == "defpar") {
		localVarTable.insert({ quat.result,param_to_fp });
		param_to_fp += 4;
		auto param = pair<string, set<string>>(quat.result, {});
		AVALUE.insert(param);
	}
	//(param,5,-,-)...实参声明：把参数压入当前栈帧栈顶
	else if (quat.operatorType == "param") {
		//参数是数字、变量统一处理
		//取得该参数的寄存器/或分配之
		int reg_idx = AllocateReg(quat.op1);
		//压入下一帧的参数区
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw " + regInfo[reg_idx].name + ",0($sp)");
	}
	//(j,-,-,7)...跳转类：
	else if (quat.operatorType == "j") {
		asmCode.push_back("j Label_" + quat.result);
	}

	//(=,0,-,T6)...赋值类：
	else if (quat.operatorType == "=") {
		//获取或分配目标变量的寄存器号
		int regidx = AllocateReg(quat.result);
		//立即数赋值
		if (isdigit(quat.op1[0])) {
			asmCode.push_back("addi " + regInfo[regidx].name + ",$zero," + quat.op1);
		}
		//变量赋值
		else {
			//获取或分配右值变量的寄存器号
			int rightidx = AllocateReg(quat.op1);
			asmCode.push_back("move " + regInfo[regidx].name + "," + regInfo[rightidx].name);
		}
	}

	//四类运算 (*,j,2,T5)A=B op C
	else if (quat.operatorType == "+" || quat.operatorType == "-"
		|| quat.operatorType == "*" || quat.operatorType == "/") {


		int reg_A = AllocateReg(quat.result);
		int reg_B = AllocateReg(quat.op1);
		int reg_C = AllocateReg(quat.op2);//C可能是立即数

		switch (quat.operatorType[0]) {
			case '+':
				asmCode.push_back("add " + regInfo[reg_A].name + "," + regInfo[reg_B].name + "," + regInfo[reg_C].name);
				break;
			case '-':
				asmCode.push_back("sub " + regInfo[reg_A].name + "," + regInfo[reg_B].name + "," + regInfo[reg_C].name);
				break;
			case '*':
				asmCode.push_back("mul " + regInfo[reg_A].name + "," + regInfo[reg_B].name + "," + regInfo[reg_C].name);
				break;
			case '/':
				asmCode.push_back("div " + regInfo[reg_B].name + "," + regInfo[reg_C].name);
				asmCode.push_back("move " + regInfo[reg_A].name + ",$lo");
				break;
		}

	}

	//(j<=,i,100,24)...判断跳转类： A compare B
	else if (quat.operatorType == "j>" || quat.operatorType == "j>="
		|| quat.operatorType == "j<" || quat.operatorType == "j<=" || quat.operatorType == "j=") {
		int reg_A = AllocateReg(quat.op1);
		int reg_B = AllocateReg(quat.op2);
		if (quat.operatorType == "j>") {
			asmCode.push_back("bgt " + regInfo[reg_A].name + "," + regInfo[reg_B].name + ",Label_" + quat.result);
		}
		else if (quat.operatorType == "j>=") {
			asmCode.push_back("bge " + regInfo[reg_A].name + "," + regInfo[reg_B].name + ",Label_" + quat.result);
		}
		else if (quat.operatorType == "j<") {
			asmCode.push_back("blt " + regInfo[reg_A].name + "," + regInfo[reg_B].name + ",Label_" + quat.result);
		}
		else if (quat.operatorType == "j<=") {
			asmCode.push_back("ble " + regInfo[reg_A].name + "," + regInfo[reg_B].name + ",Label_" + quat.result);
		}
		else if (quat.operatorType == "j=") {
			asmCode.push_back("beq " + regInfo[reg_A].name + "," + regInfo[reg_B].name + ",Label_" + quat.result);
		}
	}
}

//分配寄存器t0~t9
int GenASM::AllocateReg(string var)
{
	/**********************************STEP1 确认分配内存*****************************************/
	//如果是之前未出现过的变量，要先对其分配内存
	//如果是立即数,不分配内存。只分配一个寄存器
	if ((!isdigit(var[0])) && localVarTable.find(var) == localVarTable.end() && globalVarTable.find(var) == globalVarTable.end()) {
		//全局
		if (procedureLable == "") {
			//加入全局变量
			globalVarTable.insert({ var,globalvar_to_data });
			//全局变量偏移量更新
			globalvar_to_data += 4;
		}
		//局部
		else {
			//加入局部变量
			localVarTable.insert(pair<string, int>(var, localvar_to_fp));
			//局部变量偏移量更新
			localvar_to_fp -= 4;
			//sp移动,push in
			asmCode.push_back("subi $sp, $sp, 4");
		}
		AVALUE.insert({ var,{} });
	}
	/**********************************STEP1 分配寄存器*****************************************/
	//返回的目标寄存器编号
	int regidx = -1;
	auto iter = AVALUE.find(var);
	//如果是常量 找AVALUE中的"123imm" 
	//且:常数不可复用123imm占用的寄存器，只能抢占或分配
	if (isdigit(var[0])) {
		iter = AVALUE.find("123imm");
		if (iter == AVALUE.end()) {
			cout << "ERR:AllocateReg出错";
			return -1;
		}
	}
	//在AVALUE中已经占有则返回该寄存器
	if (!iter->second.empty() && !isdigit(var[0])) {
		string reg_name = *(iter->second.begin());//取得AVALUE记录的第一个reg
		for (int i = 0; i < REGNUM; i++) {
			if (regInfo[i].name == reg_name) {
				regidx = i;
				break;
			}
		}

	}
	//无则分配新寄存器
	else {
		//1、有空余的寄存器t0~t9
		for (int i = 0; i < REGNUM; i++) {
			if (regInfo[i].name[1]=='t'&& RVALUE[i] == "null") {
				RVALUE[i] = var;						//占用该寄存器
				iter->second.insert(regInfo[i].name);	//将分配的REG更新到对应的AVALUE表中
				regidx = i;								//空闲寄存器赋值
				break;
			}
		}
		//2、没空余的，发动LRU抢占
		if (regidx == -1) {
			int occupy_idx = GetLRU();					//获得目标reg id
			regidx = occupy_idx;						//抢占寄存器赋值
			//RVALUE更新
			string out_var = RVALUE[occupy_idx];	//被抢占的变量名
			RVALUE[occupy_idx] = var;
			//抢占者更新AVALUE
			iter->second.insert(regInfo[occupy_idx].name);

			//被抢占者若不是常数 则更新AVALUE  并保存
			if (!isdigit(out_var[0])) {
				auto iter_out = AVALUE.find(out_var);
				iter_out->second.erase(regInfo[occupy_idx].name);
				//生成存数指令SW 根据是局部还是全局去生成sw指令
				auto localiter = localVarTable.find(out_var);
				auto globaliter = globalVarTable.find(out_var);
				if (localiter != localVarTable.end()) {
					//局部变量的保存，根据局部符号表获取偏移位置，保存到该处。
					int offset = localiter->second;
					string code = "sw " + regInfo[occupy_idx].name + "," + to_string(offset) + "($fp)";
					asmCode.push_back(code);
				}
				else if (globaliter != globalVarTable.end()) {
					//!!!存疑
					int offset = globaliter->second;
					string code = "sw " + regInfo[occupy_idx].name + "," + to_string(offset) + "($zero)";
					asmCode.push_back(code);
				}
			}
		}
		//完成了寄存器的获取
		//实现取数指令LW 将内存空间的值存入寄存器使用
		//变量才需要从内存取数
		if (!isdigit(var[0])) {
			auto localiter = localVarTable.find(var);
			auto globaliter = globalVarTable.find(var);
			if (localiter != localVarTable.end()) {
				int offset = localiter->second;
				string code = "lw " + regInfo[regidx].name + "," + to_string(offset) + "($fp)";
				asmCode.push_back(code);
			}
			else if (globaliter != globalVarTable.end()) {
				int offset = globaliter->second;
				string code = "lw " + regInfo[regidx].name + "," + to_string(offset) + "($zero)";
				asmCode.push_back(code);
			}
		}
	}
	//常量只需要存到获取的reg
	if (isdigit(var[0]))
		asmCode.push_back("addi " + regInfo[regidx].name + ",$zero," + var);

	//更新regidx的ftime
	regInfo[regidx].ftime = 0;

	return regidx;
}
//在t0~t9最近最久未使用的寄存器的idx
int GenASM::GetLRU()
{
	int maxidx = -1;
	int maxtime = -1;
	int len = regInfo.size();
	for (int i = 0; i < len; i++) {
		if (regInfo[i].name[1]=='t'&&regInfo[i].ftime > maxtime) {
			maxidx = i;
			maxtime = regInfo[i].ftime;
		}
	}
	return maxidx;
}
//写入文件或打印
void GenASM::print(int mode,string file_path)
{
	if (mode == CMD_MODE||mode==BOTH_MODE) {
		cout << "目标代码生成如下：" << endl;
		for (auto it = asmCode.begin(); it != asmCode.end(); it++) {
			cout << *it << endl;
		}
	}
	if (mode == FILE_MODE || mode == BOTH_MODE) {
		ofstream outfile;
		outfile.open(file_path,ios::out);
		if (!outfile.is_open()) {
			cout << "ERR:无法打开MIPS汇编代码的生成文件!" << endl;
			throw(FILE_OPEN_ERROE);
		}
		for (auto it = asmCode.begin(); it != asmCode.end(); it++) {
			outfile << *it << endl;
		}
	}
}

void GenASM::AddNote(Quatenary quat) {
	asmCode.push_back("\n# (" + quat.operatorType +"," + quat.op1 + "," + quat.op2 + "," + quat.result+")");
}
int GenASM::getRegIdx(string reg)
{
	for (int i = 0; i < REGNUM; i++) {
		if (regInfo[i].name == reg)
			return i;
	}
	return -1;
}