// Required directives are to be added from ESSENS to use the functionality 

#include "structure_defs.hpp"
#include "input_to_network.hpp"
#include "printout_others.hpp"
#include "printout_network.hpp"
#include "ADJ/find_Xneighbors.hpp"
#include "GDV_functions.hpp"
#include "class_definitions.hpp"
#include "print_disconnected_graph.hpp"
#include <time.h>

void Calculate_GDV(int ,A_Network ,vector<OrbitMetric>&, GDVMetric&);
void readin_orbits(  ifstream* ,vector<OrbitMetric>* );
void convert_string_vector_int(string* , vector<int>* ,string );
using namespace std;

int main(int argc, char *argv[]) {
  clock_t tStart = clock();
  clock_t q, q1, q2,t;
  GDV_functions gdvf;
  /* Accepts file as input. 
     Input should be in the format of ( node1 node2 weight ). 
     By default, weight should be 1
     Should be sorted. */ 
  ifstream the_file1 ( argv[1] ); 
  if (!the_file1.is_open() ) { 
    cout<<"INPUT ERROR:: Could not open the graph input file\n";
  }

   ifstream the_file2 ( argv[2] ); 
  if (!the_file2.is_open() ) { 
    cout<<"INPUT ERROR:: Could not open the orbit file\n";
  }
  
  vector<OrbitMetric> orbits;
  readin_orbits(&the_file2,&orbits);
  // Objects for testing orbit creation 
  // print_vector(orbits[1].orbitDegree);
  // vector<OrbitMetric> filter_o = gdvf.orbit_filter(&orbits,3);
  /* Read in Network: Reads the file and converts it into a network of type A_Network*/
  A_Network X;
  readin_network(&X,argv[1],0,-1);  
  GDV_functions test_gdvf;

  // objects for testing orbit list
  // vector<OrbitMetric> filtered_orbits ;
  // gdvf.orbit_filter(orbits,3,filtered_orbits);
  // print_vector(filtered_orbits[1].orbitDistance);

  // Objects for testing GDV induced subgraph function
  //A_Network subgraph;
  //vector<int> subgraph_nodes;
  //subgraph_nodes.push_back(0);
  //subgraph_nodes.push_back(1);
  //subgraph_nodes.push_back(2);
  //subgraph_nodes.push_back(7);
  //gdvf.inducedSubgraph(X, subgraph_nodes, subgraph);
  //print_disconnected_network(subgraph);
  // cout<<"subgraph for 0,1,2,6"<<endl;
  // print_network(subgraph);

  // Objects for testing connectedness function
  //bool is_connected = false;
  //gdvf.isConnected(subgraph, is_connected);
  //cout << is_connected << endl;

  // Objects for testing degree signature
  //vector<int> degree_sig;
  //test_gdvf.degree_signature(X, degree_sig);
  //print_vector(degree_sig);

  // Objects for testing distance signature
  //vector<int> distance_sig;
  //test_gdvf.distance_signature(2, X, distance_sig);
  //print_vector(distance_sig);
  // for (int i:X)
  // {
  //   // Calculate_GDV(i,X);
  // }


    for (ADJ_Bundle node:X)
  {
    vector<int> GDV_1;
    GDVMetric gdvMetric(node.Row,GDV_1);
    Calculate_GDV(node.Row,X,orbits,gdvMetric);
    cout<<"gdv for node "<<node.Row<<endl;
    print_vector(gdvMetric.GDV);
  }
  
 printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
  return 0;
}

void Calculate_GDV(int node,A_Network Graph,vector<OrbitMetric> &orbits, GDVMetric &gdvMetric)
{
    GDV_functions gdvf;
    vector<int> gdv(orbits.size(),0);
    // printf("calculating GDV for node %d\n",node);
    vector<int> neighbours;
    gdvf.find_neighbours(node,Graph,4,&neighbours);
    print_vector(neighbours);
    int set[neighbours.size()]; 
    std::copy( neighbours.begin(), neighbours.end(), set );
    int numElements = *(&set + 1) - set;
    for (int node_count = 1; node_count < 5; node_count++)
    {
      vector<vector<int>> combinationsList;
      gdvf.find_combinations(set, numElements,node_count,&combinationsList);
      // cout<<"Node count is "<<node_count<<endl;
      // cout<<"total combinations are : "<<combinationsList.size()<<endl;
      for (vector<int> combination : combinationsList)
      {
        A_Network induced_sgraph;
        vector<int> subgraph_degree_signature;
        vector<int> subgraph_distance_signature;
        bool is_connected = false;
        combination.push_back(node);
        gdvf.inducedSubgraph(Graph, combination, induced_sgraph);
        gdvf.isConnected(induced_sgraph, is_connected);
        if(is_connected)
        {
            gdvf.degree_signature(induced_sgraph,subgraph_degree_signature);
            gdvf.distance_signature(node,induced_sgraph,subgraph_distance_signature);
            vector<OrbitMetric> filter_orbits;
            gdvf.orbit_filter(orbits,node_count+1,filter_orbits);
            for(OrbitMetric orbit: filter_orbits)
            {
              sort(orbit.orbitDegree.begin(),orbit.orbitDegree.end());
              sort(subgraph_degree_signature.begin(),subgraph_degree_signature.end());
              if( orbit.orbitDistance == subgraph_distance_signature && 
                  orbit.orbitDegree == subgraph_degree_signature)
              {
                gdv[orbit.orbitNumber] +=1;
                break;
              }
            }
        }
      }
    }
    gdvMetric.GDV = gdv;
    gdvMetric.node = node;
}

//This method takes the file and converts it into orbits and saves in output
void readin_orbits(ifstream *file,vector<OrbitMetric>* output )
{
  string line;
  string signature_delimiter;
  string internal_delimiter;
  signature_delimiter = "/";
  internal_delimiter= ",";
  while(std::getline(*file,line))
  {
    string s= line;
    size_t pos = 0;
    vector<vector<int>> vector_line;
    do
    {
      vector<int> segment; 
      string token;
      pos = s.find(signature_delimiter);
      token = s.substr(0, pos);
      token.erase(remove(token.begin(), token.end(), '['), token.end());
      token.erase(remove(token.begin(), token.end(), ']'), token.end());
      convert_string_vector_int(&token,&segment,internal_delimiter);
      s.erase(0, pos + signature_delimiter.length());
      vector_line.push_back(segment);
    }
    while (pos!= std::string::npos);

    OrbitMetric orbMetric(vector_line[0][0],vector_line[1],vector_line[2]);
    output->push_back(orbMetric);
  }

}

//This method converts a string containing integers to a vector of integers
void convert_string_vector_int(string* str, vector<int>* output,string delimiter)
{
  size_t pos = 0;
  int token;
  string s;
  s = *str;
  do
  {
    pos = s.find(delimiter);
    token = stoi(s.substr(0, pos));
    output->push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  while (pos != std::string::npos);

}



