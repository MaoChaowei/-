#include "3_Semantic_Analysis.h"
#include <fstream>

/*******************  SemanticSymbolTable  *******************/
// ���캯��
SemanticSymbolTable::SemanticSymbolTable(const TableType type, const string name)
{
	this->table_type = type;
	this->table_name = name;
}
// ��������Ѱ��һ������ û�ҵ��򷵻�-1�����򷵻ر����ڱ��еı��
int SemanticSymbolTable::FindSymbol(const string name)
{
	int size = this->table.size();
	for (int i = 0; i < size; i++) {
		if (table[i].identifierName == name)
			return i;
	}
	return -1;
}
// ����һ�����������ؼ����λ��
int SemanticSymbolTable::AddSymbol(const IdentifierInfo id)
{
	// ����û������� ���������ڱ��е�λ�á�
	if (FindSymbol(id.identifierName) == -1) {
		table.push_back(id);
		return table.size() - 1;
	}
	// �Ѵ����򷵻�-1
	return -1;
}

/*******************  SemanticAnalysis  *******************/
// ���캯��
SemanticAnalysis::SemanticAnalysis()
{
	// Ϊ�����������ȫ�ַ��ű�
	this->tables.push_back(SemanticSymbolTable(SemanticSymbolTable::GlobalTable, "theGlobalTable"));
	// ���õ�ǰ������Ϊ ȫ��������
	this->currentTableStack.push_back(0);
	// ������ʱ�������ű�
	this->tables.push_back(SemanticSymbolTable(SemanticSymbolTable::TempTable, "theTempTable"));

	// Լ����0����Ԫʽ��(j,-,-,main_function)
	this->nextQuaternaryIdx = 1;
	this->mainIdx = -1;			// main������ų�ʼֵ����Ϊ-1
	this->tmpVarCount = 0;		// ��ʱ��������Ϊ0
	this->backpatchLevel = 0;	// �������Ҫ����
}

void SemanticAnalysis::AddSymbol2List(const SemanticSymbol symbol)
{
	// �����з�����Ϣ����symbol_list
	this->symbolList.push_back(symbol);
}
// ��������
void SemanticAnalysis::Analysis(const string production_left, const vector<string> production_right)
{
	if (production_left == "Program")
		Semantic_Analysis_Program(production_left, production_right);
	else if (production_left == "ExtDef")
		Semantic_Analysis_ExtDef(production_left, production_right);
	// VarSpecifier ::= int
	else if (production_left == "VarSpecifier")
		Semantic_Analysis_VarSpecifier(production_left, production_right);
	else if (production_left == "FunSpecifier")
		Semantic_Analysis_FunSpecifier(production_left, production_right);
	else if (production_left == "FunDec")
		Semantic_Analysis_FunDec(production_left, production_right);
	else if (production_left == "CreateFunTable_m")
		Semantic_Analysis_CreateFunTable_m(production_left, production_right);
	else if (production_left == "ParamDec")
		Semantic_Analysis_ParamDec(production_left, production_right);
	else if (production_left == "Block")
		Semantic_Analysis_Block(production_left, production_right);
	else if (production_left == "Def")
		Semantic_Analysis_Def(production_left, production_right);
	else if (production_left == "AssignStmt")
		Semantic_Analysis_AssignStmt(production_left, production_right);
	else if (production_left == "Exp")
		Semantic_Analysis_Exp(production_left, production_right);
	else if (production_left == "AddSubExp")
		Semantic_Analysis_AddSubExp(production_left, production_right);
	else if (production_left == "Item")
		Semantic_Analysis_Item(production_left, production_right);
	else if (production_left == "Factor")
		Semantic_Analysis_Factor(production_left, production_right);
	else if (production_left == "CallStmt")
		Semantic_Analysis_CallStmt(production_left, production_right);
	else if (production_left == "CallFunCheck")
		Semantic_Analysis_CallFunCheck(production_left, production_right);
	else if (production_left == "Args")
		Semantic_Analysis_Args(production_left, production_right);
	else if (production_left == "ReturnStmt")
		Semantic_Analysis_ReturnStmt(production_left, production_right);
	else if (production_left == "Relop")
		Semantic_Analysis_Relop(production_left, production_right);
	else if (production_left == "IfStmt")
		Semantic_Analysis_IfStmt(production_left, production_right);
	else if (production_left == "IfStmt_m1")
		Semantic_Analysis_IfStmt_m1(production_left, production_right);
	else if (production_left == "IfStmt_m2")
		Semantic_Analysis_IfStmt_m2(production_left, production_right);
	else if (production_left == "IfNext")
		Semantic_Analysis_IfNext(production_left, production_right);
	else if (production_left == "IfStmt_next")
		Semantic_Analysis_IfStmt_next(production_left, production_right);
	else if (production_left == "WhileStmt")
		Semantic_Analysis_WhileStmt(production_left, production_right);
	else if (production_left == "WhileStmt_m1")
		Semantic_Analysis_WhileStmt_m1(production_left, production_right);
	else if (production_left == "WhileStmt_m2")
		Semantic_Analysis_WhileStmt_m2(production_left, production_right);
	else {
		if (production_right[0] != "@") {
			int count = production_right.size();
			while (count--)
				this->symbolList.pop_back();
		}
		this->symbolList.push_back({ production_left,"",-1,-1,-1,-1 });
	}
}
// ��ӡ��Ԫʽ��
void SemanticAnalysis::PrintQuaternary(const string file_path)
{
	ofstream fout;
	fout.open(file_path, ios::out);
	if (!fout) {
		cout << file_path << "�ļ���ʧ��!" << endl;
		return;
	}
	cout << "/*******************���������Ԫʽ���********************/" << endl;
	for (int i = 0; i < this->quaternary.size(); i++) {
		fout << this->quaternary[i].idx << " (" << this->quaternary[i].operatorType << "," <<
			this->quaternary[i].op1 << "," << this->quaternary[i].op2 << "," << this->quaternary[i].result << ")\n";
		cout << this->quaternary[i].idx << " (" << this->quaternary[i].operatorType << "," <<
			this->quaternary[i].op1 << "," << this->quaternary[i].op2 << "," << this->quaternary[i].result << ")\n";
	}
	cout << "��Ԫʽ�Ѵ����ļ�" << file_path << endl;
}

// �����������

// Program : ExtDefList  
void SemanticAnalysis::Semantic_Analysis_Program(const string production_left, const vector<string> production_right)
{
	// ���û�ж���main�������򱨴�
	if (mainIdx == -1) {
		cout << "��������з�������δ����main����" << endl;
		throw(SEMANTIC_ERROR_NO_MAIN);
	}
	int count = production_right.size();
	while (count--) {
		symbolList.pop_back();
	}
	// ����ǰ�������Ԫʽ
	quaternary.insert(quaternary.begin(), { 0, "j","-" , "-", to_string(mainIdx) });

	symbolList.push_back({ production_left, "", -1, -1, -1,-1 });
}
// ExtDef : VarSpecifier identifier ; \ VarSpecifier FunDec Block
void SemanticAnalysis::Semantic_Analysis_ExtDef(const string production_left, const vector<string> production_right)
{
	// ����Ƕ������
	if (production_right.size() == 3) {
		// ĿǰsymbolList��ĩβ��Specifier identifier ;���ɴ��ҵ�Specifier��identifier
		SemanticSymbol specifier = symbolList[symbolList.size() - 3];  // ��������
		SemanticSymbol identifier = symbolList[symbolList.size() - 2]; // ������

		// �����жϸñ����Ƿ��ڵ�ǰ���Ѿ�����
		bool existed = false;
		SemanticSymbolTable* current_table = &tables[currentTableStack.back()];
		if (current_table->FindSymbol(identifier.value) != -1) {
			cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "�ض���" << endl;
			throw(SEMANTIC_ERROR_REDEFINED);
		}

		// ����һ��������table
		IdentifierInfo variable;
		variable.identifierName = identifier.value; // ������
		variable.idType = IdentifierInfo::VAR;
		variable.specifierType = specifier.value;   // ��������

		// ����table
		current_table->AddSymbol(variable);

		// symbolList����
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left,identifier.value,identifier.row,identifier.col,currentTableStack.back(), int(current_table->table.size() - 1) });
	}
	// ����Ƕ��庯��
	else {
		SemanticSymbol identifier = symbolList[symbolList.size() - 2];

		// ��Ҫ�˳�������
		currentTableStack.pop_back();
		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left,identifier.value,identifier.row,identifier.col,identifier.tableIdx,identifier.idx });
	}
}
// VarSpecifier : int \ void \ float \ double
void SemanticAnalysis::Semantic_Analysis_VarSpecifier(const string production_left, const vector<string> production_right)
{
	// symbolList�����һ����int
	SemanticSymbol specifier = symbolList.back();
	specifier.value = production_right[0];
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,specifier.value,specifier.row,specifier.col,-1,-1 });
}
// $ VarSpecifier : void \ int 
void SemanticAnalysis::Semantic_Analysis_FunSpecifier(const string production_left, const vector<string> production_right)
{
	// symbolList�����һ����int��void
	SemanticSymbol specifier = symbolList.back();
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,specifier.value,specifier.row,specifier.col,-1,-1 });
}
// FunDec : identifier CreateFunTable_m ( VarList )
void SemanticAnalysis::Semantic_Analysis_FunDec(const string production_left, const vector<string> production_right)
{
	// symbolList��CreateFunTable_m��¼��table��Ϣ
	SemanticSymbol specifier = symbolList[symbolList.size() - 4];
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,specifier.value,specifier.row,specifier.col,specifier.tableIdx,specifier.idx });
}
// CreateFunTable_m : @
void SemanticAnalysis::Semantic_Analysis_CreateFunTable_m(const string production_left, const vector<string> production_right)
{
	// ����������
	// ��ʱsymbolList���һ������Ϊ�������������ڶ���Ϊ��������ֵ
	SemanticSymbol identifier = symbolList.back();
	SemanticSymbol specifier = symbolList[symbolList.size() - 2];

	// ������ȫ�ֵ�table�жϺ������Ƿ��ض���
	if (tables[0].FindSymbol(identifier.value) != -1) {
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "�ض���" << endl;
		throw(SEMANTIC_ERROR_REDEFINED);
	}

	// ����������
	tables.push_back(SemanticSymbolTable(SemanticSymbolTable::FunctionTable, identifier.value));
	// ��ȫ�ַ��ű�����ǰ�����ķ�������������������ڵ�ַ����л��
	tables[0].AddSymbol({ IdentifierInfo::FUN,specifier.value,identifier.value,0,0,int(tables.size() - 1) });

	// ������ѹ��ջ
	currentTableStack.push_back(tables.size() - 1);
	// ����ֵ
	IdentifierInfo return_value;
	return_value.idType = IdentifierInfo::RET;
	return_value.identifierName = tables.back().table_name + "_return_value";
	return_value.specifierType = specifier.value;
	// ���Ϊmain����������м�¼
	if (identifier.value == "main")
		mainIdx = nextQuaternaryIdx;
	// ������Ԫʽ
	quaternary.push_back({ nextQuaternaryIdx++ , identifier.value,"-","-" ,"-" });
	// �������м��뷵�ر���
	tables[currentTableStack.back()].AddSymbol(return_value);
	// �մ�����Ҫpop
	// ����pushback
	symbolList.push_back({ production_left,identifier.value,identifier.row,identifier.col,0,int(tables[0].table.size() - 1) });
}
// ParamDec : VarSpecifier identifier
void SemanticAnalysis::Semantic_Analysis_ParamDec(const string production_left, const vector<string> production_right)
{
	// symbolList���һ��Ϊ�������������ڶ���Ϊ����
	SemanticSymbol identifier = symbolList.back();
	SemanticSymbol specifier = symbolList[symbolList.size() - 2];
	// ��ǰ������
	SemanticSymbolTable& function_table = tables[currentTableStack.back()];


	// ����Ѿ����й�����
	if (function_table.FindSymbol(identifier.value) != -1) {
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У���������" << identifier.value << "�ض���" << endl;
		throw(SEMANTIC_ERROR_REDEFINED);
	}
	// ����������βα���
	int new_position = function_table.AddSymbol({ IdentifierInfo::VAR,specifier.value,identifier.value,-1,-1,-1 });
	// ��ǰ������ȫ�ַ����е�����
	int table_position = tables[0].FindSymbol(function_table.table_name);
	// �βθ���++
	tables[0].table[table_position].funcParameterNum++;

	// ������Ԫʽ
	quaternary.push_back({ nextQuaternaryIdx++, "defpar","-" , "-", identifier.value });

	// symbolList����
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,identifier.value,identifier.row,identifier.col,currentTableStack.back(),new_position });
}
// Block : { DefList StmtList }
void SemanticAnalysis::Semantic_Analysis_Block(const string production_left, const vector<string> production_right)
{
	// ����symbolList
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}
// Def : VarSpecifier identifier ;
void SemanticAnalysis::Semantic_Analysis_Def(const string production_left, const vector<string> production_right)
{
	// symbolList�ĵ����ڶ����������������Ǳ�����������
	SemanticSymbol identifier = symbolList[symbolList.size() - 2];
	SemanticSymbol specifier = symbolList[symbolList.size() - 3];
	SemanticSymbolTable& current_table = tables[currentTableStack.back()];

	// �ض����򱨴�
	if (current_table.FindSymbol(identifier.value) != -1)
	{
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "�ض���" << endl;
		throw(SEMANTIC_ERROR_REDEFINED);
	}

	current_table.AddSymbol({ IdentifierInfo::VAR,specifier.value,identifier.value ,-1,-1,-1 });

	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left, identifier.value ,identifier.row,identifier.col,currentTableStack.back(),int(tables[currentTableStack.back()].table.size() - 1) });
}
// AssignStmt : identifier = Exp
void SemanticAnalysis::Semantic_Analysis_AssignStmt(const string production_left, const vector<string> production_right)
{
	// symbolList�ĵ�����һ����������������Exp�ͱ�����
	SemanticSymbol identifier = symbolList[symbolList.size() - 3];
	SemanticSymbol exp = symbolList.back();

	// ���id�Ƿ���ڣ��������򱨴�
	bool existed = false;
	int tableIdx = -1, idx = -1;
	// �ӵ�ǰ�㿪ʼ���ϱ���
	for (int scope_layer = currentTableStack.size() - 1; scope_layer >= 0; scope_layer--) {
		auto current_table = tables[currentTableStack[scope_layer]];
		if ((idx = current_table.FindSymbol(identifier.value)) != -1) {
			existed = true;
			tableIdx = currentTableStack[scope_layer];
			break;
		}
	}
	if (existed == false) {
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "δ����" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}

	quaternary.push_back({ nextQuaternaryIdx++, "=", exp.value, "-", identifier.value });

	// ����symbolList
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left, identifier.value ,identifier.row,identifier.col,tableIdx,idx });
}
// Exp : AddSubExp \ Exp Relop AddSubExp
void SemanticAnalysis::Semantic_Analysis_Exp(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 1) {
		SemanticSymbol exp = symbolList.back();
		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, exp.value ,exp.row,exp.col,exp.tableIdx,exp.idx });
	}
	else {
		SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
		SemanticSymbol op = symbolList[symbolList.size() - 2];
		SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
		int next_label_num = nextQuaternaryIdx++;
		string new_tmp_var = "T" + to_string(tmpVarCount++);
		quaternary.push_back({ next_label_num, "j" + op.value, sub_exp1.value, sub_exp2.value, to_string(next_label_num + 3) });
		quaternary.push_back({ nextQuaternaryIdx++, "=", "0", "-", new_tmp_var });
		quaternary.push_back({ nextQuaternaryIdx++, "j", "-", "-", to_string(next_label_num + 4) });
		quaternary.push_back({ nextQuaternaryIdx++, "=", "1", "-", new_tmp_var });

		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, new_tmp_var ,-1,-1,-1,-1 });
	}
}
// AddSubExp : Item \ Item + Item \ Item - Item
void SemanticAnalysis::Semantic_Analysis_AddSubExp(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 1) {
		SemanticSymbol exp = symbolList.back();
		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, exp.value ,exp.row,exp.col,exp.tableIdx,exp.idx });
	}
	else {
		SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
		SemanticSymbol op = symbolList[symbolList.size() - 2];
		SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
		string new_tmp_var = "T" + to_string(tmpVarCount++);
		quaternary.push_back({ nextQuaternaryIdx++, op.token, sub_exp1.value, sub_exp2.value, new_tmp_var });

		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, new_tmp_var ,-1,-1,-1,-1 });
	}
}
// Item : Factor \ Factor * Factor \ Factor / Factor
void SemanticAnalysis::Semantic_Analysis_Item(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 1) {
		SemanticSymbol exp = symbolList.back();
		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, exp.value ,exp.row,exp.col,exp.tableIdx,exp.idx });
	}
	else {
		SemanticSymbol sub_exp1 = symbolList[symbolList.size() - 3];
		SemanticSymbol op = symbolList[symbolList.size() - 2];
		SemanticSymbol sub_exp2 = symbolList[symbolList.size() - 1];
		std::string new_tmp_var = "T" + to_string(tmpVarCount++);
		quaternary.push_back({ nextQuaternaryIdx++, op.token, sub_exp1.value, sub_exp2.value, new_tmp_var });

		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, new_tmp_var ,-1,-1,-1,-1 });
	}
}
// Factor : number \ ( Exp ) \ identifier \ CallStmt
void SemanticAnalysis::Semantic_Analysis_Factor(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 1) {
		SemanticSymbol exp = symbolList.back();
		// �����ID������Ƿ���й�����
		if (production_right[0] == "identifier") {
			bool existed = false;
			for (int scope_layer = currentTableStack.size() - 1; scope_layer >= 0; scope_layer--) {
				auto current_table = tables[currentTableStack[scope_layer]];
				if (current_table.FindSymbol(exp.value) != -1) {
					existed = true;
					break;
				}
			}
			if (existed == false) {
				cout << "��������з������󣺣�" << exp.row << "�У�" << exp.col << "�У�����" << exp.value << "δ����" << endl;
				throw(SEMANTIC_ERROR_UNDEFINED);
			}
		}

		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, exp.value ,exp.row,exp.col,exp.tableIdx,exp.idx });
	}
	else {
		SemanticSymbol exp = symbolList[symbolList.size() - 2];

		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, exp.value ,exp.row,exp.col,exp.tableIdx,exp.idx });
	}
}
// CallStmt : identifier ( CallFunCheck Args )
void SemanticAnalysis::Semantic_Analysis_CallStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol identifier = symbolList[symbolList.size() - 5];
	SemanticSymbol check = symbolList[symbolList.size() - 3];
	SemanticSymbol args = symbolList[symbolList.size() - 2];

	// ��麯���Ƿ��壨��CallFunCheckʱ�Ѿ���飩

	// ����������
	int para_num = tables[check.tableIdx].table[check.idx].funcParameterNum;
	if (para_num > stoi(args.value)) {
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "����ʱ��������" << endl;
		throw(SEMANTIC_ERROR_PARAMETER_NUM);
	}
	else if (para_num < stoi(args.value)) {
		cout << "��������з������󣺣�" << identifier.row << "�У�" << identifier.col << "�У�����" << identifier.value << "����ʱ��������" << endl;
		throw(SEMANTIC_ERROR_PARAMETER_NUM);
	}

	// ���ɺ���������Ԫʽ 
	string new_tmp_var = "T" + to_string(tmpVarCount++);
	quaternary.push_back({ nextQuaternaryIdx++, "call", identifier.value, "-", new_tmp_var });

	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	// �µ�exp��valueΪ��ʱ������
	symbolList.push_back({ production_left, new_tmp_var ,-1,-1,-1,-1 });
}
// CallFunCheck : @
void SemanticAnalysis::Semantic_Analysis_CallFunCheck(const string production_left, const vector<string> production_right)
{
	SemanticSymbol fun_id = symbolList[symbolList.size() - 2];

	int fun_id_pos = tables[0].FindSymbol(fun_id.value);

	if (-1 == fun_id_pos) {
		cout << "��������з������󣺣�" << fun_id.row << "�У�" << fun_id.col << "�У�����" << fun_id.value << "����δ����" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	if (tables[0].table[fun_id_pos].idType != IdentifierInfo::FUN) {
		cout << "��������з������󣺣�" << fun_id.row << "�У�" << fun_id.col << "�У�����" << fun_id.value << "����δ����" << endl;
		throw(SEMANTIC_ERROR_UNDEFINED);
	}
	symbolList.push_back({ production_left, fun_id.value,fun_id.row,fun_id.col, 0, fun_id_pos });
}
// Args : Exp , Args \ Exp \ @
void SemanticAnalysis::Semantic_Analysis_Args(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 3) {
		SemanticSymbol exp = symbolList[symbolList.size() - 3];
		quaternary.push_back({ nextQuaternaryIdx++, "param", exp.value, "-", "-" });
		int aru_num = stoi(symbolList.back().value) + 1;
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, to_string(aru_num),-1,-1,-1,-1 });
	}
	else if (production_right[0] == "Exp") {
		SemanticSymbol exp = symbolList.back();
		quaternary.push_back({ nextQuaternaryIdx++, "param", exp.value, "-", "-" });
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, "1",-1,-1,-1,-1 });
	}
	else if (production_right[0] == "@") {
		symbolList.push_back({ production_left, "0",-1,-1,-1,-1 });
	}
}
// ReturnStmt : return Exp \ return
void SemanticAnalysis::Semantic_Analysis_ReturnStmt(const string production_left, const vector<string> production_right)
{
	if (production_right.size() == 2) {
		// ����ֵ
		SemanticSymbol return_exp = symbolList.back();
		// ������
		SemanticSymbolTable function_table = tables[currentTableStack.back()];

		// �����Ԫʽ
		quaternary.push_back({ nextQuaternaryIdx++,"=",return_exp.value,"-",function_table.table[0].identifierName });

		// �����Ԫʽ
		quaternary.push_back({ nextQuaternaryIdx++ ,"return",function_table.table[0].identifierName,"-",function_table.table_name });

		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, return_exp.value,-1,-1,-1,-1 });
	}
	else {
		// ������
		SemanticSymbolTable function_table = tables[currentTableStack.back()];

		// ��麯���ķ���ֵ�Ƿ�Ϊvoid
		if (tables[0].table[tables[0].FindSymbol(function_table.table_name)].specifierType != "void") {
			cout << "��������з������󣺣�" << symbolList.back().row << "�У�" << symbolList.back().col + sizeof("return") << "�У�����" << function_table.table_name << "�����з���ֵ" << endl;
			throw(SEMANTIC_ERROR_NO_RETURN);
		}

		// �����Ԫʽ
		quaternary.push_back({ nextQuaternaryIdx++ ,"return","-","-",function_table.table_name });

		// ����symbolList
		int count = production_right.size();
		while (count--)
			symbolList.pop_back();
		symbolList.push_back({ production_left, "",-1,-1,-1,-1 });
	}
}
// Relop : > \ < \ >= \ <= \ == \ !=
void SemanticAnalysis::Semantic_Analysis_Relop(const string production_left, const vector<string> production_right)
{
	SemanticSymbol op = symbolList.back();

	int count = production_right.size();
	while (count--) {
		symbolList.pop_back();
	}
	symbolList.push_back({ production_left, op.token ,-1,-1,-1,-1 });
}
// IfStmt : if IfStmt_m1 ( Exp ) IfStmt_m2 Block IfNext
void SemanticAnalysis::Semantic_Analysis_IfStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol ifstmt_m2 = symbolList[symbolList.size() - 3];
	SemanticSymbol ifnext = symbolList[symbolList.size() - 1];

	if (ifnext.value.empty()) {
		// ֻ��ifû��else
		// �����
		quaternary[backpatchList.back()].result = ifstmt_m2.value;
		backpatchList.pop_back();

		// �ٳ���
		quaternary[backpatchList.back()].result = to_string(nextQuaternaryIdx);
		backpatchList.pop_back();
	}
	else {
		// if�����
		quaternary[backpatchList.back()].result = to_string(nextQuaternaryIdx);
		backpatchList.pop_back();
		// if�����
		quaternary[backpatchList.back()].result = ifstmt_m2.value;
		backpatchList.pop_back();
		// if�ٳ���
		quaternary[backpatchList.back()].result = ifnext.value;
		backpatchList.pop_back();
	}
	backpatchLevel--;

	// popback
	int count = production_right.size();
	while (count--)
		symbolList.pop_back();
	symbolList.push_back({ production_left,"",-1,-1,-1,-1 });
}
// IfStmt_m1 ::= @
void SemanticAnalysis::Semantic_Analysis_IfStmt_m1(const string production_left, const vector<string> production_right)
{
	backpatchLevel++;
	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}
// IfStmt_m2 : @
void SemanticAnalysis::Semantic_Analysis_IfStmt_m2(const string production_left, const vector<string> production_right)
{
	SemanticSymbol if_exp = symbolList[symbolList.size() - 2];

	// ��������Ԫʽ : �ٳ���
	quaternary.push_back({ nextQuaternaryIdx++,"j=",if_exp.value,"0","" });
	backpatchList.push_back(quaternary.size() - 1);

	// ��������Ԫʽ : �����
	quaternary.push_back({ nextQuaternaryIdx++,"j=","-","-","" });
	backpatchList.push_back(quaternary.size() - 1);

	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}
// IfNext : @ \ IfStmt_next else Block
void SemanticAnalysis::Semantic_Analysis_IfNext(const string production_left, const vector<string> production_right)
{
	SemanticSymbol if_stmt_next = symbolList[symbolList.size() - 3];

	int count = production_right.size();
	while (count--)
		symbolList.pop_back();

	symbolList.push_back({ production_left,if_stmt_next.value,-1,-1,-1,-1 });
}
// IfStmt_next : @
void SemanticAnalysis::Semantic_Analysis_IfStmt_next(const string production_left, const vector<string> production_right)
{
	// if ���������(else ֮ǰ)(������)
	quaternary.push_back({ nextQuaternaryIdx++,"j","-","-","" });
	backpatchList.push_back(quaternary.size() - 1);
	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}
// WhileStmt : while WhileStmt_m1 ( Exp ) WhileStmt_m2 Block
void SemanticAnalysis::Semantic_Analysis_WhileStmt(const string production_left, const vector<string> production_right)
{
	SemanticSymbol whilestmt_m1 = symbolList[symbolList.size() - 6];
	SemanticSymbol whilestmt_m2 = symbolList[symbolList.size() - 2];

	// ��������ת�� while �������ж���䴦
	quaternary.push_back({ nextQuaternaryIdx++,"j","-","-" ,whilestmt_m1.value });

	// ���������
	quaternary[backpatchList.back()].result = whilestmt_m2.value;
	backpatchList.pop_back();

	// ����ٳ���
	quaternary[backpatchList.back()].result = to_string(nextQuaternaryIdx);
	backpatchList.pop_back();

	backpatchLevel--;

	int count = production_right.size();
	while (count--)
		symbolList.pop_back();

	symbolList.push_back({ production_left,"",-1,-1,-1,-1 });

}
// WhileStmt_m1 : @
void SemanticAnalysis::Semantic_Analysis_WhileStmt_m1(const string production_left, const vector<string> production_right)
{
	backpatchLevel++;
	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}
// WhileStmt_m2 : @
void SemanticAnalysis::Semantic_Analysis_WhileStmt_m2(const string production_left, const vector<string> production_right)
{
	SemanticSymbol while_exp = symbolList[symbolList.size() - 2];

	// �ٳ���
	quaternary.push_back({ nextQuaternaryIdx++,"j=",while_exp.value,"0","" });
	backpatchList.push_back(quaternary.size() - 1);
	// �����
	quaternary.push_back({ nextQuaternaryIdx++ ,"j","-","-" ,"" });
	backpatchList.push_back(quaternary.size() - 1);

	symbolList.push_back({ production_left,to_string(nextQuaternaryIdx),-1,-1,-1,-1 });
}

