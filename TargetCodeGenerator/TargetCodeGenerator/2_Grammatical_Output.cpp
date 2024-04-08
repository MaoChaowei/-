#include "2_Grammatical_Analysis.h"
#include "2_Grammatical_LR1Grammer.h"
#include <fstream>
#include <queue>

#define QUEUE_TYPE pair<ThreadedList*, int>

void addNode(ofstream& outfile, int cnt, ThreadedList* Node);
void addEdge(ofstream& outfile, int start, int end);
void addIdentifier(ofstream& outfile, int cnt, QUEUE_TYPE identifier);

string dotPath = "../outfiles/syntaxTree.dot";
string pngPath = "../outfiles/syntaxTree.png";
// ������dot�ű����Թ淶�Ĺ����������.dot�ļ���
// �ٵ��ýӿں���ת���ɹ������ͼƬ
void generateTreeDot(ThreadedList* Root) {
	ofstream outfile(dotPath, ios::out);
	outfile << "digraph all{" << endl;
	outfile << "    rankdir = LR" << endl;
	outfile << "    node[shape = ellipse, fontname = \"Times-Roman\"]" << endl;
	outfile << "    edge[arrow = 0.5]" << endl;

	// ʹ��label���ԣ������ظ����
	// �����������������в�α�����Ϊÿ����㴴��һ��node
	// ʹ��nullptr��Ϊ�����ʶ
	int nodeCount = 0;

	queue <QUEUE_TYPE> threadedListQueue;
	threadedListQueue.push(QUEUE_TYPE(Root, 0));
	addNode(outfile, nodeCount, Root);				// д������

	while (!threadedListQueue.empty())
	{
		// ÿ�ε������ף�������չ���ӣ��ٽ�ѹ�����
		// ��չʱ������ӽ�����������������
		QUEUE_TYPE elem = threadedListQueue.front();
		threadedListQueue.pop();
		if (elem.first->IdIfExists != "")
		{
			++nodeCount;		// ����µĽ��ǰ����Ҫcount����
			addIdentifier(outfile, nodeCount, elem);
		}

		for (int i = 0; i < elem.first->threads.size(); ++i)
		{
			++nodeCount;		// ����µĽ��ǰ����Ҫcount����
			QUEUE_TYPE newNode(elem.first->threads[i], nodeCount);

			addNode(outfile, nodeCount, newNode.first);
			addEdge(outfile, elem.second, nodeCount);
			threadedListQueue.push(newNode);
		}
	}
	outfile << "}" << endl;
	outfile.close();

	string command;
	command += "dot -Tpng " + dotPath + " -o " + pngPath;
	system(command.c_str());
}

void addIdentifier(ofstream& outfile, int cnt, QUEUE_TYPE identifier)
{
	outfile << "node" << to_string(cnt) << '[' << "label = " << '\"';
	outfile << identifier.first->IdIfExists;
	outfile << '\"' << ']' << ';' << endl;

	outfile << "node" << identifier.second << "->" << "node" << cnt;
	outfile << "[style = \"dotted\", arrowtail = \"none\", arrowhead = \"none\"]" << ';' << endl;
}

void addNode(ofstream &outfile, int cnt, ThreadedList* Node)
{
	outfile << "node" << to_string(cnt) << '[' << "label = " << '\"';

	if (Node->regulationStep)
		outfile << '(' << Node->regulationStep << ')' << ' ';
	outfile << Node->symbol;

	outfile << '\"' << ']' << ';' << endl;
}

void addEdge(ofstream& outfile, int start, int end)
{
	outfile << "node" << start << "->" << "node" << end << ';' << endl;
}