#include <chrono> // for time
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <ctime>
#include <unordered_map>

using namespace std;

// node structure
struct node
{
    vector<int> boardState;
    int g;        // cost / depth
    int h;        // hueristic value (estimated)
    int f;        // g+h
    node *parent; // so we can trace path

    node()
    {
        g = 0;
        h = 0;
        f = 0;
        parent = NULL;
    }
};

// operator overloading for priority queue
struct comp
{
    bool operator()(const node *node1, const node *node2) const
    {
        return node1->f > node2->f;
    }
};

int numElem; // number of elements
int NG = 0;  // Nodes Generated
int NE = -1; // Nodes Expanded
int D = 0;   // Depth of tree

vector<int> goalBoard = {1, 2, 3, 8, 0, 4, 7, 6, 5}; // goal state
priority_queue<node *, vector<node *>, comp> OPEN;   // priority queue
vector<node *> CLOSED;                               // to keep track of closed nodes
vector<node *> totalPath;                            // all the nodes in the path from the root to the goal

// print board
void printBoard(node *n)
{
    int dim = sqrt(numElem);
    int k = 0;
    for (int i = 0; i < dim; ++i)
    {
        for (int j = 0; j < dim; ++j)
        {
            cout << n->boardState[k++] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// print path from root to goal node
void getFinalPath(node *n)
{
    node *temp = n;

    while (temp != NULL)
    {
        totalPath.push_back(temp);
        temp = temp->parent;
    }

    int totalSize = totalPath.size();

    // show the moves one by one
    cout << "Total Path" << endl;
    for (int i = totalSize - 1; i >= 0; --i)
    {
        printBoard(totalPath[i]);
    }
}

//  heuristic used by team
// int heuristic(node *n)
// {
//     int count = 0;
//     for (int i = 0; i < numElem; ++i)
//     {
//         if (goalBoard[i] != n->boardState[i])
//         {
//             ++count; // tiles out of place
//         }
//     }
//     return count;
// }



// custom individual heuristic
int heuristic(node *n)
{
    int count = 0;
    int dim = sqrt(numElem);
    for (int i = 0; i < numElem; ++i)
    {
        // check if current cell is not in its final position
        if (n->boardState[i] != goalBoard[i])
        {
            int row = i / dim;
            int col = i % dim;
            bool hasNeighbor = false;

            // check if there is a neighboring cell that is also not in its final position
            if (row > 0 && n->boardState[i - dim] != goalBoard[i - dim])
            {
                hasNeighbor = true;
            }
            else if (row < dim - 1 && n->boardState[i + dim] != goalBoard[i + dim])
            {
                hasNeighbor = true;
            }
            else if (col > 0 && n->boardState[i - 1] != goalBoard[i - 1])
            {
                hasNeighbor = true;
            }
            else if (col < dim - 1 && n->boardState[i + 1] != goalBoard[i + 1])
            {
                hasNeighbor = true;
            }

            if (hasNeighbor)
            {
                count++;
            }
        }
    }

    return count;
}

// build successor node
node *getSuccessor(node *state, int pos1, int pos2)
{
    NG++; // increment nodes generated
    node *newState = new node();
    newState->boardState = state->boardState;
    swap(newState->boardState[pos1], newState->boardState[pos2]);
    newState->h = heuristic(newState);
    newState->g = state->g + 1; // increment depth
    newState->f = newState->h + newState->g;
    newState->parent = state;

    return newState;
}

// generate successors
vector<node *> getSuccessors(node *n)
{
    NE++;
    int pos, row, col, dim;
    for (int i = 0; i < numElem; ++i)
    {
        if (n->boardState[i] == 0)
        {
            pos = i;
            break;
        }
    }
    dim = sqrt(numElem);
    row = pos / dim;
    col = pos % dim;

    vector<node *> successors;

    if (col != 0)
    {
        successors.push_back(getSuccessor(n, pos, pos - 1));
    }

    if (col != dim - 1)
    {
        successors.push_back(getSuccessor(n, pos, pos + 1));
    }
    if (row != 0)
    {
        successors.push_back(getSuccessor(n, pos, pos - dim));
    }
    if (row != dim - 1)
    {
        successors.push_back(getSuccessor(n, pos, pos + dim));
    }
    return successors;
}

// return true if goal nonde
bool goalReached(node *n)
{
    if (n->h == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// check if node has been previously generated
bool checkOLD(node *n)
{
    int totalSize = CLOSED.size(), j;
    for (int i = 0; i < totalSize; ++i)
    {
        for (j = 0; j < numElem; ++j)
        {
            if (n->boardState[j] != CLOSED[i]->boardState[j])
                break;
        }
        if (j == numElem)
            return 1;
    }
    return 0;
}

void A_star(node *n)
{

    n->h = heuristic(n);
    n->f = n->h;
    n->parent = NULL;
    OPEN.push(n);

    bool flag;
    int totalGCost, totalSize, k;
    node *current, *temp;
    vector<node *> current_successors;
    priority_queue<node *, vector<node *>, comp> pq;

    while (!OPEN.empty())
    {
        current = OPEN.top();
        OPEN.pop();
        CLOSED.push_back(current);

        // if (goalReached(current)) // for misplaced tiles

        // {
        //     getFinalPath(current);
        //     return;
        // }
        if (goalBoard == current->boardState)
        {
            getFinalPath(current);
            return;
        }
        current_successors.clear();
        current_successors = getSuccessors(current);

        totalSize = current_successors.size();
        for (int i = 0; i < totalSize; ++i)
        {

            if (checkOLD(current_successors[i]))
                continue;
            totalGCost = current->g + 1;

            while (!pq.empty())
            {
                pq.pop();
            }
            while (!OPEN.empty())
            {

                temp = OPEN.top();
                OPEN.pop();

                flag = 0;
                for (k = 0; k < numElem; ++k)
                {
                    if (current_successors[i]->boardState[k] != temp->boardState[k])
                    {
                        break;
                    }
                }
                if (k == numElem)
                {
                    flag = 1;
                }

                if (flag && totalGCost < temp->g)
                {
                    temp->parent = current;
                    temp->g = totalGCost;
                    temp->f = temp->g + temp->h;
                }
                pq.push(temp);
            }
            if (!flag)
            {
                pq.push(current_successors[i]);
            }
            OPEN = pq;
        }
    }

    return;
}

int main()
{
    
    auto start = chrono::high_resolution_clock::now();

    node *newNode = new node();
    numElem = 9;

    vector<int> puzzle1 = {2, 8, 3, 1, 6, 4, 0, 7, 5}; // initial state 1
    vector<int> puzzle2 = {2, 1, 6, 4, 0, 8, 7, 5, 3}; // initial state 2

    cout << "Which Initial State?" << endl;
    vector<int> puzzle = {0};
    int puzzleChoice = 0;
    cin >> puzzleChoice;

    if (puzzleChoice == 1)
    { // initial state 1 selected
        puzzle = puzzle1;
    }
    else if (puzzleChoice == 2)
    { // initial state 2 selected
        puzzle = puzzle2;
    }
    else
    { // wrong initial state
        cout << "invalid choice" << endl;
        return 0;
    }
    cout << endl;
    newNode->boardState = puzzle;
    cout << "Initial State" << endl;
    printBoard(newNode);

    A_star(newNode);
    D = totalPath.size() - 1;

    auto stop = chrono::high_resolution_clock::now();
    // print execution time
    auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    cout << "Execution time: " << duration.count() << " microseconds" << endl;

    cout << "Nodes Generated: " << NG << endl
         << "Nodes Expanded: " << NE << endl
         << "Depth: " << D << endl;
    cout << "b* Factor: " << ((float)NG / (float)D) << endl;
 
}