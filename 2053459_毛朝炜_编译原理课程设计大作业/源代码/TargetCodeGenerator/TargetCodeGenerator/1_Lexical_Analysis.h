#pragma once
#include <iostream>
#include <string>

using namespace std;

//���������ʷ��������(tokens)�����ݽṹ
struct TOKEN
{
	string token_key;//���ͣ������֡���ʶ����number...
	string value;//ֵ

	//Դ�ļ��е�λ�ã����ڳ�����Ķ�λ
	int row;
	int col;
};