#include <queue>
#include <vector>
#include <iostream>
#include <functional>

using namespace std;

class Node{

public:
    Node(vector<vector<int>> puzzle){this->puzzleState=puzzle;}
    vector<vector<int>> puzzleState;
    Node* C1 = nullptr;
    Node* C2 = nullptr;
    Node* C3 = nullptr;
    Node* C4 = nullptr;
    int cost = 0;
    int depth = 0;
    bool expanded = false;
    friend ostream & operator << (ostream &, const Node&);
};

struct CompareCost{
    bool operator()(Node &N1, Node &N2) const {
        return (N1.cost+N1.depth) > (N2.cost+N2.depth);
    }
};

int main(){
    priority_queue<Node, vector<Node>, CompareCost> q;
    Node N1 = Node({{1,3,6}, {5,0,2}, {4,7,8}});
    N1.cost = 5;
    Node N2 = Node({{1,3,6}, {5,0,2}, {4,7,8}});
    N2.cost = 7;
    Node N3 = Node({{1,3,6}, {5,0,2}, {4,7,8}});
    N3.cost = 6;
    q.push(N1);
    q.push(N2);
    q.push(N3);
    cout << q.top().cost << endl;
    q.pop();
    cout << q.top().cost << endl;
    return 0;
}