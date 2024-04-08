#include "2_Grammatical_Analysis.h"
#include "2_Grammatical_LR1Grammer.h"
#include <stack>
#include <iomanip>

extern void lexAnalysis(const string path);           // �ʷ������ӿں���
extern vector<TOKEN>lexAnswer;						 
// ��������
ThreadedList* grammaticalAnalysis( map<TABLE_KEY_TYPE, TABLE_VALUE_TYPE>& Table, vector<ITEM_TYPE>& Items, SemanticAnalysis& Semantic)
{
	cout << "**���ôʷ��������̣�" << endl;
	lexAnalysis("../infiles/test2.txt");               // �ʷ�����

	for (auto tempPair : lexAnswer) {                          // ��ӡ�ʷ������õ����ķ���������
		cout << setw(15) << tempPair.token_key << " " << setw(15) << tempPair.value <<
			setw(10) << " ROW:" << setw(3) << tempPair.row << setw(10) << " COL:" << setw(3) << tempPair.col << endl;
	}

	vector<ANSWER_TYPE>&  Answer = lexAnswer;

	//�����������
	Semantic = SemanticAnalysis();
	Semantic.AddSymbol2List({ "Program","",-1,-1,-1,-1 });

	TOKEN temp = { "#", "--", -1, -1 };
	Answer.push_back(temp);

	vector<int>statusStack;					// ״̬ջ
	vector<string>symbolsStack;				// ����ջ
	vector<ThreadedList*>threadNodeStack;   // ���߱�ջ�����ڻ�ͼ
	int answerPointer = 0;					// �ַ���ָ��
	int regulationStepCount = 0;			// ��ǰ���е��ڼ��ι�Լ

	TABLE_VALUE_TYPE tableValue;			// ��Table��ȡ�����ݴ浽����
	string action;							// ��һ���裬��tableValue.first

	statusStack.push_back(0);					// ��ʼ״̬Ϊ״̬0
	string symbol;
	while (true)
	{
		symbol = Answer[answerPointer].token_key;
		pair<int, string>cur(statusStack.back(), symbol); // ����map����
		tableValue = Table[cur];			  // ��action����ȡ��һ������

		string action = tableValue.first;
		if (action == "acc")		// ��Լ�ɹ�
		{
			if (threadNodeStack.size() == 1)
				return threadNodeStack[0];
			else
			{
				deleteThreadedList(threadNodeStack[0]);
				return nullptr;
			}
		}
		else if (action == "")	// ��Լʧ��
		{
			deleteThreadedList(threadNodeStack[0]);
			return nullptr;
		}
		else if (action == "s")		// �ƽ�
		{
			string curSymbol = Answer[answerPointer].token_key;
			symbolsStack.push_back(curSymbol);			       // �����봮��ǰ�ַ�ѹ�����ջ�����ƶ�ָ��
			statusStack.push_back(tableValue.second);

			ThreadedList* Node = new(nothrow)ThreadedList;     // �����µĴ��߱���
			if (!Node)
			{
				deleteThreadedList(threadNodeStack[0]);
				exit(OVERFLOW);
			}
			if (curSymbol == "identifier")		// ���������ս������˲����ڹ�Լʱ�ٿ���֮
				Node->IdIfExists = Answer[answerPointer].value; // ���ڻ����﷨��
			Node->symbol = curSymbol;

			threadNodeStack.push_back(Node);

			//��token������������ķ��Ŵ���
			Semantic.AddSymbol2List({ Answer[answerPointer].token_key,Answer[answerPointer].value,Answer[answerPointer].row,Answer[answerPointer].col,-1,-1 });

			answerPointer++;
		}
		else if (action == "r")		// ��Լ
		{
			++regulationStepCount;			// ��Լ����+1

			int regulationTerm = tableValue.second;								// ѡ����Լ��
			string regulationLeft = Items[regulationTerm].left_symbol;			// ��Լ�󲿣�Ψһ

			int regulationRightLength = Items[regulationTerm].right_symbol.size();	// �Ҳ�����

			if (statusStack.size() < regulationRightLength)
			{
				cout << "ERROR: NOT ENOUGH STATUS!" << endl;
				deleteThreadedList(threadNodeStack[0]);
				exit(EXIT_FAILURE);
			}

			if (Items[regulationTerm].right_symbol[0] == "@")							// �մ�����
				regulationRightLength = 0;

			ThreadedList* Node = new(nothrow)ThreadedList(regulationStepCount);     // �����µĴ��߱��㣬�˽��Ϊ��ǰ��Լʹ�õĲ���ʽ��
			if (!Node)
			{
				deleteThreadedList(threadNodeStack[0]);
				exit(OVERFLOW);
			}
			Node->symbol = regulationLeft;

			for (int i = 0; i < regulationRightLength; ++i)  // ��������Լ���Ҳ������Ӵ��߱�
			{
				symbolsStack.pop_back();
				statusStack.pop_back();

				Node->threads.push_back(threadNodeStack.back());
				threadNodeStack.pop_back();
			}

			symbolsStack.push_back(regulationLeft);				// ѹ����
			threadNodeStack.push_back(Node);					// �����Ӻõķ��ս�����ѹ�ش��߱���ջ

			cur = pair<int, string>(statusStack.back(), symbolsStack.back()); // ����map����
			tableValue = Table[cur];			     // ���(goto)����ȡ��ѹ���״̬
			statusStack.push_back(tableValue.second);

			//�������
			Semantic.Analysis(Items[regulationTerm].left_symbol, Items[regulationTerm].right_symbol);

		}
		else // �����ַ�
		{
			cout << "ERROR: UNKNOWN OPERATION OF LR1TABLE!" << endl;
			deleteThreadedList(threadNodeStack[0]);
			exit(EXIT_FAILURE);
		}
	}
}

// ɾ�����߱�
void deleteThreadedList(ThreadedList* Root)
{
	// �ǵݹ������ʹ��ջ����
	stack<ThreadedList*>threadedListStack;
	threadedListStack.push(Root);

	while (!threadedListStack.empty())
	{
		ThreadedList* Node = threadedListStack.top();
		threadedListStack.pop();
		for (int i = 0; i < Node->threads.size(); ++i)
			threadedListStack.push(Node->threads[i]);
		delete Node;
	}
}