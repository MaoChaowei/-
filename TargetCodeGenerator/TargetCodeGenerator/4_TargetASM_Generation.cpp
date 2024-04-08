#include "4_TargetASM_Generation.h"
#include <fstream>
#include<ctype.h>

/* boundary
 ��ֻ����int��
 �ֲ������Ͳ�������ֻʹ��t0~t9
 ��δ����ȫ�ֱ�����ʵ�֣�Ӧ������data���У���ȫ�ַ��ű�ά��
 AVALUE���������б�����֧�ֱ���������
*/


//���캯��
GenASM::GenASM(vector<Quatenary> quatenary)
{
	//����Ԫʽ
	this->quatenary = quatenary;
	//	��Ϊ����(j,-,-,39)���Ҫ�ڴ����39����Ԫʽǰ���ϱ�ǩ������Ԥ�Ƚ�Ҫ�ӱ�ǩ����Ԫʽ��Ŷ���¼������
	int len = quatenary.size();
	for (int i = 0; i < len; i++) {
		if (quatenary[i].operatorType[0] == 'j')
			labels.insert(quatenary[i].result);
	}
	
	vector<string> registers = {							//MIPS��32���Ĵ���
		"$zero",											//$0 ����0(constant value 0)
		"$at",												//$1 �����������(Reserved for assembler)
		"$v0","$v1",										//$2-$3 �������÷���ֵ(values for results and expression evaluation)
		"$a0","$a1","$a2","$a3",							//$4-$7 �������ò���(arguments)
		"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",	//$8-$15 ��ʱ��(������õ�)
		"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",	//$16-$23 �����(������ã���ҪSAVE/RESTORE��)(saved)
		"$t8","$t9",										//$24-$25 ��ʱ��(������õ�)
		"$k0","k1",											//����ϵͳ���쳣������������ҪԤ��һ�� 
		"$gp",												//$28 ȫ��ָ��(Global Pointer)
		"$sp",												//$29 ��ջָ��(Stack Pointer)
		"$fp",												//$30 ָ֡��(Frame Pointer)
		"$ra"												//$31 ���ص�ַ(return address)
	};
	//��ʼ���Ĵ���
	for (int i = 0; i < REGNUM; i++) {
		Reg temp = { i,registers[i] ,0 };
		this->regInfo.push_back(temp);
	}
	//��ʼ���Ĵ���ʹ��״̬RVALUE
	RVALUE[0] = "0";//$0 ����0(constant value 0)
	for (int i = 0; i < REGNUM; i++) {
		RVALUE[i] = "null";	//��ʼΪ��
	}
	//Ϊ������׼��һ��λ�ã����ڹ���Ϊ��������ļĴ���
	AVALUE.insert({ "123imm",{} });

}

//Ŀ���������
void GenASM::TargetCodeGen()
{
	//��ʼ��ջ�ṹ
	asmCode.push_back("addi $sp,$zero,"+to_string(STACK_ADDR));
	asmCode.push_back("addi $fp,$zero,"+to_string(STACK_ADDR+4));//!!!!!
	//��������Ԫʽ
	int len = quatenary.size();
	for (int i = 0; i < len; i++) {
		//ftime���� ��������Ϊ0��������ÿ�η���ǰ����
		for (vector<Reg>::iterator iter = regInfo.begin(); iter != regInfo.end(); iter++)
				iter->ftime++;
		
		//Ŀ���������
		SingleStepGen(quatenary[i]);
	}
	return;
}


//����Ԫʽ�ľ���ASM��������
void GenASM::SingleStepGen(Quatenary quat)
{
	//��ӻ��ע��
	AddNote(quat);

	//���⴦��--j��j>...��ת�ࣺ�Ӻ�ӱ�ǩ
	if (labels.find(to_string(quat.idx)) != labels.end()) {
		string str = "Label_" + to_string(quat.idx) + ":";
		asmCode.push_back(str);
	}

	//(program,-,-,-)...���������ࣺ������ջ֡ջ�Ľṹ
	if (quat.op1 == "-" && quat.op2 == "-" && quat.result == "-") {
		//maintain
		//���һ���ɾ��ľֲ����������ڱ�����ʹ�ã���ֱ��ˢ�¼��ɡ�
		localVarTable.clear();
		procedureLable = quat.operatorType;//���ù�����
		param_to_fp = PARAM_TO_FP;//����ջ�ṹ
		localvar_to_fp = LOCALVAR_TO_FP;

		//generate code:
		//1\  ��ǩ
		string str = quat.operatorType + ":";
		asmCode.push_back(str);
		//2\  ��ra
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw $ra,0($sp)");
		//3\   ��fp����fp��Ϊ��ǰջ֡��ջ��
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw $fp,0($sp)");
		asmCode.push_back("move $fp,$sp");
	}
	//(call,demo,-,T10)...���̵��ã���ת
	else if (quat.operatorType == "call") {
		//�����ֳ�:�����оֲ��������ʹ�õļĴ���ֵ��newest��д�ص��ڴ�
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
		//����jal��ת
		asmCode.push_back("jal " + quat.op1);
		//�ָ��ֳ��������оֲ������ڴ��е�ֵд�ص��þֲ�����ռ�õ����мĴ�����
		for (auto iter : localVarTable) {
			if (!AVALUE.at(iter.first).empty()) {
				for (auto r : AVALUE.at(iter.first)) {
					asmCode.push_back("lw " + r + "," + to_string(iter.second) + "($fp)");
					int idx = getRegIdx(r);
					RVALUE[idx] = iter.first;
				}
			}
		}
		//����,ע��ֻ֧�ַ���һ�����ֽڵĽ��������v1�С�
		int ans_idx = AllocateReg(quat.result);
		asmCode.push_back("move " + regInfo[ans_idx].name + ",$v1");
	}

	//(return,main_return_value,-,main)...�������أ�ȡ�÷���ֵ����ջ����ת��
	else if (quat.operatorType == "return") {
		//1���з���ֵ ��淵��ֵ->v1
		if (quat.op1 != "-") {
			string ans = *(AVALUE.at(quat.op1).begin());
			asmCode.push_back("move $v1," + ans);
		}
		//2���ָ�AVALUE��RVALUE���ͷžֲ�����ռ�õļĴ���
		for (auto it = localVarTable.begin(); it != localVarTable.end(); it++) {
			auto p = AVALUE.find(it->first);
			if (p != AVALUE.end()) {//��Ȼ�ж� ���ֲ�����Ӧ��������AVALUE��
				for (auto Preg = p->second.begin(); Preg != p->second.end(); Preg++) {
					for (int i = 0; i < REGNUM; i++) {
						if (regInfo[i].name == *Preg)
							RVALUE[i] = "null";
					}
				}
				AVALUE.erase(it->first);
			}
		}

		//3����ջ
		//sp=fp��spĨ����������,fp=old fp
		asmCode.push_back("move $sp,$fp");//��ֱ�����
		asmCode.push_back("addi $sp,$sp," + to_string(param_to_fp));//���������
		asmCode.push_back("lw $ra,4($fp)");//ȡ�����ص�ַ
		asmCode.push_back("lw $fp,0($fp)");//�ص�old fp����ɳ�ջ
		//4��jr��ת ע�⣺������main�����Ͳ���ת�ˣ�ֱ�ӽ��������Լ��Ͻ����Ļ��ָ���
		if (procedureLable != "main")
			asmCode.push_back("jr $ra");
		//5���ָ�������Ա���(������ ջ��ʼ�ṹ �ֲ��������ű�)
		procedureLable = "";
		localVarTable.clear();
		param_to_fp = PARAM_TO_FP;
		localvar_to_fp = LOCALVAR_TO_FP;
	}

	//(defpar,-,-,a)...�β����������������뵽�ֲ��������У����param��Ԫʽ��ɲ������ݡ�
	else if (quat.operatorType == "defpar") {
		localVarTable.insert({ quat.result,param_to_fp });
		param_to_fp += 4;
		auto param = pair<string, set<string>>(quat.result, {});
		AVALUE.insert(param);
	}
	//(param,5,-,-)...ʵ���������Ѳ���ѹ�뵱ǰջ֡ջ��
	else if (quat.operatorType == "param") {
		//���������֡�����ͳһ����
		//ȡ�øò����ļĴ���/�����֮
		int reg_idx = AllocateReg(quat.op1);
		//ѹ����һ֡�Ĳ�����
		asmCode.push_back("subi $sp,$sp,4");
		asmCode.push_back("sw " + regInfo[reg_idx].name + ",0($sp)");
	}
	//(j,-,-,7)...��ת�ࣺ
	else if (quat.operatorType == "j") {
		asmCode.push_back("j Label_" + quat.result);
	}

	//(=,0,-,T6)...��ֵ�ࣺ
	else if (quat.operatorType == "=") {
		//��ȡ�����Ŀ������ļĴ�����
		int regidx = AllocateReg(quat.result);
		//��������ֵ
		if (isdigit(quat.op1[0])) {
			asmCode.push_back("addi " + regInfo[regidx].name + ",$zero," + quat.op1);
		}
		//������ֵ
		else {
			//��ȡ�������ֵ�����ļĴ�����
			int rightidx = AllocateReg(quat.op1);
			asmCode.push_back("move " + regInfo[regidx].name + "," + regInfo[rightidx].name);
		}
	}

	//�������� (*,j,2,T5)A=B op C
	else if (quat.operatorType == "+" || quat.operatorType == "-"
		|| quat.operatorType == "*" || quat.operatorType == "/") {


		int reg_A = AllocateReg(quat.result);
		int reg_B = AllocateReg(quat.op1);
		int reg_C = AllocateReg(quat.op2);//C������������

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

	//(j<=,i,100,24)...�ж���ת�ࣺ A compare B
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

//����Ĵ���t0~t9
int GenASM::AllocateReg(string var)
{
	/**********************************STEP1 ȷ�Ϸ����ڴ�*****************************************/
	//�����֮ǰδ���ֹ��ı�����Ҫ�ȶ�������ڴ�
	//�����������,�������ڴ档ֻ����һ���Ĵ���
	if ((!isdigit(var[0])) && localVarTable.find(var) == localVarTable.end() && globalVarTable.find(var) == globalVarTable.end()) {
		//ȫ��
		if (procedureLable == "") {
			//����ȫ�ֱ���
			globalVarTable.insert({ var,globalvar_to_data });
			//ȫ�ֱ���ƫ��������
			globalvar_to_data += 4;
		}
		//�ֲ�
		else {
			//����ֲ�����
			localVarTable.insert(pair<string, int>(var, localvar_to_fp));
			//�ֲ�����ƫ��������
			localvar_to_fp -= 4;
			//sp�ƶ�,push in
			asmCode.push_back("subi $sp, $sp, 4");
		}
		AVALUE.insert({ var,{} });
	}
	/**********************************STEP1 ����Ĵ���*****************************************/
	//���ص�Ŀ��Ĵ������
	int regidx = -1;
	auto iter = AVALUE.find(var);
	//����ǳ��� ��AVALUE�е�"123imm" 
	//��:�������ɸ���123immռ�õļĴ�����ֻ����ռ�����
	if (isdigit(var[0])) {
		iter = AVALUE.find("123imm");
		if (iter == AVALUE.end()) {
			cout << "ERR:AllocateReg����";
			return -1;
		}
	}
	//��AVALUE���Ѿ�ռ���򷵻ظüĴ���
	if (!iter->second.empty() && !isdigit(var[0])) {
		string reg_name = *(iter->second.begin());//ȡ��AVALUE��¼�ĵ�һ��reg
		for (int i = 0; i < REGNUM; i++) {
			if (regInfo[i].name == reg_name) {
				regidx = i;
				break;
			}
		}

	}
	//��������¼Ĵ���
	else {
		//1���п���ļĴ���t0~t9
		for (int i = 0; i < REGNUM; i++) {
			if (regInfo[i].name[1]=='t'&& RVALUE[i] == "null") {
				RVALUE[i] = var;						//ռ�øüĴ���
				iter->second.insert(regInfo[i].name);	//�������REG���µ���Ӧ��AVALUE����
				regidx = i;								//���мĴ�����ֵ
				break;
			}
		}
		//2��û����ģ�����LRU��ռ
		if (regidx == -1) {
			int occupy_idx = GetLRU();					//���Ŀ��reg id
			regidx = occupy_idx;						//��ռ�Ĵ�����ֵ
			//RVALUE����
			string out_var = RVALUE[occupy_idx];	//����ռ�ı�����
			RVALUE[occupy_idx] = var;
			//��ռ�߸���AVALUE
			iter->second.insert(regInfo[occupy_idx].name);

			//����ռ�������ǳ��� �����AVALUE  ������
			if (!isdigit(out_var[0])) {
				auto iter_out = AVALUE.find(out_var);
				iter_out->second.erase(regInfo[occupy_idx].name);
				//���ɴ���ָ��SW �����Ǿֲ�����ȫ��ȥ����swָ��
				auto localiter = localVarTable.find(out_var);
				auto globaliter = globalVarTable.find(out_var);
				if (localiter != localVarTable.end()) {
					//�ֲ������ı��棬���ݾֲ����ű��ȡƫ��λ�ã����浽�ô���
					int offset = localiter->second;
					string code = "sw " + regInfo[occupy_idx].name + "," + to_string(offset) + "($fp)";
					asmCode.push_back(code);
				}
				else if (globaliter != globalVarTable.end()) {
					//!!!����
					int offset = globaliter->second;
					string code = "sw " + regInfo[occupy_idx].name + "," + to_string(offset) + "($zero)";
					asmCode.push_back(code);
				}
			}
		}
		//����˼Ĵ����Ļ�ȡ
		//ʵ��ȡ��ָ��LW ���ڴ�ռ��ֵ����Ĵ���ʹ��
		//��������Ҫ���ڴ�ȡ��
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
	//����ֻ��Ҫ�浽��ȡ��reg
	if (isdigit(var[0]))
		asmCode.push_back("addi " + regInfo[regidx].name + ",$zero," + var);

	//����regidx��ftime
	regInfo[regidx].ftime = 0;

	return regidx;
}
//��t0~t9������δʹ�õļĴ�����idx
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
//д���ļ����ӡ
void GenASM::print(int mode,string file_path)
{
	if (mode == CMD_MODE||mode==BOTH_MODE) {
		cout << "Ŀ������������£�" << endl;
		for (auto it = asmCode.begin(); it != asmCode.end(); it++) {
			cout << *it << endl;
		}
	}
	if (mode == FILE_MODE || mode == BOTH_MODE) {
		ofstream outfile;
		outfile.open(file_path,ios::out);
		if (!outfile.is_open()) {
			cout << "ERR:�޷���MIPS������������ļ�!" << endl;
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