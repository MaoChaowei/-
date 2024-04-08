#include "2_Grammatical_LR1Grammer.h"
#include <fstream>
#include <iomanip>

bool LR1_item::operator==(const LR1_item& item)
{
	if (this->left_symbol != item.left_symbol) return false;
	if (this->dotPos != item.dotPos)return false;
	if (this->seek_symbol != item.seek_symbol)return false;
	if (this->right_symbol.size() != item.right_symbol.size())return false;
	for (int i = 0; i < this->right_symbol.size(); i++) {
		if (this->right_symbol[i] != item.right_symbol[i])
			return false;
	}
	return true;
}

bool LR1_item::operator<(const LR1_item& item) const
{
	if (this->left_symbol != item.left_symbol)
		return this->left_symbol < item.left_symbol;
	else {
		if (this->right_symbol.size() != item.right_symbol.size())
			return this->right_symbol.size() < item.right_symbol.size();
		else {
			for (int i = 0; i < this->right_symbol.size(); i++) {
				if (this->right_symbol[i] != item.right_symbol[i])
					return this->right_symbol[i] < item.right_symbol[i];
			}
		}
		if (this->seek_symbol != item.seek_symbol)
			return this->seek_symbol < item.seek_symbol;
		else
			return this->dotPos < item.dotPos;
	}
}

//��ӡ��Ŀ��
void LR1_grammer::printItems(set<LR1_item> inItems) {
	for (auto pitem : inItems) {
		cout << pitem.left_symbol << " -> ";
		int i = 0;
		for (auto r : pitem.right_symbol) {
			if (i++ == pitem.dotPos)
				cout << "��";
			cout << r << " ";
		}
		if(pitem.dotPos==pitem.right_symbol.size())
			cout << " �� ";
		cout << " --- " << pitem.seek_symbol << endl;

	}
	cout << "/****************************/" << endl;
}


// �ж��Ƿ�Ϊ������ͬ�ıհ�
bool LR1_grammer::isSameClosure(set<LR1_item>closure1, set<LR1_item>closure2) {
	if (closure1.size() != closure2.size())
		return false;
	int count = 0;
	for (auto tmp : closure1) {
		for (auto tmp_ : closure2) {
			if (tmp == tmp_) {
				++count;
				break;
			}
		}
	}
	return count == closure1.size();
}

// ������һ����Ŀ����״̬��
// ��ǰ�������ΪpresentStrʱ��״̬fromItemsתȥ��״̬toItems
void LR1_grammer::generateNextClosure(set<LR1_item> fromItems, set<LR1_item>& toItems, string presentStr) {
	if (presentStr == "#") // ������#
		return;
	for (auto lItem : fromItems) {   // ɨ����Ŀ��fromItems��ÿһ����Ŀ
		if (lItem.dotPos >= lItem.right_symbol.size()) // dot��ĩβ
			continue;
		if (lItem.right_symbol[lItem.dotPos] != presentStr) // dot��һ�����Ų���presentStr
			continue;
		// dot ����presentStr
		LR1_item temp = lItem;
		temp.dotPos = lItem.dotPos + 1;  // dot����һλ
		toItems.insert(temp);   // �Ѹ���Ŀ���뵽�µ���Ŀ��
	}
	getClosure(toItems, toItems);  // ������Ŀ���ıհ�
}

// ������Ŀ����
void LR1_grammer::generateClosureFamily() {
	set<LR1_item> startClosure;   // ��ʼ��Ŀ��
	LR1_item startItem;           // ��ʼ��Ŀ
	productItem startProductItem; // ��ʼ����ʽ
	startProductItem = this->grammer.productItems[this->grammer.startItemPos];

	// ���ݳ�ʼ����ʽ�����ʼ��Ŀ
	startItem.left_symbol = startProductItem.left_symbol;
	for (auto str : startProductItem.right_symbol) {
		startItem.right_symbol.push_back(str);
	}
	startItem.dotPos = 0;
	startItem.seek_symbol = "#";

	startClosure.insert(startItem);      // ����ʼ��Ŀ���뵽��ʼ��Ŀ��
	this->getClosure(startClosure, startClosure);  // ���ʼ��Ŀ�ıհ�
	closureFamily.push_back(startClosure); // ����ʼ��Ŀ���ıհ�������Ŀ����

	// ������Ŀ�����е�ÿһ����Ŀ��
	for (int i = 0; i < closureFamily.size(); i++) {
		// ��ÿ����Ŀ�� ���������ս��
		for (auto str : this->grammer.Terminals) {
			if (str == "#")  // ��Ȼ����Ҫ���� #
				continue;
			
			set<LR1_item> tempClosure;  // ����str����ǰ״̬��ת�Ƶ���һ����Ŀ��tempClosure
			generateNextClosure(closureFamily[i], tempClosure, str); // ������һ����Ŀ���ıհ�
			if (tempClosure.empty())   // ��һ����Ŀ��Ϊ�գ�˵����ǰ��Ŀ������������str��ת��
				continue;

			// �����Ŀ�������Ƿ��Ѿ�������ͬ��״̬
			int existIndex = -1;
			for (int j = 0; j < closureFamily.size(); j++) {
				if (isSameClosure(closureFamily[j], tempClosure)) {
					existIndex = j;
					break;
				}
			}
			if (existIndex != -1) {   // ������ͬ״̬����Ҫ����
				pair<int, string>in = pair<int, string>(i, str);
				if (LR1_table.count(in) == 0) {   // ���Action��Ķ�Ӧ�ƽ������Ƿ��г�ͻ
					// ��дLR1�������е��ƽ�����
					LR1_table[pair<int, string>(i, str)] = pair<string, int>("s", existIndex);
				}
				else 
					cout << "ERROR:��ڳ�ͻ!";
			}
			else {   // ��������ͬ״̬����Ҫ����
				closureFamily.push_back(tempClosure);// ����һ����Ŀ���ıհ�������Ŀ����

				pair<int, string>in = pair<int, string>(i, str);
				if (LR1_table.count(in) == 0) {   // ���Action��Ķ�Ӧ�ƽ������Ƿ��г�ͻ
					// ��дLR1�������е��ƽ�����
					LR1_table[pair<int, string>(i, str)] = pair<string, int>("s", closureFamily.size() - 1);
				}
				else 
					cout << "ERROR:��ڳ�ͻ!";
			}
		}
		// ��ÿ����Ŀ�� �������з��ս��
		for (auto str : this->grammer.NTterminals) {
			set<LR1_item> tempClosure;  // ����str����ǰ״̬��ת�Ƶ���һ����Ŀ��tempClosure
			generateNextClosure(closureFamily[i], tempClosure, str);  // ������һ����Ŀ���ıհ�
			if (tempClosure.empty())   // ��һ����Ŀ��Ϊ�գ�˵����ǰ��Ŀ������������str��ת��
				continue;

			// �����Ŀ�������Ƿ��Ѿ�������ͬ��״̬
			int existIndex = -1;
			for (int j = 0; j < closureFamily.size(); j++) {
				if (isSameClosure(closureFamily[j], tempClosure)) {
					existIndex = j;
					break;
				}
			}
			if (existIndex != -1) {  // ������ͬ״̬����Ҫ����
				pair<int, string>in = pair<int, string>(i, str);  
				if (LR1_table.count(in) == 0) {   // ���GOTO��Ķ�Ӧת�ƶ����Ƿ��г�ͻ
					// ��дLR1�������е��ƽ�����
					LR1_table[pair<int, string>(i, str)] = pair<string, int>("null", existIndex);
				}
				else 
					cout << "ERROR:��ڳ�ͻ!";
			}
			else {  // ��������ͬ״̬����Ҫ����
				closureFamily.push_back(tempClosure);  // ����һ����Ŀ���ıհ�������Ŀ����

				pair<int, string>in = pair<int, string>(i, str);
				if (LR1_table.count(in) == 0) {   // ���GOTO��Ķ�Ӧת�ƶ����Ƿ��г�ͻ
					// ��дLR1�������е��ƽ�����
					LR1_table[pair<int, string>(i, str)] = pair<string, int>("null", closureFamily.size() - 1);
				}
				else 
					cout << "ERROR:��ڳ�ͻ!";
			}
		}
	}
}

// �������ķ����ŵ�FIRST��
void LR1_grammer::generateFirst() {
	// ���ս����first��
	for (auto str : grammer.Terminals) {
		// �ս����FIRST����������
		set<string> firstSet;
		firstSet.insert(str);
		this->FIRST[str] = firstSet;
	}

	// ����ս����first��
	bool changed;
	while (true) {
		changed = false;
		// �������з��ս��
		for (auto ntStr : grammer.NTterminals) {
			// �������в���ʽ
			for (auto pItem : grammer.productItems) {
				// �������ʽ��߲�Ϊ��ǰ���ս��str���������˲���ʽ
				if (pItem.left_symbol != ntStr)
					continue;

				// �ҵ����ò���ʽ����������ʽ�Ҳ�
				int pos = 0;

				// ����ʽ�Ҳ���һ���������ս��������epsilon��
				if (grammer.Terminals.find(pItem.right_symbol[pos]) != grammer.Terminals.end()) {
					changed = this->FIRST[ntStr].insert(pItem.right_symbol[pos]).second || changed;
					continue;
				}

				// ����ʽ�Ҳ��Է��ս����ʼ
				bool flag = true; // ���Ƶ����մ��ı�־
				for (; pos < pItem.right_symbol.size(); pos++) {
					// ������ս������ֹͣ����
					if (grammer.Terminals.find(pItem.right_symbol[pos]) != grammer.Terminals.end()) {
						flag = false;
						changed = this->FIRST[ntStr].insert(pItem.right_symbol[pos]).second || changed;
						break;
					}

					// �Ƿ��ս������Ѹ÷��ս����epsilon�����first���ϲ���first[ntStr]
					set<string> temp = this->FIRST[pItem.right_symbol[pos]];
					temp.erase("@");   //ɾ��epsilon
					int size1 = this->FIRST[ntStr].size();
					this->FIRST[ntStr].insert(temp.begin(), temp.end());
					int size2 = this->FIRST[ntStr].size();
					changed = size2 > size1 ? (changed || 1) : (changed || 0);   // first���Ƿ����仯

					// �÷��ս���Ƿ����Ƴ��մ������ǣ����������
					flag = flag && this->FIRST[pItem.right_symbol[pos]].find("@") != this->FIRST[pItem.right_symbol[pos]].end();

					if (!flag)
						break;
				}

				// ����ʽ�Ҳ����з��Ŷ������Ƴ����ս������first������Epsilon
				if (flag && pos == pItem.right_symbol.size())
					changed = this->FIRST[ntStr].insert("@").second || changed;
			}
		}
		//���û�иı䣬˵����first���������˳�
		if (!changed)
			break;
	}
}

// ��հ�
void LR1_grammer::getClosure(set<LR1_item> inItems, set<LR1_item>& outItems)
{
	// cout << "***���뼯�ϣ�" << endl;
	// printItems(inItems); //////////////////////////////////////////////////////////////////////////////////////��ʱע��
	//�հ���������inItems�е���Ŀ
	vector<LR1_item> outvec;
	for (auto temp : inItems)
		outvec.push_back(temp);
	//outItems = inItems;
	int item_size = inItems.size();
	//��inItems��ÿһ������S->a.Bc ��x �Ĳ���ʽ
	for (int pitem = 0; pitem < outvec.size();pitem++) {
		//iter->right_symbol[iter->dotPos]�����ķ���
		//��Լ���ĩβ ֱ������
		if (outvec[pitem].dotPos == outvec[pitem].right_symbol.size())
			continue;
		//����ǿ����תΪ��Լ��Ŀ
		if (outvec[pitem].right_symbol[0][0] == grammer.NULL_SIGN) {
			outvec[pitem].dotPos++;
			continue;
		}
		//�������顤�����ǲ���NT
		if (grammer.NTterminals.find(outvec[pitem].right_symbol[outvec[pitem].dotPos]) != grammer.NTterminals.end()) {
			vector<string> cx;
			for (int i = outvec[pitem].dotPos + 1; i < outvec[pitem].right_symbol.size(); i++) {
				cx.push_back(outvec[pitem].right_symbol[i]);
			}
			cx.push_back(outvec[pitem].seek_symbol);
			for (auto pgram : grammer.productItems) {
				//Ѱ����pitem.right_symbol[pitem.dotPos]Ϊ�󲿵Ĳ���ʽ
				if (pgram.left_symbol == outvec[pitem].right_symbol[outvec[pitem].dotPos]) {
					//���ķ�productItems��ÿһ������ʽB->b
					set<string> colFirstSet = getStrFirstSet(cx);
					//{cx}��FIRST����ÿһ��Ԫ��t����B->b��t ���� outItems
					LR1_item temp;
					temp.left_symbol = pgram.left_symbol;
					temp.right_symbol.assign(pgram.right_symbol.begin(), pgram.right_symbol.end());
					temp.dotPos = (temp.right_symbol[0][0] == grammer.NULL_SIGN) ? 1 : 0;
					for (auto pfirst : colFirstSet) {
						temp.seek_symbol = pfirst;
						//�ж��Ƿ��Ѿ���outvec��
						bool is_push = 1;
						int vecsize = outvec.size();
						for (int i = 0; i < vecsize; i++) {
							if (outvec[i] == temp) {
								is_push = 0;
								break;
							}
						}
						if (is_push) {
							outvec.push_back(temp);
						//cout << temp.left_symbol << "->";
						//int cnt = 0;
						//for (auto t : temp.right_symbol) {
						//	if (cnt++ == temp.dotPos)
						//		cout << "��";
						//	cout << t << " ";
						//}
						//if (temp.dotPos == temp.right_symbol.size())
						//	cout << "��";
						//cout << "---" << temp.seek_symbol << endl;
						//
						}

					}
				}
			}
		}
	}
	for (auto i : outvec)
		outItems.insert(i);
	// cout << "***�հ����ϣ�" << endl;
	// printItems(outItems); 
}

// ��һ���ķ����Ŵ���FIRST��
// ��հ�ʱʹ�� ��Ϊ��������@ ����û����
set<string>  LR1_grammer::getStrFirstSet(vector<string> str)
{
	int num = str.size();
	set<string> strFirst;
	if (str.empty())
		return strFirst;
	//bool is_epsilon = true;

	//���α���ÿһ������
	for (auto itstr : str) {
		//�ս��
		if (grammer.Terminals.find(itstr) != grammer.Terminals.end()) {
			strFirst.insert(itstr);
			break;
		}
		//���ս��
		auto it = grammer.NTterminals.find(itstr);
		if (it != grammer.NTterminals.end()) {
			bool is_continue = 0;
			for (auto t : FIRST[*it]) {
				if (t[0] != grammer.NULL_SIGN)
					strFirst.insert(t);
				else
					is_continue = 1;
			}
			if (is_continue)
				continue;
			else
				break;
		}
		else {
			cout << "ERROR!";
			break;
		}
	}
	return strFirst;
}


// ��ӡfirst��
void LR1_grammer::printFirst() {
	for (auto str : grammer.Terminals) {
		cout << "FIRST(" << str << ") = { ";
		for (auto str1 : this->FIRST[str]) {
			cout << str1 << " ";
		}
		cout << "}" << endl;
	}
	for (auto str : grammer.NTterminals) {
		cout << "FIRST(" << str << ") = { ";
		for (auto str1 : this->FIRST[str]) {
			cout << str1 << " ";
		}
		cout << "}" << endl;
	}
}

void LR1_grammer::fillGuiYueTable()
{
	int setsize = closureFamily.size();
	for (int i = 0; i < setsize; i++) {
		for (auto it = closureFamily[i].begin(); it != closureFamily[i].end(); it++) {
			//��Լ��Ŀ
			if (it->right_symbol.size() == it->dotPos) {

				int gramNum = -1;
				int proSize = grammer.productItems.size();
				for (int t = 0; t < proSize; t++) {
					auto gram = grammer.productItems[t];
					if (gram.left_symbol == it->left_symbol && gram.right_symbol.size() == it->right_symbol.size()) {
						int nsize = gram.right_symbol.size();
						int n = 0;
						for (; n < nsize; n++) {
							if (gram.right_symbol[n] != it->right_symbol[n])
								break;
						}
						if (n == nsize) {
							gramNum = t;
							break;
						}
					}
				}
				if (gramNum == -1) {
					cout << "��Լ����!" << endl;
					exit(-1);
				}
				pair<int, string>in = pair<int, string>(i, it->seek_symbol);
				//ACC
				if (LR1_table.count(in) == 0) {//û�г�ͻ
					if (gramNum == grammer.startItemPos) {
						LR1_table[in] = pair<string, int>("acc", gramNum);
					}
					else {
						LR1_table[in] = pair<string, int>("r", gramNum);
					}
				}
				else {
					cout << "ERROR:��ڳ�ͻ!";
				}
			}
		}
	}
}

void LR1_grammer::printLR1Table(string filename)
{
	ofstream outfile;
	outfile.open(filename);
	if (!outfile) {
		cout << "�ļ���ʧ��!" << endl;
		exit(-1);
	}

	pair<int, string> in;
	int stateNum = closureFamily.size();
	int vtNum = grammer.Terminals.size();
	int nvtNum = grammer.NTterminals.size();
	int colNum = vtNum + nvtNum;

	outfile << setiosflags(ios::left) << setw(7) << " ";
	for (auto str : grammer.Terminals) {
		outfile << setiosflags(ios::left) << setw(10) << str;
	}
	for (auto str : grammer.NTterminals) {
		outfile << setiosflags(ios::left) << setw(10) << str;
	}
	outfile << endl;

	//���
	for (int row = 0; row < stateNum; row++) {
		outfile << setiosflags(ios::left) << setw(5) << row << ": ";
		auto pt = grammer.Terminals.begin();
		auto pnt = grammer.NTterminals.begin();

		for (int col = 0; col < colNum; col++) {
			string symbol;
			if (col < vtNum) {
				symbol = *pt;
				pt++;
			}
			else {
				symbol = *pnt;
				pnt++;
			}
			in = pair<int, string>(row, symbol);
			if (LR1_table.count(in) == 0) {//REJ
				outfile << setiosflags(ios::left) << setw(10) << " ";
			}
			else {
				if (LR1_table[in].first != "null") {
					string temp;
					if (LR1_table[in].first != "acc")
						temp = LR1_table[in].first + to_string(LR1_table[in].second);
					else
						temp = LR1_table[in].first;

					outfile << setiosflags(ios::left) << setw(10) << temp;
				}
				else
					outfile << setiosflags(ios::left) << setw(10) << LR1_table[in].second;
			}
		}
		outfile << endl;
	}
}
