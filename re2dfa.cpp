/**
* @file re2dfa.cpp
* @brief Convert regular expression to minimal DFA
* @param[in] expression Regular expression (txt file)
* @param[out] dfa minimal DFA
*/
#include <bits/stdc++.h>
using namespace std;

struct edge
{
    int beginID = -1;
    char trans;
    int endID = -1;
};

struct state
{
    int lastID = -1;
    char trans;
    int nowID = -1;
    vector<int> closure;
    bool terminal = false;
};

vector<edge> nfa;  // To store the NFA by edge
vector<state> dfa; // To store the DFA by state

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
    while (/* condition */ !op.empty())
    {
        suffix = suffix + op.top();
        op.pop();
    }
    return suffix;
}

vector<char> getalphabet(string a)
{
    vector<char> alphabet;
    for (int i = 0; i < a.length(); i++)
    {
        if (a[i] >= 'a' && a[i] <= 'z')
        {
            alphabet.push_back(a[i]);
        }
    }
    sort(alphabet.begin(), alphabet.end());
    alphabet.erase(unique(alphabet.begin(), alphabet.end()), alphabet.end());
    cout << "alphabet:";
    for (int i = 0; i < alphabet.size(); i++)
    {
        cout << alphabet[i] << " ";
    }
    cout << endl;
    return alphabet;
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
            // ID++;
            edge e1;
            e1.trans = '='; // epsilon
            e1.endID = beginst.top();
            beginst.pop();
            int temp = endst.top();
            endst.pop();
            e1.beginID = endst.top();
            endst.pop();
            endst.push(temp);
            nfa.push_back(e1);
        }
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
    // for (int i = 0; i < nfa.size() - 1; i++)
    // {
    //     if (nfa[i + 1].beginID == nfa[i].endID && nfa[i + 1].trans == '=' && nfa[i + 1].endID == nfa[i + 1].beginID + 1)
    //     {
    //         nfa.erase(nfa.begin() + i + 1);
    //         for (int j = i + 1; j < nfa.size(); j++)
    //         {
    //             nfa[j].beginID--;
    //             nfa[j].endID--;
    //         }
    //     }
    // }
    cout<<"---------DFA---------"<<endl;
    cout << "beginID\ttrans\tendID" << endl;
    for (int i = 0; i < nfa.size(); i++)
    {
        cout << nfa[i].beginID << "\t" << nfa[i].trans << "\t" << nfa[i].endID << endl;
    }
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

int DFadd(vector<int> &clos, char tran, int a1)
// to build the closure of the start state
{
    for (int i = 0; i < nfa.size(); i++)
    {
        if (nfa[i].beginID == a1 && (nfa[i].trans == tran || nfa[i].trans == '='))
        {
            clos.push_back(nfa[i].endID);
            DFadd(clos, tran, nfa[i].endID);
        }
    }
    return 0;
}

vector<int> clusure(int a)
// return a set of states that begin with a and trans == eposilon
{
    vector<int> v;
    v.push_back(a);
    for (int i = 0; i < nfa.size(); i++)
    {
        if (nfa[i].beginID == a && nfa[i].trans == '=')
        {
            v.push_back(nfa[i].endID);
        }
    }
    for (int i = 0; i < v.size(); i++)
    {
        for (int j = 0; j < nfa.size(); j++)
        {
            if (nfa[j].beginID == v[i] && nfa[j].trans == '=')
            {
                v.push_back(nfa[j].endID);
            }
        }
    }
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
    return v;
}

vector<int> move(vector<int> v, char a)
// return a set of states that begin with any of v and trans == a
{
    vector<int> closure;
    // closure.push_back(1000);
    for (int i = 0; i < v.size(); i++)
    {
        for (int j = 0; j < nfa.size(); j++)
        {
            if (v[i] == nfa[j].beginID && nfa[j].trans == a)
            {
                closure.push_back(nfa[j].endID);
            }
        }
    }
    if (!closure.empty())
    {
        cout << "move(";
        for (int i = 0; i < v.size(); i++)
        {
            cout << v[i] << ",";
        }
        cout << a << ")={";
        for (int i = 0; i < closure.size() - 1; i++)
        {
            cout << closure[i] << ",";
        }
        cout << closure[closure.size() - 1] << "}" << endl;
    }
    else
    {
        cout << "move(";
        for (int i = 0; i < v.size(); i++)
        {
            cout << v[i] << ",";
        }
        cout << a << ")={ }" << endl;
    }
    return closure;
}

vector<int> clusure_move(vector<int> v)
// return clusure{move(int a, char c)}
{
    vector<int> v1;
    vector<int> v2;
    for (int i = 0; i < v.size(); i++)
    {
        v1 = clusure(v[i]);
        v2.insert(v2.end(), v1.begin(), v1.end());
    }
    sort(v2.begin(), v2.end());
    v2.erase(unique(v2.begin(), v2.end()), v2.end());
    cout << "clusure_move(";
    for (int i = 0; i < v.size() - 1; i++)
    {
        cout << v[i] << ",";
    }
    cout << v[v.size() - 1];
    cout << ")={";
    for (int i = 0; i < v2.size() - 1; i++)
    {
        cout << v2[i] << ",";
    }
    cout << v2[v2.size() - 1] << "}" << endl;
    return v2;
}

int index(vector<int> v)
{
    for (int i = 0; i < dfa.size(); i++)
    {
        if (v == dfa[i].closure)
        {
            return i;
        }
    }
    return 0;
}

// int index(int a)
// {
//     for (int i = 0; i < dfa.size(); i++)
//     {
//         if (a == dfa[i].lastID)
//         {
//             return i;
//         }
//     }
//     return -1;
// }

void Todfa(vector<edge> nfa, string s1)
{
    for (int i = 0; i < nfa.size(); i++)
    {
        cout << "clusure(" << nfa[i].beginID << ")={";
        vector<int> v1;
        v1 = clusure(nfa[i].beginID);
        for (int j = 0; j < v1.size() - 1; j++)
        {
            cout << v1[j] << ",";
        }
        cout << v1[v1.size() - 1] << "}" << endl;
    }

    vector<char> alphabet = getalphabet(s1);
    state d1;
    int beginID = findbegin();
    d1.lastID = 0;
    d1.nowID = 1;
    d1.trans = 'N';
    d1.closure.push_back(beginID);
    DFadd(d1.closure, '=', beginID);
    std::sort(d1.closure.begin(), d1.closure.end());
    dfa.push_back(d1);
    queue<state> q;
    q.push(d1);
    vector<state> dumplicate;

    while (!q.empty())
    {
        state s = q.front();
        state temp;
        for (int i = 0; i < alphabet.size(); i++)
        {
            vector<int> v1 = move(s.closure, alphabet[i]);
            vector<int> v2 = clusure_move(v1);
            if (!index(v2))
            {
                temp.closure = v2;
                temp.lastID = s.nowID;
                temp.nowID = dfa.size() + 1;
                temp.trans = alphabet[i];
                dfa.push_back(temp);
                q.push(temp);
            }
            else
            {
                temp.closure = v2;
                temp.lastID = s.nowID;
                temp.nowID = index(v2) + 1;
                temp.trans = alphabet[i];
                dumplicate.push_back(temp);
            }
        }
        q.pop();
    }
    int endID = findend();
    for (int i = 0; i < dfa.size(); i++)
    {
        if (count(dfa[i].closure.begin(), dfa[i].closure.end(), endID))
        {
            dfa[i].terminal = true;
        }
    }
    // cout << "============Dfa1============" << endl;
    // cout << "nowID\tisterminal\tclosure" << endl;
    // for (int i = 0; i < dfa.size(); i++)
    // {
    //     cout << dfa[i].nowID << "\t" << dfa[i].terminal << "\t\t";
    //     for (int j = 0; j < dfa[i].closure.size(); j++)
    //     {
    //         cout << dfa[i].closure[j] << " ";
    //     }
    //     cout << endl;
    // }
    dfa.insert(dfa.end(), dumplicate.begin(), dumplicate.end());
    sort(dfa.begin(), dfa.end(), [](state a, state b)
         { return a.nowID < b.nowID; });
    for (int i = 0; i < dfa.size(); i++)
    {
        if (count(dfa[i].closure.begin(), dfa[i].closure.end(), endID))
        {
            dfa[i].terminal = true;
        }
    }
    // cout << "============Dfa2============" << endl;
    // cout << "lastID\ttrans\tnowID\tisterminal\tclosure" << endl;
    // for (int i = 0; i < dfa.size(); i++)
    // {
    //     cout << dfa[i].lastID << "\t" << dfa[i].trans << "\t" << dfa[i].nowID << "\t" << dfa[i].terminal << "\t\t";
    //     for (int j = 0; j < dfa[i].closure.size(); j++)
    //     {
    //         cout << dfa[i].closure[j] << " ";
    //     }
    //     cout << endl;
    // }
}

void deletedumplicate(int a, int b)
{
    for (int i = 0; i < dfa.size(); i++)
    {
        if (dfa[i].lastID == b)
        {
            dfa[i].lastID = a;
        }
        if (dfa[i].nowID == b)
        {
            dfa[i].nowID = a;
        }
    }
}

bool unique(vector<state> v,state s)
{
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].closure == s.closure && v[i].terminal == s.terminal && v[i].lastID == s.lastID && v[i].nowID == s.nowID && v[i].trans == s.trans)
        {
            return false;
        }
    }
    return true;
}

void minimize()
{
    vector<state> minidfa;
    for (int i = 0; i < dfa.size(); i++)
    {
        vector<int>().swap(dfa[i].closure);
    }
    bool endend = true;
    while (endend)
    {
        vector<vector<int>> V;
        vector<int> v;
        vector<int> tomove;
        endend = false;
        for (int j = 0; j < dfa.size(); j++)
        {
            vector<int>().swap(v);
            for (int i = 0; i < dfa.size(); i++)
            {
                if (dfa[i].lastID == j)
                {
                    int a = int(dfa[i].trans) + dfa[i].nowID * 100;
                    if (count(v.begin(), v.end(), a) == 0)
                    {
                        v.push_back(a);
                    }
                }
            }
            sort(v.begin(), v.end());
            V.push_back(v);
        }
        for (int i = 0; i < V.size(); i++)
        {
            if (V.back().empty())
            {
                V.pop_back();
            }
        }
        // for (int i = 0; i < V.size(); i++)
        // {
        //     for (int j = 0; j < V[i].size(); j++)
        //     {
        //         cout << V[i][j] << " ";
        //     }
        //     cout << endl;
        // }
        for (int i = 0; i < V.size(); i++)
        {
            for (int j = i + 1; j < V.size(); j++)
            {
                if (V[i] == V[j] && !V[j].empty())
                {
                    // cout << "state" << i << "," << j << "can be one state" << endl;
                    endend = true;
                    tomove.push_back(j);
                    deletedumplicate(i, j);
                }
            }
        }
    }
    for(int i =0;i<dfa.size();i++)
    {
        if(i == 0)
        {
            minidfa.push_back(dfa[i]);
        }
        else
        {
            if(unique(minidfa,dfa[i]))
            {
                minidfa.push_back(dfa[i]);
            }
        }
    }
    cout<<"============MinimizedDfa============"<<endl;
    cout<<"lastID\ttrans\tnowID\tisterminal"<<endl;
    for(int i =0;i<minidfa.size();i++)
    {
        cout<<minidfa[i].lastID<<"\t"<<minidfa[i].trans<<"\t"<<minidfa[i].nowID<<"\t"<<minidfa[i].terminal<<endl;
    }
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
            Todfa(nfa, expression);
            minimize();
        }
    }
}