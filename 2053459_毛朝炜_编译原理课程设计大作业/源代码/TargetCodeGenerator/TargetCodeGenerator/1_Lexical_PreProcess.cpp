#include "1_Lexical_PreProcess.h"
#include <fstream>
#include <string>
/*******************************************************************/
//���ߺ���
/*******************************************************************/
//ɾ��str����Ŀո�
void delSideBlank(string& str) 
{
	int length = str.size();
	int left = 0;
	int right = length-1;
	for (; left < length; left++) {
		if (str[left] != ' ')
			break;
	}
	for (; right >= 0; right--) {
		if (str[right] != ' ')
			break;
	}
	str = str.substr(left,right-left+1);
}

//��str����seperator���ֵ�substr��
void divideRightStr(string str, char seperator,vector<string>& substr)
{
	int length = str.size();
	string temp;
	for (int i = 0; i < length; i++) {
		if (str[i] == seperator) {
			delSideBlank(temp);//ȥ���������˵Ŀո�
			substr.push_back(temp);
			temp.clear();
			continue;
		}
		temp += str[i];
	}
	delSideBlank(temp);
	substr.push_back(temp);

}

/*******************************************************************/
//CGrammer���Ա����ʵ��
/*******************************************************************/
CGrammer::CGrammer()
{
	//empty
}

//��ȡ�ļ��е��ķ�
void CGrammer::readGrammer(string filename)
{
	ifstream infile;
	infile.open(filename);
	if (!infile) {
		cout << "�ļ���ʧ��!" << endl;
		exit(-1);
	}

	string line, left, right;
	vector<string> left_and_right;
	while (getline(infile, line, '\n')) {
		//����ע��
		if (line[0] == EXEG_SIGN || line.size() == 0) {
			line.clear();
			continue;
		}
		right.clear();
		left.clear();
		left_and_right.clear();

		divideRightStr(line, DERI_SIGN, left_and_right);
		if (left_and_right.size() != 2) {
			cout << "����ʽ��ʽ����ȷ!" << endl;
			exit(-1);
		}
		left = left_and_right[0];
		right = left_and_right[1];

		//��ȡ�ս��
		if (left == "%VT") {
			vector<string> temp;
			divideRightStr(right, SEPE_SIGN, temp);
			for (vector<string>::iterator iter = temp.begin(); iter < temp.end(); iter++)
			{
				if (*iter != "")
					Terminals.insert(*iter);
			}
		}
		//��ȡ����ʽ
		else {
			vector<string> temp;
			divideRightStr(right, SEPE_SIGN, temp);
			int len = temp.size();//����ʽ����
			for (int i = 0; i < len; i++) {
				productItem tempItem;
				tempItem.left_symbol = left;
				divideRightStr(temp[i], AND_SIGN, tempItem.right_symbol);
				productItems.push_back(tempItem);//����һ������ʽ
				if (left.size() == 1 && left[0] == INIT_SIGN)
					startItemPos = productItems.size() - 1;//��ʼ����ʽ�ĽǱ�
			}
			//������ս��
			NTterminals.insert(left);
		}
		line.clear();
	}
	infile.close();
}


//��ӡ�ķ����ļ��У�������
void CGrammer::printGrammer(string filename) {
	ofstream outfile;
	outfile.open(filename);
	if (!outfile) {
		cout << "�ļ���ʧ��!" << endl;
		exit(-1);
	}
	outfile << "��ʼ����ʽλ�ã�" << this->startItemPos << endl;
	outfile << "�ս���� = { ";
	for (auto str : this->Terminals) {
		outfile << str << " ";
	}
	outfile << "}" << endl;

	outfile << "���ս���� = { ";
	for (auto str : this->NTterminals) {
		outfile << str << " ";
	}
	outfile << "}" << endl;

	outfile << "����ʽ��" << endl;
	int i = 0;
	for (auto pItem : this->productItems) {
		outfile << i << "��";
		i++;
		outfile << pItem.left_symbol << "-->";
		for (auto str : pItem.right_symbol) {
			outfile << str << " ";
		}
		outfile << endl;
	}
}
