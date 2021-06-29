/* This code was written by Cole Nicholson-Rubidoux for CS415s20 Project 2
 * This is an adjacency list representation of a graph with the purpose of this being to calculate the
 * path through the graph which produces the optimal value when each node is given a weight
 */

#ifndef ADJLIST_HPP
#define ADJLIST_HPP
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <queue>
#include <string>

using namespace std;

class adjList;

class adjList
{
private:
  struct vertex
  { // Stores the start time, end time, revenue of each client, and maximum weight, as well as a pointer to the next compatible client
    vertex(int i, int j, int n); // Default constructor
    void setValues(int i, int j, int n); // Helper function for setting values
    int start; // Stores the client's starting date
    int end; // Stores the client's ending date
    int rev; // Stores the client's offered value
    int weight; // Stores self.rev + max(adj1.weight, adj2.weight... adjn.weight)
    int count; // Stores the number of incoming verticies
    vertex* original; // Stores the header node of which this vertex may be a copy of. Will be set to self if vertex is a header node
    vertex* opt; // Stores the adjacent vertex which leads to the optimal route to traverse the graph
    vertex* edge; // Stores the next adjacent vertex to this vertex's header node
  };
  vertex* begin = new vertex(0, 0, 0); // Artificial vertex which will serve as the starting point for graph traversal
  vertex* finish = new vertex(0, 0, 0); // Artificial vertex which will serve as the ending point for graph traversal
  vector<vertex*> vertexList; // Stores the vertex which shall become the header node for each adjacency list
  vector<vertex*> sortList; // Stores the header nodes in a topologically sorted fashion
  vector<vertex*> optList; // When parsed in ascending order, will produce the sequences of verticies which produce the largest revenue for the given clients.
  void buildGraph();
  void optPath(vector<vertex*>* list);
  void calcWeight();
  int findMax(vertex* node);
  void topologicalSort(vector<vertex*>* list);
  void deleteGraph();
  void deleteDependents(vertex* node);
public:
  adjList(); // Default Constructor
  ~adjList(); // Deconstructor
  friend ostream & operator << (ostream & output, const adjList & list); // Overloaded Output Operator
};

adjList::vertex::vertex(int i, int j, int n)
{ // Constructor for vertex. Initializes start, end, and rev to passed values. edge remains NULL until later
  start = i;
  end = j;
  rev = n;
  weight = 0;
  count = 0;
  original = this;
  opt = nullptr;
  edge = nullptr;
}

void adjList::vertex::setValues(int i, int j, int n)
{ // Helper function for assigning values to a vertex 
  start = i;
  end = j;
  rev = n;
}

adjList::adjList()
{ // Constructor function, will parse input into a set of vertices, then build adjacency based on vertex data, then calculate edge weight for each vertex
    vertexList.push_back(begin);
    fstream file;
    fstream fileout;
    string input;
    string subInput;
    int sdate, edate, rev;
    int count = 0;
    cout << "Enter the file name that contains the data: ";
    cin >> input;
    file.open(input);
    input = "out" + input;
    fileout.open(input, std::fstream::in | std::fstream::out | std::fstream::app);
    while(getline(file, input))
    {
        if(input == "done")
            break;
        int i = 0;
        while(!isdigit(input[i]))
            i++;
        while(isdigit(input[i]))
        {
            subInput += input[i];
            i++;
        }
        sdate = atoi(subInput.c_str());
        subInput.clear();
        while(!isdigit(input[i]))
            i++;
        while(isdigit(input[i]))
        {
            subInput += input[i];
            i++;
        }
        edate = atoi(subInput.c_str());
        subInput.clear();
        while(!isdigit(input[i]))
            i++;
        while(isdigit(input[i]))
        {
            subInput += input[i];
            i++;
        }
        rev = atoi(subInput.c_str());
        subInput.clear();
        vertex* temp = new vertex(sdate, edate, rev);
        vertexList.push_back(temp);
        count++;
    }
    file.close();
    fileout << "There are " << count << " clients in this file\n";
    finish->setValues(-1, -1, 0);
    finish->original = finish;
    vertexList.push_back(finish);
    buildGraph();
    topologicalSort(&sortList);
    optPath(&optList);
    fileout << *this;
    fileout.close();
}

adjList::~adjList()
{ // Deconstructor function
    delete begin;
    delete finish;
    deleteGraph();
}

ostream & operator << (ostream & output, const adjList & list)
{ // Prints every header vertex followed by each of its adjacent verticies

    output << "Header Nodes and Adjacent Verticies:\n";
    for(int i = 0; i < list.vertexList.size(); i++)
    {
      output << "(" << list.vertexList[i]->start << ", " << list.vertexList[i]->end << ", " << list.vertexList[i]->rev << ", " << list.vertexList[i]->count << ")";
      if(list.vertexList[i]->edge != nullptr)
          output << " -> ";
      else
          output << endl;
      adjList::vertex* itr;
      itr = list.vertexList[i]->edge;
      while(itr != nullptr)
      {
          output << "(" << itr->start << ", " << itr->end << ", " << itr->rev << ")";
          if(itr->edge != nullptr)
              output << " -> ";
          itr = itr->edge;
      }
    output << endl;
    }
    output << "Topological Sorting: ";

    for(int i = 0; i < list.sortList.size(); i++)
    {
        output << "(" << list.sortList[i]->start << ", " << list.sortList[i]->end << ", " << list.sortList[i]->rev << ")";
        if(i != list.sortList.size() - 1)
        output << ", ";
    }
    output << "\nOptimal Path: ";

    for(int i = 1; i < list.optList.size() - 1; i++)
    {
        output << "(" << list.optList[i]->start << ", " << list.optList[i]->end << ", " << list.optList[i]->rev << ")";
        if(i != list.optList.size() - 2)
        output << " -> ";
    }
    output << "\nOptimal Revenue: " << list.begin->weight << endl;
    return output;
}

void adjList::buildGraph()
{ // Function will build the adjacency list for each vertex in the vertexList and create a 'starting' and 'ending' vertex with no weight
    // Connect body verticies
    for(int i = 1; i < vertexList.size() - 1; i++) // Loop set up to skip the 'begin' and 'finish' nodes. Will be processed after.
    { // Builds the adjacency list based on the inputs given
        vertex* itr = vertexList[i];
        for(int j = 1; j < vertexList.size() - 1; j++)
        {
            if(i == j)
                continue;
            if(vertexList[i]->end <= vertexList[j]->start)
            { // Create a new node, set as adjacent to previous, move to newly created node
                vertex* adjacent = new vertex(vertexList[j]->start, vertexList[j]->end, vertexList[j]->rev);
                adjacent->original = vertexList[j];
                vertexList[j]->count++;
                itr->edge = adjacent;
                itr = adjacent;
            }
        }
    }
    // End connect body verticies
    // Connect 'finish' to graph
    vector<vertex*> tempList = vertexList;
    for(int i = 1; i < vertexList.size() - 1; i++)
    { // Connects nodes with no outgoing edges to the 'finish' node
        if(vertexList[i]->edge == nullptr)
        {
            vertexList[i]->edge = finish;
            finish->count++;
        }
    }
    // End connect 'finish'
    // Connect 'begin' to graph
    for(int i = 1; i < vertexList.size() - 1; i++) // Parses each header node's adjacent nodes and removes them from the tempList
    { // Remove all members from the temp list who have an incoming edge
        vertex* itr = vertexList[i]->edge;
        while(itr != nullptr)
        { // Parses all adjacent verticies
            for(int j = 1; j < tempList.size() - 1; j++)
                if(itr->original == tempList[j])
                    tempList.erase(tempList.begin() + j);
            itr = itr->edge;
        }
    }
    vertex* temp = begin;
    for(int i = 1; i < tempList.size() - 1; i++)
    { // Set start adjacent to all remaining members of the tempList, since these elems had no incoming verticies
        vertex* adjacent = new vertex(tempList[i]->start, tempList[i]->end, tempList[i]->rev);
        adjacent->original = tempList[i];
        tempList[i]->count++;
        temp->edge = adjacent;
        temp = adjacent;
    }
    // End connect 'begin'
}

void adjList::optPath(vector<vertex*>* list)
{ // creates a vector that stores the path which produces the largest revenue
    calcWeight();
    vertex* itr = begin;
    while(itr != nullptr)
    {
        list->push_back(itr);
        itr = itr->opt;
    }
}

void adjList::calcWeight()
{ // Sets weight of all header nodes based on the topological sorting computed previously
    for(int i = sortList.size() - 1; i >= 0; i--)
        sortList[i]->weight = findMax(sortList[i]) + sortList[i]->rev;
}

int adjList::findMax(vertex* node)
{ // Finds the header node of among all adjacent verticies with the maximum weight. Returns said weight 
    int val = 0;
    vertex* itr = node->edge;
    while(itr != nullptr)
    {
        if(itr->original->weight >= val)
        {
          val = itr->original->weight;
          node->original->opt = itr->original;
        }
        itr = itr->edge;
    }
    return val;
}

void adjList::topologicalSort(vector<vertex*>* list)
{ // Function creates a topological sorting of the graph and places it inside a vector
    vector<vertex*> tempList = vertexList;
    queue<vertex*> q;
    while(!tempList.empty())
    {
        for(int i = 0; i < tempList.size(); i++)
        { // Find each element in the list with no incoming edges.
            if(tempList[i]->count == 0)
            {
                q.push(tempList[i]);
                tempList.erase(tempList.begin() + i);
            }
        }
        while(!q.empty())
        { // While the queue has items in it, add those items to the list, decrement the count of adjacent verticies, then pop q
            list->push_back(q.front());
            vertex* itr = q.front()->edge;
            while(itr != nullptr)
            {
                itr->original->count--;
                itr = itr->edge;
            }
        q.pop();
        }
    }
}

void adjList::deleteGraph()
{ // Function remove all elements from the graph as well as all the verticies
    for(int i = 0; i < vertexList.size(); i++)
        deleteDependents(vertexList[i]);
    vertexList.clear();
    sortList.clear();
    optList.clear();
}

void adjList::deleteDependents(vertex* node)
{ // Function will remove all adjacent verticies from a given node
    if(node->edge != nullptr)
        deleteDependents(node->edge);
    delete node;
}
#endif
