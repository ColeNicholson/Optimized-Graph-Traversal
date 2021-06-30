Author: Cole Nicholson-Rubidoux
Description: This showcases a graph traversal algorithm which is meant to optimize the amount of 'value' one can achieve
	     by the traversal. In this case, the nodes of the graph contain a 'start date', 'end date', and a 'revenue'.
	     Our goal was to optimize revenue over a period of time. 
             The graph is represented in the form of an adjacency list where vertexList holds each 'header node' where each subsequent edge pointer
      	     represents a vertex adjacent to that header node. Observe the buildGraph() function to see the implementation.
             The graph is topologically sorted via a queue based source removal algorithm. See topologicalSort() function for the implementation.

Note: Runtime for the large input sets is longer than it needs to be because the program will print the graph representation. I thought this was necessary for showing the adjacency list.
      All of the expected output items will occur at the bottom of the outfile after the graph representation is complete.
