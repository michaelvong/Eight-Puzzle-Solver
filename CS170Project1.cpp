#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>
#include <queue>
#include <chrono>

using namespace std::chrono;
using namespace std;

//node class to easily depict each state
//holds puzzle state at a point, the 4 states it may be expanded to (left,right,up,down) if possible
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

//https://stackoverflow.com/questions/19535644/how-to-use-the-priority-queue-stl-for-objects reference
//struct used to compare for priority queue
struct CompareCost{
    bool operator()(Node &N1, Node &N2) const {
        return (N1.cost+N1.depth) > (N2.cost+N2.depth);
    }
};

void generalSearch(vector<vector<int>>, int);
Node expandState(Node, map<vector<vector<int>>, int>);
bool isGoal(Node);
int misplaced(vector<vector<int>>);
int manhattan(vector<vector<int>>);

int main(){
    int gridSize = 3;
    cout << "Eight-Puzzle Solver" << endl;
    cout << "Type '1' to use a default puzzle, or '2' to create your own." << endl;
    int puzzleType = 0;
    cin >> puzzleType;
    cin.ignore(INT_MAX,'\n');
    vector<vector<int>> puzzle;
    if (puzzleType == 1){
        //puzzle = {{1,2,3}, {4,5,6}, {0,7,8}};
        //puzzle = {{1,2,3}, {4,5,6}, {7,8,0}};
        puzzle = {{1,3,6}, {5,0,2}, {4,7,8}};
    } 
    else if (puzzleType == 2){
        string row1S, row2S, row3S;
        vector<int> row1, row2, row3;

        //getting user input for rows
        //referenced geeksforgeeks.org for space removal
        cout << "Enter your puzzle, Please only enter valid 8-puzzles. Spaces are delimiters. ENTER to finish row" << endl;
        cout << "Row 1: ";
        getline(cin, row1S);
        row1S.erase(remove(row1S.begin(), row1S.end(), ' '), row1S.end());
        cout << "Row 2: ";
        getline(cin, row2S);
        row2S.erase(remove(row2S.begin(), row2S.end(), ' '), row2S.end());
        cout << "Row 3: ";
        getline(cin, row3S);
        row3S.erase(remove(row3S.begin(), row3S.end(), ' '), row3S.end());

        //subtracting 48 because these are ascii values
        //pushing all entered values into vectors
        for (int i = 0; i < gridSize; i++){
            row1.push_back(row1S[i]-48);
            row2.push_back(row2S[i]-48);
            row3.push_back(row3S[i]-48);
        }
        puzzle = {row1, row2, row3};
    }
       
    //getting search type
    int solveType = 0;
    cout << "Select a choice." << endl;
    cout << "1. Uniform Cost Searched " << endl;
    cout << "2. A* with the Misplaced Tile heuristic. " << endl;
    cout << "3. A* with the Manhattan distance heuristic. " << endl;
    cin >> solveType;
    cin.ignore(INT_MAX, '\n');
    generalSearch(puzzle, solveType);
    return 0;
}

//helper function to easily print puzzle
ostream & operator << (ostream &out, const Node &n){
    for (int i = 0; i < n.puzzleState.size(); i++){
        for (int j = 0; j < n.puzzleState[i].size(); j++){
            out << n.puzzleState[i][j] << ' ';
        }
        out << endl;
    }
    return out;
}

//function to expand a node, assigns the 4 expansions to the children of passed in node
Node expandState(Node n, map<vector<vector<int>>, int> seenMap){
    int row=0, col=0;
    //self-note: vectors will be size 0 if not able to move in that direction
    vector<vector<int>> leftPuzzle;
    vector<vector<int>> rightPuzzle;
    vector<vector<int>> upPuzzle;
    vector<vector<int>> downPuzzle;

    //going through 2d vector to locate 0
    //working
    for (int i = 0; i < n.puzzleState.size(); i++){
        for (int j = 0; j < n.puzzleState[i].size(); j++){
            if (n.puzzleState[i][j] == 0){
                row = i;
                col = j;
            }
        }
    }

    //check if able to move left
    if (col != 0){
        copy(n.puzzleState.begin(), n.puzzleState.end(), back_inserter(leftPuzzle)); //referenced from geeksforgeeks.org vector copy
        int leftVal = n.puzzleState[row][col-1]; //get val left of 0
        leftPuzzle[row][col-1] = 0;
        leftPuzzle[row][col] = leftVal;
    }

    //check if able to move right 
    if (col != n.puzzleState[0].size()-1){
        copy(n.puzzleState.begin(), n.puzzleState.end(), back_inserter(rightPuzzle));
        int rightVal = n.puzzleState[row][col+1]; //get val right of 0
        rightPuzzle[row][col+1] = 0;
        rightPuzzle[row][col] = rightVal;
    }

    //check if able to move up
    if (row != 0){
        copy(n.puzzleState.begin(), n.puzzleState.end(), back_inserter(upPuzzle));
        int upVal = n.puzzleState[row-1][col];
        upPuzzle[row-1][col] = 0;
        upPuzzle[row][col] = upVal;
    }

    //check if able to move down
    if (row != n.puzzleState.size()-1){
        copy(n.puzzleState.begin(), n.puzzleState.end(), back_inserter(downPuzzle));
        int downVal = n.puzzleState[row+1][col];
        downPuzzle[row+1][col] = 0;
        downPuzzle[row][col] = downVal;
    }

    //assign children to node if states have not been seen before
    //if direction puzzle was possible and state has not been seen before, then assign a child to new state
    if (leftPuzzle.size() != 0 && seenMap.find(leftPuzzle) == seenMap.end()){
        n.C1 = new Node(leftPuzzle);
    }
    if (rightPuzzle.size() != 0 && seenMap.find(rightPuzzle) == seenMap.end()){
        n.C2 = new Node(rightPuzzle);
    }
    if (upPuzzle.size() != 0 && seenMap.find(upPuzzle) == seenMap.end()){
        n.C3 = new Node(upPuzzle);
    }
    if (downPuzzle.size() != 0 && seenMap.find(downPuzzle) == seenMap.end()){
        n.C4 = new Node(downPuzzle);
    }


    return n;
}

void generalSearch(vector<vector<int>> problem, int queueFunction){
    int h = 0;
    int maxQ = 0,nodeCount = 0;

    if (queueFunction == 2){
        h = misplaced(problem);
    }
    else if (queueFunction == 3){
        h = manhattan(problem);
    }

    //this map will track all seen PUZZLES
    map<vector<vector<int>>, int> seenMap;

    //starting state, initializing values 
    Node n = Node(problem);
    n.depth = 0;

    //adding starting state to seenMap and queue
    seenMap[problem] = 1;

    //queue<Node> nodesToSearch; //used for regular queue
    priority_queue<Node, vector<Node>, CompareCost> nodesToSearch;
    nodesToSearch.push(n);

    bool flag = true;
    auto start = high_resolution_clock::now();
    while (flag){

        if (nodesToSearch.size() == 0){
            flag = false;
            cout << "No nodes left to search. Failed." << endl;
        }

        //set current node the first node in queue, then pop it off queue
        //Node currentNode = nodesToSearch.front(); used for regular queue
        Node currentNode = nodesToSearch.top();
        nodesToSearch.pop();

        //checks if current node is goal state
        if (isGoal(currentNode)){
            flag = false;
            cout << "Goal reached!" << endl;
            cout << "A total of " << nodeCount << " nodes were expanded. " << endl;
            cout << "The max number of nodes in a queue at one time was " << maxQ << endl;
            cout << "The depth of the goal node was " << currentNode.depth << endl;
            break;
        }

        //print state conditions
        if (nodeCount != 0){
            cout << "The best state to exapnd with a g(n) = " << currentNode.depth << " and h(n) = ";
            cout << currentNode.cost << " is " << endl;
            cout << currentNode << endl;
        } else {
            cout << "Expanding original state: " << endl << currentNode.puzzleState << endl;
        }

        if (!currentNode.expanded){
            currentNode.expanded = true;
            nodeCount += 1;
        }

        //expand the node and track seen states
        Node nExpanded = expandState(currentNode, seenMap);
        vector<Node*> childrenNodes = {nExpanded.C1, nExpanded.C2, nExpanded.C3, nExpanded.C4};

        //iterate through children nodes, increment depth, add to seenMap and queue accordingly
        for (int k = 0; k < childrenNodes.size(); k++){
            if (childrenNodes[k] != nullptr){
                if (queueFunction == 1){
                    childrenNodes[k]->cost = 0;
                }
                else if (queueFunction == 2){
                    childrenNodes[k]->cost = misplaced(childrenNodes[k]->puzzleState);
                }
                else if (queueFunction == 3){
                    childrenNodes[k]->cost = manhattan(childrenNodes[k]->puzzleState);
                }
                childrenNodes[k]->depth = nExpanded.depth + 1;

                nodesToSearch.push(*childrenNodes[k]);
                //add puzzle to map if not seen before
                if (seenMap.find(childrenNodes[k]->puzzleState) == seenMap.end()){
                    seenMap.insert({childrenNodes[k]->puzzleState, 1});
                }
            }
        }

        //tracks queue size
        if (nodesToSearch.size() > maxQ){
            maxQ = nodesToSearch.size();
        }
        
    }
    //cout << seenMap.find({{1,3,6}, {5,0,2}, {4,7,8}})->first << endl;
    //cout << seenMap.size() << endl;
    //https://www.geeksforgeeks.org/measure-execution-time-function-cpp/ used to implement time tracking
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<seconds>(stop - start);
    if (duration.count() == 0) {
        cout << "Total time taken to search: less than 1 second. " << endl;
    } else { cout << "Total time taken to search: " << duration.count() << " seconds." << endl; }
}

//checks if a given node's puzzle state matches goal puzzle
bool isGoal(Node n){
    vector<vector<int>> goal = {{1,2,3}, {4,5,6}, {7,8,0}};
    if (n.puzzleState == goal){
        return true;
    }
    return false;
}

//calculate h for misplaced not including 0
int misplaced(vector<vector<int>> puzzle){
    vector<vector<int>> goal = {{1,2,3}, {4,5,6}, {7,8,0}};
    int h = 0;
    for (int i = 0; i < goal.size(); i++){
        for (int j = 0; j < goal.size(); j++){
            if (puzzle[i][j] != 0 && puzzle[i][j] != goal[i][j]){
                h++;
            }
        }
    }
    return h;
}

//calculate h for manhattan distance 
int manhattan(vector<vector<int>> puzzle){
    vector<vector<int>> goal = {{1,2,3}, {4,5,6}, {7,8,0}};
    int h = 0;

    //calculate moves for each integer to get to goal state
    int row=0, col=0, goalRow=0, goalCol=0;
    for (int piece = 1; piece <= 9; piece++){
        for (int r=0; r < goal.size(); r++){
            for (int c=0; c < goal.size(); c++){
                if (puzzle[r][c] == piece){
                    row = r;
                    col = c;
                }
                if (goal[r][c] == piece){
                    goalRow = r;
                    goalCol = c;
                }
            }
        }
        h += abs(goalRow-row) + abs(goalCol-col);
    }
    return h;
}