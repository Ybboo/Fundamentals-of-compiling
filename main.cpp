//dot -Tpng nfa_graph.dot -o nfa_graph.png
#include <iostream>
#include <stack>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <fstream>

using namespace std;

//NFA�Ľڵ�
struct node
{
	string nodeName;
};

//NFA�ı�
struct edge
{
	node startName;	//��ʼ��
	node endName;	//Ŀ���
	char tranSymbol;	//ת������
};

//NFA����ɵ�Ԫ��һ�����NFA��Ԫ�������ɺܶ�С��Ԫͨ������ƴ������
struct elem
{
	int edgeCount;	//����
	edge edgeSet[100];	//��NFAӵ�еı�
	node startName;	//��ʼ״̬
	node endName; //����״̬
};


//�����½ڵ�
node new_node();

//���� a
elem act_Elem(char);

//����a|b
elem act_Unit(elem,elem);

//��ɵ�Ԫ��������
void elem_copy(elem&, elem);

//����ab
elem act_join(elem, elem);

//���� a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//������Ԫƴ����һ���൱��һ��+

class infixToPostfix {
public:
	infixToPostfix(const string& infix_expression);

	int is_letter(char check);
	int ispFunc(char c);
	int icpFunc(char c);
	void inToPost();
	string getResult();

private:
	string infix;
	string postfix;
	map<char, int> isp;
	map<char, int> icp;
};

elem express_to_NFA(string);

void Display(elem);

int is_letter(char check);

void generateDotFile_NFA(const elem& nfa);

int nodeNum = 0;

//�����½ڵ�
node new_node()
{
	node newNode;
	newNode.nodeName = nodeNum + 65;//�������ô�д��ĸ��ʾ
	nodeNum++;
	return newNode;
}

//��������������ʽ
void input(string& RE)
{
	cout << "������������ʽ������������() * |;�ַ�����a~z A~Z��" << endl;
	cin >> RE;
}


//��ɵ�Ԫ��������
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount+i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
}

//���� a
elem act_Elem(char c) 
{
	//�½ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//�±�
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//��NFA���Ԫ�أ�С��NFAԪ��/��Ԫ)
	elem newElem;
	newElem.edgeCount = 0;	//��ʼ״̬
	newElem.edgeSet[newElem.edgeCount++] = newEdge;
	newElem.startName = newElem.edgeSet[0].startName;
	newElem.endName = newElem.edgeSet[0].endName;
	
	return newElem;
}

//����a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;
	
	//����µ�״̬�ڵ�
	node startNode = new_node();
	node endNode = new_node();
	
	//����e1����������AB����ʼ��A��
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//����e2����������CD����ʼ��C��
	edge2.startName = startNode;
	edge2.endName  = sec.startName;
	edge2.tranSymbol = '#';

	//����e3������AB���յ���յ㣩
	edge3.startName = fir.endName;
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//����e4������CD���յ���յ㣩
	edge4.startName = sec.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//��fir��sec�ϲ�
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//�¹�����4����
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}


//���� N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//��fir�Ľ���״̬��sec�Ŀ�ʼ״̬�ϲ�����sec�ı߸��Ƹ�fir����fir����
	//��sec��������StartState��ͷ�ı�ȫ���޸�
	for (int i = 0; i < sec.edgeCount; i++) {
		if (sec.edgeSet[i].startName.nodeName.compare(sec.startName.nodeName) == 0)
		{
			sec.edgeSet[i].startName = fir.endName; //�ñ�e1�Ŀ�ʼ״̬����N(t)����ʼ״̬
		}
		else if (sec.edgeSet[i].endName.nodeName.compare(sec.startName.nodeName) == 0) {
			sec.edgeSet[i].endName = fir.endName; //�ñ�e2�Ľ���״̬����N(t)����ʼ״̬
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//��fir�Ľ���״̬����Ϊsec�Ľ���״̬
	fir.endName = sec.endName;
	return fir;
}

//*�հ�
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//�����״̬�ڵ�
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//�հ�ȡ�մ�

	//e2
	edge2.startName = Elem.endName;
	edge2.endName = Elem.startName;
	edge2.tranSymbol = '#';

	//e3
	edge3.startName = startNode;
	edge3.endName = Elem.startName;
	edge3.tranSymbol = '#';

	//e4
	edge4.startName = Elem.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//������Ԫ
	elem_copy(newElem, Elem);

	//���¹����������߼���EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//����NewElem����ʾ״̬�ͽ���״̬
	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}

int is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}
//������ӷ���
string add_join_symbol(string add_string)
{
	int length = add_string.size();
	int return_string_length = 0;
	char* return_string = new char[2 * length + 2];//���������
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//Ҫ�ӵĿ�������ab �� *b �� a( �� )b �����
		//���ڶ�������ĸ����һ������'('��'|'��Ҫ���
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//���ڶ�����'(',��һ������'|'��'(',ҲҪ��
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//�����һ���ַ�д��second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "��'+'��ı��ʽ��" << STRING << endl;
	return STRING;
}

//����ĸ���Ԫ�ض���
infixToPostfix::infixToPostfix(const string& infix_expression) : infix(infix_expression), postfix("") {
	isp = { {'+', 3}, {'|', 5}, {'*', 7},  {'(', 1}, {')', 8}, {'#', 0} };
	icp = { {'+', 2}, {'|', 4}, {'*', 6}, {'(', 8}, {')', 1}, {'#', 0} };
}

int infixToPostfix::is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}

int infixToPostfix::ispFunc(char c) {
	int priority = isp.count(c) ? isp[c] : -1;
	if (priority == -1) {
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: ����δ֪���ţ�" << endl;
		exit(1);  // �쳣�˳�
	}
	return priority;
}

void infixToPostfix::inToPost() {
	string infixWithHash = infix + "#";
	stack<char> stack;
	int loc = 0;
	while (!stack.empty() || loc < infixWithHash.size()) {
		if (is_letter(infixWithHash[loc])) {
			postfix += infixWithHash[loc];
			loc++;
		}
		else {
			char c1 = (stack.empty()) ? '#' : stack.top();
			char c2 = infixWithHash[loc];
			if (ispFunc(c1) < icpFunc(c2)) { // ջ�����������ȼ����ڵ�ǰ�ַ�������ǰ�ַ���ջ
				stack.push(c2);
				loc++;
			}
			else if (ispFunc(c1) > icpFunc(c2)) {  // ջ�����������ȼ����ڵ�ǰ�ַ�����ջ����������ջ����ӵ���׺���ʽ
				postfix += c1;
				stack.pop();
			}
			else {
				if (c1 == '#' && c2 == '#') { // �������� #�����ʽ����
					break;
				}
				stack.pop(); //��������������������ʱ������������ų�ջ�������Ų���ջ
				loc++;
			}
		}
	}
}

string infixToPostfix::getResult() {
	postfix = ""; // ��ս��
	inToPost();
	return postfix;
}

/**���ʽתNFA������,�������յ�NFA����
*/
elem express_to_NFA(string expression)
{
	int length = expression.size();
	char element;
	elem Elem, fir, sec;
	stack<elem> STACK;
	for (int i = 0; i < length; i++)
	{
		element = expression.at(i);
		switch (element)
		{
		case '|':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_Unit(fir, sec);
			STACK.push(Elem);
			break;
		case '*':
			fir = STACK.top();
			STACK.pop();
			Elem = act_star(fir);
			STACK.push(Elem);
			break;
		case '+':
			sec = STACK.top();
			STACK.pop();
			fir = STACK.top();
			STACK.pop();
			Elem = act_join(fir, sec);
			STACK.push(Elem);
			break;
		default:
			Elem = act_Elem(element);
			STACK.push(Elem);
		}
	}
	cout << "�ѽ�������ʽת��ΪNFA!" << endl;
	Elem = STACK.top();
	STACK.pop();

	return Elem;
}

//��ӡNFA
void Display(elem Elem) {
	cout << "NFA States:" << endl;
	cout << "Start State: " << Elem.startName.nodeName << endl;
	cout << "End State: " << Elem.endName.nodeName << endl;

	cout << "NFA Transitions:" << endl;
	for (int i = 0; i < Elem.edgeCount; i++) {
		cout << "Edge " << i + 1 << ": ";
		cout << Elem.edgeSet[i].startName.nodeName << " --(" << Elem.edgeSet[i].tranSymbol << ")--> ";
		cout << Elem.edgeSet[i].endName.nodeName << endl;
	}

	cout << "End" << endl;
}

//����NFAdot�ļ�
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  \n\n";
		dotFile << " node [shape = circle];   \n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// ��� NFA ״̬
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// ��� NFA ת��
		for (int i = 0; i < nfa.edgeCount; i++) {
			const edge& currentEdge = nfa.edgeSet[i];
			dotFile << "  " << currentEdge.startName.nodeName << " -> " << currentEdge.endName.nodeName << " [label=\"" << currentEdge.tranSymbol << "\"];\n";
		}

		dotFile << "}\n";

		dotFile.close();
		cout << "NFA DOT file generated successfully.\n";
	}
	else {
		cerr << "Unable to open NFA DOT file.\n";
	}
}

bool isAcceptedByNFA(const elem& nfa, const string& testString) {
    // ����ת����
    map<string, map<char, set<string>>> transitions; // transitions[start][symbol] = {end_states}
    map<string, set<string>> epsilonTransitions;    // ��ת�Ʊ� transitions[start] = {end_states}

    for (int i = 0; i < nfa.edgeCount; i++) {
        const edge& e = nfa.edgeSet[i];
        string start = e.startName.nodeName;
        string end = e.endName.nodeName;

        if (e.tranSymbol == '#') {
            epsilonTransitions[start].insert(end);  // ��ת��
        } else {
            transitions[start][e.tranSymbol].insert(end);  // �����ŵ�ת��
        }
    }

    // BFS �㷨����ַ����Ƿ� NFA ����
    queue<pair<string, int>> stateQueue; // �����е�Ԫ���� {��ǰ״̬, ��ƥ����ַ�λ��}
    set<pair<string, int>> visited;     // ��ֹ�ظ�����

    // ��ʼ�����кͷ��ʼ���
    stateQueue.push(make_pair(nfa.startName.nodeName, 0));
    visited.insert(make_pair(nfa.startName.nodeName, 0));

    while (!stateQueue.empty()) {
        pair<string, int> current = stateQueue.front();
        stateQueue.pop();

        string currentState = current.first;
        int pos = current.second;

        // ���������ֹ״̬�����ַ���ƥ���꣬����
        if (currentState == nfa.endName.nodeName && pos == testString.size()) {
            return true;
        }

        // ��ת��
        if (epsilonTransitions.count(currentState)) {
            for (const string& nextState : epsilonTransitions[currentState]) {
                if (visited.find(make_pair(nextState, pos)) == visited.end()) {
                    stateQueue.push(make_pair(nextState, pos));
                    visited.insert(make_pair(nextState, pos));
                }
            }
        }

        // ����ת��
        if (pos < testString.size() && transitions[currentState].count(testString[pos])) {
            for (const string& nextState : transitions[currentState][testString[pos]]) {
                if (visited.find(make_pair(nextState, pos + 1)) == visited.end()) {
                    stateQueue.push(make_pair(nextState, pos + 1));
                    visited.insert(make_pair(nextState, pos + 1));
                }
            }
        }
    }

    return false; // �������·�����޷������ַ���
}


int main() {
    // ����������ʽ������ NFA
    string Regular_Expression;
    elem NFA_Elem;
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)
        Regular_Expression = add_join_symbol(Regular_Expression);

    infixToPostfix Solution(Regular_Expression);
    cout << "��׺���ʽΪ��";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;

    NFA_Elem = express_to_NFA(Regular_Expression);
    Display(NFA_Elem);
    generateDotFile_NFA(NFA_Elem);

    // ��������ַ���
    int testCaseCount;
    cout << "����������ַ���������";
    cin >> testCaseCount;
    vector<string> testStrings(testCaseCount);
    cout << "����������ַ�����" << endl;
    for (int i = 0; i < testCaseCount; i++) {
        cin >> testStrings[i];
    }

    // �������ַ����Ƿ� NFA ����
    cout << "���Խ����" << endl;
    for (const string& testString : testStrings) {
        if (isAcceptedByNFA(NFA_Elem, testString)) {
            cout << "�ַ��� \"" << testString << "\" �� NFA ����" << endl;
        } else {
            cout << "�ַ��� \"" << testString << "\" δ�� NFA ����" << endl;
        }
    }

    return 0;
}