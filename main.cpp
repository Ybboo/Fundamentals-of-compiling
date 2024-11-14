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

//NFA的节点
struct node
{
	string nodeName;
};

//NFA的边
struct edge
{
	node startName;	//起始点
	node endName;	//目标点
	char tranSymbol;	//转换符号
};

//NFA的组成单元，一个大的NFA单元可以是由很多小单元通过规则拼接起来
struct elem
{
	int edgeCount;	//边数
	edge edgeSet[100];	//该NFA拥有的边
	node startName;	//开始状态
	node endName; //结束状态
};


//创建新节点
node new_node();

//处理 a
elem act_Elem(char);

//处理a|b
elem act_Unit(elem,elem);

//组成单元拷贝函数
void elem_copy(elem&, elem);

//处理ab
elem act_join(elem, elem);

//处理 a*
elem act_star(elem);

void input(string&);

string add_join_symbol(string);	//两个单元拼接在一起相当于一个+

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

//创建新节点
node new_node()
{
	node newNode;
	newNode.nodeName = nodeNum + 65;//将名字用大写字母表示
	nodeNum++;
	return newNode;
}

//接收输入正规表达式
void input(string& RE)
{
	cout << "请输入正则表达式：（操作符：() * |;字符集：a~z A~Z）" << endl;
	cin >> RE;
}


//组成单元拷贝函数
void elem_copy(elem& dest, elem source)
{
	for (int i = 0; i < source.edgeCount; i++) {
		dest.edgeSet[dest.edgeCount+i] = source.edgeSet[i];
	}
	dest.edgeCount += source.edgeCount;
}

//处理 a
elem act_Elem(char c) 
{
	//新节点
	node startNode = new_node();
	node endNode = new_node();

	//新边
	edge newEdge;
	newEdge.startName = startNode;
	newEdge.endName = endNode;
	newEdge.tranSymbol = c;

	//新NFA组成元素（小的NFA元素/单元)
	elem newElem;
	newElem.edgeCount = 0;	//初始状态
	newElem.edgeSet[newElem.edgeCount++] = newEdge;
	newElem.startName = newElem.edgeSet[0].startName;
	newElem.endName = newElem.edgeSet[0].endName;
	
	return newElem;
}

//处理a|b
elem act_Unit(elem fir, elem sec)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;
	
	//获得新的状态节点
	node startNode = new_node();
	node endNode = new_node();
	
	//构建e1（连接起点和AB的起始点A）
	edge1.startName = startNode;
	edge1.endName = fir.startName;
	edge1.tranSymbol = '#';

	//构建e2（连接起点和CD的起始点C）
	edge2.startName = startNode;
	edge2.endName  = sec.startName;
	edge2.tranSymbol = '#';

	//构建e3（连接AB的终点和终点）
	edge3.startName = fir.endName;
	edge3.endName = endNode;
	edge3.tranSymbol = '#';

	//构建e4（连接CD的终点和终点）
	edge4.startName = sec.endName;
	edge4.endName = endNode;
	edge4.tranSymbol = '#';

	//将fir和sec合并
	elem_copy(newElem, fir);
	elem_copy(newElem, sec);

	//新构建的4条边
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}


//处理 N(s)N(t)
elem act_join(elem fir, elem sec)
{
	//将fir的结束状态和sec的开始状态合并，将sec的边复制给fir，将fir返回
	//将sec中所有以StartState开头的边全部修改
	for (int i = 0; i < sec.edgeCount; i++) {
		if (sec.edgeSet[i].startName.nodeName.compare(sec.startName.nodeName) == 0)
		{
			sec.edgeSet[i].startName = fir.endName; //该边e1的开始状态就是N(t)的起始状态
		}
		else if (sec.edgeSet[i].endName.nodeName.compare(sec.startName.nodeName) == 0) {
			sec.edgeSet[i].endName = fir.endName; //该边e2的结束状态就是N(t)的起始状态
		}
	}
	sec.startName = fir.endName;

	elem_copy(fir, sec);

	//将fir的结束状态更新为sec的结束状态
	fir.endName = sec.endName;
	return fir;
}

//*闭包
elem act_star(elem Elem)
{
	elem newElem;
	newElem.edgeCount = 0;
	edge edge1, edge2, edge3, edge4;

	//获得新状态节点
	node startNode = new_node();
	node endNode = new_node();

	//e1
	edge1.startName = startNode;
	edge1.endName = endNode;
	edge1.tranSymbol = '#';	//闭包取空串

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

	//构建单元
	elem_copy(newElem, Elem);

	//将新构建的四条边加入EdgeSet
	newElem.edgeSet[newElem.edgeCount++] = edge1;
	newElem.edgeSet[newElem.edgeCount++] = edge2;
	newElem.edgeSet[newElem.edgeCount++] = edge3;
	newElem.edgeSet[newElem.edgeCount++] = edge4;

	//构建NewElem的启示状态和结束状态
	newElem.startName = startNode;
	newElem.endName = endNode;

	return newElem;
}

int is_letter(char check) {
	if (check >= 'a' && check <= 'z' || check >= 'A' && check <= 'Z')
		return true;
	return false;
}
//添加连接符号
string add_join_symbol(string add_string)
{
	int length = add_string.size();
	int return_string_length = 0;
	char* return_string = new char[2 * length + 2];//最多是两倍
	char first, second;
	for (int i = 0; i < length - 1; i++)
	{
		first = add_string.at(i);
		second = add_string.at(i + 1);
		return_string[return_string_length++] = first;
		//要加的可能性如ab 、 *b 、 a( 、 )b 等情况
		//若第二个是字母、第一个不是'('、'|'都要添加
		if (first != '(' && first != '|' && is_letter(second))
		{
			return_string[return_string_length++] = '+';
		}
		//若第二个是'(',第一个不是'|'、'(',也要加
		else if (second == '(' && first != '|' && first != '(')
		{
			return_string[return_string_length++] = '+';
		}
	}
	//将最后一个字符写入second
	return_string[return_string_length++] = second;
	return_string[return_string_length] = '\0';
	string STRING(return_string);
	cout << "加'+'后的表达式：" << STRING << endl;
	return STRING;
}

//类里的各类元素定义
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
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
	}
	return priority;
}

int infixToPostfix::icpFunc(char c) {
	int priority = icp.count(c) ? icp[c] : -1;
	if (priority == -1) {
		cerr << "error: 出现未知符号！" << endl;
		exit(1);  // 异常退出
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
			if (ispFunc(c1) < icpFunc(c2)) { // 栈顶操作符优先级低于当前字符，将当前字符入栈
				stack.push(c2);
				loc++;
			}
			else if (ispFunc(c1) > icpFunc(c2)) {  // 栈顶操作符优先级高于当前字符，将栈顶操作符出栈并添加到后缀表达式
				postfix += c1;
				stack.pop();
			}
			else {
				if (c1 == '#' && c2 == '#') { // 遇到两个 #，表达式结束
					break;
				}
				stack.pop(); //其中右括号遇到左括号时会抵消，左括号出栈，右括号不入栈
				loc++;
			}
		}
	}
}

string infixToPostfix::getResult() {
	postfix = ""; // 清空结果
	inToPost();
	return postfix;
}

/**表达式转NFA处理函数,返回最终的NFA集合
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
	cout << "已将正则表达式转换为NFA!" << endl;
	Elem = STACK.top();
	STACK.pop();

	return Elem;
}

//打印NFA
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

//生成NFAdot文件
void generateDotFile_NFA(const elem& nfa) {
	std::ofstream dotFile("nfa_graph.dot");

	if (dotFile.is_open()) {
		dotFile << "digraph NFA {\n";
		dotFile << "  rankdir=LR;  \n\n";
		dotFile << " node [shape = circle];   \n\n";

		dotFile << nfa.endName.nodeName << " [shape=doublecircle];\n";
		// 添加 NFA 状态
		dotFile << "  " << nfa.startName.nodeName << " [label=\"Start State: " << nfa.startName.nodeName << "\"];\n";
		dotFile << "  " << nfa.endName.nodeName << " [label=\"End State: " << nfa.endName.nodeName << "\"];\n";

		// 添加 NFA 转移
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
    // 构建转换表
    map<string, map<char, set<string>>> transitions; // transitions[start][symbol] = {end_states}
    map<string, set<string>> epsilonTransitions;    // 空转移表 transitions[start] = {end_states}

    for (int i = 0; i < nfa.edgeCount; i++) {
        const edge& e = nfa.edgeSet[i];
        string start = e.startName.nodeName;
        string end = e.endName.nodeName;

        if (e.tranSymbol == '#') {
            epsilonTransitions[start].insert(end);  // 空转移
        } else {
            transitions[start][e.tranSymbol].insert(end);  // 带符号的转移
        }
    }

    // BFS 算法检测字符串是否被 NFA 接受
    queue<pair<string, int>> stateQueue; // 队列中的元素是 {当前状态, 已匹配的字符位置}
    set<pair<string, int>> visited;     // 防止重复访问

    // 初始化队列和访问集合
    stateQueue.push(make_pair(nfa.startName.nodeName, 0));
    visited.insert(make_pair(nfa.startName.nodeName, 0));

    while (!stateQueue.empty()) {
        pair<string, int> current = stateQueue.front();
        stateQueue.pop();

        string currentState = current.first;
        int pos = current.second;

        // 如果到达终止状态并且字符串匹配完，接受
        if (currentState == nfa.endName.nodeName && pos == testString.size()) {
            return true;
        }

        // 空转移
        if (epsilonTransitions.count(currentState)) {
            for (const string& nextState : epsilonTransitions[currentState]) {
                if (visited.find(make_pair(nextState, pos)) == visited.end()) {
                    stateQueue.push(make_pair(nextState, pos));
                    visited.insert(make_pair(nextState, pos));
                }
            }
        }

        // 符号转移
        if (pos < testString.size() && transitions[currentState].count(testString[pos])) {
            for (const string& nextState : transitions[currentState][testString[pos]]) {
                if (visited.find(make_pair(nextState, pos + 1)) == visited.end()) {
                    stateQueue.push(make_pair(nextState, pos + 1));
                    visited.insert(make_pair(nextState, pos + 1));
                }
            }
        }
    }

    return false; // 如果所有路径均无法接受字符串
}


int main() {
    // 输入正则表达式并生成 NFA
    string Regular_Expression;
    elem NFA_Elem;
    input(Regular_Expression);
    if (Regular_Expression.length() > 1)
        Regular_Expression = add_join_symbol(Regular_Expression);

    infixToPostfix Solution(Regular_Expression);
    cout << "后缀表达式为：";
    Regular_Expression = Solution.getResult();
    cout << Regular_Expression << endl;

    NFA_Elem = express_to_NFA(Regular_Expression);
    Display(NFA_Elem);
    generateDotFile_NFA(NFA_Elem);

    // 输入测试字符串
    int testCaseCount;
    cout << "请输入测试字符串数量：";
    cin >> testCaseCount;
    vector<string> testStrings(testCaseCount);
    cout << "请输入测试字符串：" << endl;
    for (int i = 0; i < testCaseCount; i++) {
        cin >> testStrings[i];
    }

    // 检测测试字符串是否被 NFA 接受
    cout << "测试结果：" << endl;
    for (const string& testString : testStrings) {
        if (isAcceptedByNFA(NFA_Elem, testString)) {
            cout << "字符串 \"" << testString << "\" 被 NFA 接受" << endl;
        } else {
            cout << "字符串 \"" << testString << "\" 未被 NFA 接受" << endl;
        }
    }

    return 0;
}