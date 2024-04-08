#pragma once
#include <iostream>
#include <vector>
#include <set>

using namespace std;


//��C�����ķ��Ĳ���ʽ
struct productItem {
    string left_symbol;//����ʽ��ߵ�symbol
    vector<string> right_symbol;//����ʽ�ұߵ�symbol
};

//��C�����ķ���
class CGrammer {
public:
    vector<productItem> productItems;//���в���ʽ
    int startItemPos;//��ʼ����ʽλ��
    set<string> Terminals;//�����ս��
    set<string> NTterminals;//���з��ս��

    //������Ŷ���
    const char DERI_SIGN = ':';//�Ƶ���
    const char SEPE_SIGN = '\\';//�ָ���
    const char AND_SIGN = ' ';//��ѡʽ�ڲ����ӷ�
    const char EXEG_SIGN = '$';//ע�ͷ�
    const char INIT_SIGN = 'S';//��չ�ķ���ʼ��
    const char NULL_SIGN = '@';//�մ���

    CGrammer();
    void readGrammer(string filename);
    void printGrammer(string filename);
};