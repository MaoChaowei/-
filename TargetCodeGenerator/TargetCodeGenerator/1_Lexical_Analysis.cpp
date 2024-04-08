#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include "1_Lexical_Analysis.h"
using namespace std;

vector<TOKEN>lexAnswer;  // 1���ʷ��������Ľ��

// pathΪC����Դ�����ļ�·��
void lexAnalysis(const string path);  // ���ýӿڣ����ñ��������дʷ�����

void init();  // ���ر�����
//string preProcessing(string src);   // Դ����Ԥ����
int findReservedWord(string word);  // ���ұ����ֲ���������룬δ�ҵ�����-1
void scanner(string dst, int& pointer, ofstream& outfile);   // ɨ����
void readRealNumber(string src, int& pointer, string& dst, int& col);  // ���ֶ�ȡ

// ������
string reservedWord[] = { "(", ")", "{",  "}", ",", ";", "\"",
						  "=", "+", "-", "*", "/", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=",
						  ">", "<", ">=", "<=", "==", "!=", "||", "&&",
						  "else", "if", "int", "float", "double", "return", "void", "while" };

map<string, int>wordMap;   // �������������Ĺ�ϣ����

void lexAnalysis(const string path) {
	// --------------------
	// ��ȡԴ���� 
	// --------------------

	ifstream infile;
	ofstream outfile;
	infile.open(path, ios::in);
	if (!infile.is_open()) {
		cout << "�ļ���ʧ��~" << endl;
		exit(-1);
	}
	string src;
	//���ļ�����ȫ��д���ַ�����
	src.assign(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());

	// ------------------------
	// ɨ��������ʵĶ�Ԫ���б�
	// ------------------------

	init();
	outfile.open("../outfiles/1_Lexical_AnalysisAnswer.txt", ios::out | ios::trunc);
	if (!outfile.is_open()) {
		cout << "�ļ���ʧ��~" << endl;
		exit(-1);
	}
	int pointer = 0;  // Դ����ָ��
	while (pointer < src.length()) {
		scanner(src, pointer, outfile);
		if (!src[pointer]) break;
	}
	outfile.close();
}

// ���ر�����
void init() {
	int i = 0;
	for (auto word : reservedWord) {
		wordMap.insert(pair<string, int>(word, ++i));
	}
}


// ���ұ����ֲ���������룬δ�ҵ�����-1
int findReservedWord(string word) {
	auto iter = wordMap.find(word);
	return iter == wordMap.end() ? -1 : wordMap[word];
}

// ɨ�����������㷨
// ��������ֱ�������ֵ�Ķ�Ԫ��
void scanner(string dst, int& pointer, ofstream& outfile) {
	static int row = 1;
	static int col = 1;
	static bool lastIsIdentifier = false; // ��һ���Ƿ�Ϊ��ʶ���������ж϶���ʵ����ʱ�ġ�-���Ǹ��Ż��Ǽ���

	int coding = -1;   // ���ʱ��빤������
	string word = "";  // ���ʹ�������
	string attribute = "";  // ���Թ�������
	//preprocessing
	do {
		while (dst[pointer] == ' ' || dst[pointer] == '\r' || dst[pointer] == '\n') {
			if (dst[pointer] == ' ') {
				pointer++;
				col++;
			}
			else {
				row++;
				col = 1;
				pointer++;
			}
			if (pointer >= dst.size())
				return;
		}
		// ɾ����ע��
		if (dst[pointer] == '/' && ((pointer + 1) < dst.length()) && dst[pointer + 1] == '/') {
			for (; pointer < dst.length() && dst[pointer] != '\r' && dst[pointer] != '\n'; pointer++)
				col++;
			if (pointer >= dst.length())//ĩβ�򷵻�
				return;
			else {//�������޸�row col
				row++;
				col = 1;
				pointer++;
			}
		}
		if (dst[pointer] == '/' && ((pointer + 1) < dst.length()) && dst[pointer + 1] == '*') {
			for (; pointer < dst.length() && !(dst[pointer - 1] == '*' && dst[pointer] == '/'); pointer++) {
				if (dst[pointer] == '\r' || dst[pointer] == '\n') {
					row++;
					col = 1;
				}
				else
					col++;
			}
			if (pointer == dst.length()) {
				cerr << "Դ�������û���ҵ�\"*/\"!";
			}
			pointer++;
		}
	} while ((dst[pointer] == ' ' || dst[pointer] == '\r' || dst[pointer] == '\n')
		|| (dst[pointer] == '/' && ((pointer + 1) < dst.length()) && dst[pointer + 1] == '/')
		|| (dst[pointer] == '/' && ((pointer + 1) < dst.length()) && dst[pointer + 1] == '*'));


	if (isalpha(dst[pointer]) || isdigit(dst[pointer]) || dst[pointer] == '_') {  // ���֡���ĸ���»��� ��ͷ

		if (isalpha(dst[pointer]) || dst[pointer] == '_') {   // ��ĸ���»��� ��ͷ

			while (isalpha(dst[pointer]) || isdigit(dst[pointer]) || dst[pointer] == '_') {
				word += dst[pointer++];  //�ռ���Ȼ������
				col++;
			}

			coding = findReservedWord(word);   // ������Ǳ����֣������ֱ��룻

			attribute = coding == -1 ? word : "--";
			word = coding == -1 ? "identifier" : word;
		}
		else {   //���ֿ�ͷ��ʶ��ʵ������
			word = "number";
			readRealNumber(dst, pointer, attribute, col);
		}
	}
	else if (dst[pointer] == '\"') {  // �ַ�������
		pointer++;
		col++;
		word += '\"';
		while (dst[pointer] != '\"') {
			word += dst[pointer];
			pointer++;
			col++;
		}
		word += '\"';
		attribute = word;
		word = "const string";
		pointer++;
		col++;
	}
	else {  // ��ͷΪ�����ַ�
		word = dst[pointer];
		word += dst[pointer + 1];   // ��ǰ����
		if ((coding = findReservedWord(word)) != -1) {  // || != <= == >=
			pointer += 2;
			col += 2;
			attribute = "--";
		}
		else if (!lastIsIdentifier && (word[0] == '-' || word[0] == '.') && isdigit(word[1]))	// ˵���˴���-��Ϊ����
		{
			word = "number";
			readRealNumber(dst, pointer, attribute, col);
		}
		else {
			word = dst[pointer];
			if ((coding = findReservedWord(word)) != -1) {  // ( ) * + , - / ; < >
				pointer += 1;
				col += 1;
			}
			else
			{
				cout << "ERROR:" << endl;
				exit(EXIT_FAILURE);
			} // ������-------------------------------------------------------------
			attribute = "--";
		}
	}
	lastIsIdentifier = (attribute == "--") ? false : true;
	outfile << "< " << word << " , " << attribute << " >" << " ROW: " << row << " COL: " << col << endl;

	TOKEN token = { word,attribute,row,col };//***********************************TODO:ʵ�����м���
	lexAnswer.push_back(token);
}

// ��ȡʵ�����ĺ���
// ʵ���������ͣ�
// ʮ������ / �˽�������0��ͷ / ʮ����������0x��ͷ
// С�������ҽ���һ��С����
// ָ�������ҽ���һ��e����eǰ����������e���������
// ע����ΪС����ָ������ǰ��0��Ч����Ϊ��������ǰ��0ָʾ�˽���
// ע��С����������һ��e����e������С����
// ע����0xǰ��ʮ�����ƣ��򸺺�ֻ����0xǰ��
void readRealNumber(string src, int& pointer, string& dst, int& col)
{
	dst = "";
	if (src[pointer] == '-') {
		dst += src[pointer++];
		col++;
	}

	bool is_decimal = true;
	if (src[pointer] == '0')
	{
		dst += src[pointer++];
		col++;
		if (src[pointer] == 'x' || src[pointer] == 'X') // 16����
		{
			dst += src[pointer++];
			while (isdigit(src[pointer]) || (src[pointer] >= 'a' && src[pointer] <= 'f')
				|| (src[pointer] >= 'A' && src[pointer] <= 'F')) {
				dst += src[pointer++];
				col++;
			}
			is_decimal = false;
		}
		else // �޷�ȷ����ʮ���ƻ��ǰ˽��ƣ����ʹ��tmp_p
		{
			// ��������0-7�У��������ֽ��ƶ��ǺϷ�������
			while (src[pointer] >= '0' && src[pointer] <= '7') {
				dst += src[pointer++];
				col++;
			}

			// �����ֳ���0-7�ķ�Χ����ʱ��ȷ���⴮�����Ƿ���ʮ���ƣ�����ݴ�
			int tmp_p = pointer;
			string dst_tmp = dst;
			while (isdigit(src[tmp_p])) {
				dst_tmp += src[tmp_p++];
			}

			// ֻ������e��.������ȷ����ʮ���ƣ�������ǰ˽���
			if (src[tmp_p] == 'e' || src[tmp_p] == 'E' || src[tmp_p] == '.')
			{
				is_decimal = true;
				dst = dst_tmp;
				pointer = tmp_p;
			}
			else
				is_decimal = false;
		}
	}

	if (is_decimal)
	{
		while (isdigit(src[pointer])) {
			dst += src[pointer++];
			col++;
		}
		if (src[pointer] == 'e' || src[pointer] == 'E')
		{
			dst += src[pointer++];
			col++;
			if (src[pointer] == '-') {
				dst += src[pointer++];
				col++;
			}
			while (isdigit(src[pointer])) {
				dst += src[pointer++];
				col++;
			}
		}
		else if (src[pointer] == '.')
		{
			dst += src[pointer++];
			col++;
			while (isdigit(src[pointer])) {
				dst += src[pointer++];
				col++;
			}
			if (src[pointer] == 'e' || src[pointer] == 'E')
			{
				dst += src[pointer++];
				col++;
				if (src[pointer] == '-') {
					dst += src[pointer++];
					col++;
				}
				while (isdigit(src[pointer])) {
					dst += src[pointer++];
					col++;
				}
			}
		}
	}
}

/*
Դ����Ԥ����
����ո񡢻س����Ʊ������Ϊһ���ո�
ע�ͺ���:��ע�� ��ע��
*/
//string preProcessing(string src)
//{
//	int pointer = 0;//�ַ���ָ�룬ָ��ǰdst��ĩβλ��
//	int length = src.length();
//	string dst;
//
//	for (int i = 0; i < length; i++) {
//		//�����ַ�������
//		if (src[i] == '"') {
//			do {
//				pointer++;
//				dst += src[i++];
//			} while (src[i] != '"');
//			pointer++;
//			dst += src[i];
//		}
//		//�س������Ʊ� ��Ϊһ���ո�
//		else if (src[i] == ' ' || src[i] == '\r' || src[i] == '\n' || src[i] == '\t') {
//			//ǰһ��д��dst���ַ����ǿո����ܼ���ո�
//			if (pointer && dst[pointer - 1] != ' ') {
//				pointer++;
//				dst += ' ';
//			}
//		}
//		//ע��ɾ��
//		else if (src[i] == '/') {
//			if (i + 1 == length) {	//���һ��token�ǵ����ġ�/��
//				pointer++;
//				dst += '/';
//				break;
//			}
//			if (src[i + 1] == '/') {//��ע��
//				for (; i < length && src[i] != '\r' && src[i] != '\n'; i++)
//					;
//			}
//			else if (src[i + 1] == '*') {//��ע��
//				for (; i < length && !(src[i - 1] == '*' && src[i] == '/'); i++)
//					;
//				if (i == length) {
//					cerr << "Դ�������û���ҵ�\"*/\"!";
//				}
//			}
//			else {
//				pointer++;
//				dst += '/';
//			}
//		}
//		else {
//			dst += src[i];
//			pointer++;
//		}
//	}
//	return dst;
//}