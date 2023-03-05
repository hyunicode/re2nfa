/**
 * @file re2nfa.cpp
 * @brief Convert regular expression to NFA
 * @param[in] expression Regular expression (txt file)
 * @param[out] nfa NFA (markdown file)
 */
#include <bits/stdc++.h>
using namespace std;

struct edge
{
    int beginID = -1;
    char trans;
    int endID = -1;
};

vector<edge> nfa; // To store the NFA by edge

// dectect if the input is a legal regular expression
bool isLegal(string s)
{
    stack<char> bracket; // To check if the brackets are matched
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] >= 'a' && s[i] <= 'z') // If letter
            continue;
        else if (s[i] == '|' || s[i] == '*' || s[i] == '.') // If operator
            continue;
        else if (s[i] == '(') // Check if the brackets are matched
            bracket.push('(');
        else if (s[i] == ')')
        {
            if (bracket.empty())
                return false;
            else
                bracket.pop();
        }
        else
            return false;
    }
    if (!bracket.empty())
        return false;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] == '|') //'|' can't be the first or last character,and can't be followed by '|','*',')'and '.'
        {
            if (i == 0 || i == s.length() - 1)
                return false;
            else if (s[i + 1] == '|' || s[i + 1] == '*' || s[i + 1] == '.' || s[i + 1] == ')')
                return false;
        }
        else if (s[i] == '.') //'.' can't be the first or last character,and can't be followed by '|','*',')'and '.'
        {
            if (i == 0 || i == s.length() - 1)
                return false;
            else if (s[i + 1] == '|' || s[i + 1] == '*' || s[i + 1] == '.' || s[i + 1] == ')')
                return false;
        }
        else if (s[i] == '*') //'*' can't be the first character,and can't be followed by'*'
        {
            if (i == 0)
                return false;
            else if (s[i + 1] == '*')
                return false;
        }
    }
    return true;
}

int precedence(char c)
{
    if (c == '*')
    {
        return 3;
    }
    else if (c == '.')
    {
        return 2;
    }
    else if (c == '|')
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

string toinfix(string expression)
{
    string infix;
    for (int i = 0; i < expression.length(); i++)
    {
        char tmp = expression[i];
        char next;
        if (i == expression.length() - 1)
        {
            next = '\0'; //结束
        }
        else
        {
            next = expression[i + 1];
        }
        if (((tmp != '(' && tmp != '.' && tmp != '|') || tmp == ')' || tmp == '*') && (next != ')' && next != '*' && next != '|' && next != '.' && next != '\0'))
        {
            infix = infix + tmp + '.';
        }
        else
        {
            infix = infix + tmp;
        }
    }
    return infix;
}

string tosuffix(string infix)
{
    stack<char> op; // To store operators((),.,*)
    string suffix;
    for (int i = 0; i < infix.length(); i++)
    {
        char tmp = infix[i];
        if (tmp == '(')
        {
            op.push(tmp);
        }
        else if (tmp == ')')
        {
            while (op.top() != '(')
            {
                suffix = suffix + op.top();
                op.pop();
            }
            op.pop();
        }
        else if (tmp == '*' || tmp == '.' || tmp == '|')
        {
            while (!op.empty() && op.top() != '(' && precedence(tmp) <= precedence(op.top()))
            {
                suffix = suffix + op.top();
                op.pop();
            }
            op.push(tmp);
        }
        else
        {
            suffix = suffix + tmp;
        }
    }
    while (!op.empty())
    {
        suffix = suffix + op.top();
        op.pop();
    }
    return suffix;
}

void Tonfa(string suffix)
{
    vector<edge>().swap(nfa); // Initialize nfa
    int ID = -1;
    stack<int> beginst; // To store the beginID of the edge
    stack<int> endst;   // To store the endID of the edge
    for (int i = 0; i < suffix.length(); i++)
    {
        char tmp = suffix[i];
        if (tmp >= 97 && tmp <= 122) // Current character is a letter,create a new edge
        {
            ID++;
            edge e;
            e.trans = tmp;
            e.beginID = ID;
            beginst.push(ID); // To store the beginID of the edge
            ID++;
            e.endID = ID;
            nfa.push_back(e);
            endst.push(ID); // To store the endID of the edge
        }
        else if (tmp == '|')
        {
            ID++;
            edge e1;
            edge e2;
            edge e3;
            edge e4;
            e1.trans = e2.trans = e3.trans = e4.trans = '='; // epsilon
            e1.beginID = e2.beginID = ID;
            e2.endID = beginst.top();
            beginst.pop();
            e1.endID = beginst.top();
            beginst.pop();
            e4.beginID = endst.top();
            endst.pop();
            e3.beginID = endst.top();
            endst.pop();
            beginst.push(ID);
            ID++;
            endst.push(ID);
            e3.endID = e4.endID = ID;
            nfa.push_back(e1);
            nfa.push_back(e2);
            nfa.push_back(e3);
            nfa.push_back(e4);
        }
        else if (tmp == '.')
        {
            int temp = endst.top();
            endst.pop();
            for (int i = 0; i < nfa.size(); ++i)
                if (nfa[i].beginID == beginst.top())
                    nfa[i].beginID = endst.top();
            beginst.pop();
            endst.pop();
            endst.push(temp);
        }
        // else if (tmp == '.')
        // {
        //     // ID++;
        //     edge e1;
        //     e1.trans = '='; // epsilon
        //     e1.endID = beginst.top();
        //     beginst.pop();
        //     int temp = endst.top();
        //     endst.pop();
        //     e1.beginID = endst.top();
        //     endst.pop();
        //     endst.push(temp);
        //     nfa.push_back(e1);
        // }
        else if (tmp == '*')
        {
            ID++;
            edge e1;
            edge e2;
            edge e3;
            edge e4;
            e1.trans = e2.trans = e3.trans = e4.trans = '='; // epsilon
            e1.beginID = ID;
            e3.beginID = ID;
            e1.endID = beginst.top();
            e4.endID = beginst.top();
            beginst.pop();
            e2.beginID = endst.top();
            e4.beginID = endst.top();
            endst.pop();
            beginst.push(ID); // To store the begin state of current nfa
            ID++;
            endst.push(ID); // To store the end state of current nfa
            e2.endID = ID;
            e3.endID = ID;
            nfa.push_back(e1);
            nfa.push_back(e2);
            nfa.push_back(e3);
            nfa.push_back(e4);
        }
        else
        {
            cout << "error" << endl;
        }
    }
    sort(nfa.begin(), nfa.end(), [](edge a, edge b)
         { return a.beginID < b.beginID; });
    cout << "beginID\ttrans\tendID" << endl;
    for (int i = 0; i < nfa.size(); i++)
    {
        cout << nfa[i].beginID << "\t" << nfa[i].trans << "\t" << nfa[i].endID << endl;
    }

    // for (int i = 0; i < nfa.size() - 1; i++)
    // {
    //     if (nfa[i + 1].beginID == nfa[i].endID && nfa[i + 1].trans == '=' && nfa[i+1].endID == nfa[i+1].beginID+1)
    //     {
    //         nfa.erase(nfa.begin() + i + 1);
    //         for (int j = i + 1; j < nfa.size(); j++)
    //         {
    //             nfa[j].beginID--;
    //             nfa[j].endID--;
    //         }
    //     }
    // }
}

int findbegin()
// find a edge whose beginID is not other edge's endID
{
    int beginID = -1;
    for (int i = 0; i < nfa.size(); i++)
    {
        int flag = 0;
        for (int j = 0; j < nfa.size(); j++)
        {
            if (nfa[i].beginID == nfa[j].endID)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            beginID = nfa[i].beginID;
            break;
        }
    }
    return beginID;
}

int findend()
// find a edge whose endID is not other edge's beginID
{
    int endID = -1;
    for (int i = 0; i < nfa.size(); i++)
    {
        int flag = 0;
        for (int j = 0; j < nfa.size(); j++)
        {
            if (nfa[i].endID == nfa[j].beginID)
            {
                flag = 1;
                break;
            }
        }
        if (flag == 0)
        {
            endID = nfa[i].endID;
            break;
        }
    }
    return endID;
}

int main()
{
    ifstream fin("input.txt");
    if (!fin)
    {
        cout << "Fail to open 'input.txt'" << endl;
        return 1;
    }
    int n = 0;
    ofstream fout("output.md", ios::out);
    while (!fin.eof())
    {
        n++;
        string expression;
        fin >> expression;
        if (fin.fail())
            break;
        if (isLegal(expression))
        {
            string infix = toinfix(expression);
            string suffix = tosuffix(infix);
            Tonfa(suffix);
            fout << "### RE" << n << endl;
            cout << "RE" << n << endl;
            fout << "~~~" << endl;
            fout << "Regular expression: " << expression << "" << endl;
            cout << "Regular expression: " << expression << "" << endl;
            // fout << "Infix expression: " << infix << "" << endl;
            // cout << "Infix expression: " << infix << "" << endl;
            fout << "Suffix expression: " << suffix << "" << endl;
            cout << "Suffix expression:" << suffix << "" << endl;
            fout << "Begin state: " << findbegin() << "" << endl;
            cout << "Begin state: " << findbegin() << "" << endl;
            fout << "End state: " << findend() << "" << endl;
            cout << "End state: " << findend() << "" << endl;
            fout << "~~~" << endl;
            fout << "NFA:" << endl;
            cout << "NFA:" << endl;
            fout << "~~~mermaid\nflowchart LR\n";
            for (int i = 0; i < nfa.size(); i++)
            {
                if (nfa[i].beginID == findbegin())
                {
                    fout << nfa[i].beginID << "((" << nfa[i].beginID << ":Start))--" << nfa[i].trans << "-->" << nfa[i].endID << "((" << nfa[i].endID << "))" << endl;
                    cout << nfa[i].beginID << "--" << nfa[i].trans << "-->" << nfa[i].endID << endl;
                }
                else if (nfa[i].endID == findend())
                {
                    fout << nfa[i].beginID << "((" << nfa[i].beginID << "))--" << nfa[i].trans << "-->" << nfa[i].endID << "(" << nfa[i].endID << ":End)" << endl;
                    cout << nfa[i].beginID << "--" << nfa[i].trans << "-->" << nfa[i].endID << endl;
                }
                else
                {
                    fout << nfa[i].beginID << "((" << nfa[i].beginID << "))--" << nfa[i].trans << "-->" << nfa[i].endID << "((" << nfa[i].endID << "))" << endl;
                    cout << nfa[i].beginID << "--" << nfa[i].trans << "-->" << nfa[i].endID << endl;
                }
            }
            fout << "~~~" << endl;
        }
        else
        {
            fout << "### RE" << n << endl;
            cout << "### RE" << n << endl;
            fout << "~~~" << endl;
            fout << "Regular expression: " << expression << " is illegal" << endl;
            fout << "~~~" << endl;
            cout << "Regular expression: " << expression << " is illegal" << endl;
        }
    }
    fout.close();
    fin.close();
    return 0;
}