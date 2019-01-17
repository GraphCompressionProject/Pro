#include "stdafx.h"
#include "offline_edge_iterator.hpp"
#include <iostream>
#include <iterator>
#include <cassert>
#include <algorithm>
#include <boost/utility.hpp>
#include <boost/tuple/tuple.hpp>

using namespace boost;
using namespace boost::tuples;
using namespace std;

//#define DEBUG 1

namespace webgraph { namespace ascii_graph {

//constructeur : itérateur vide aucun fichier
offline_edge_iterator::offline_edge_iterator()
{
   is_end_marker = true;
}

//constructeur : itérateur sur le fichier filename.
offline_edge_iterator::offline_edge_iterator(const char* filename) : current_vertex(filename), last_vertex() {
   is_end_marker = false;
   
   
   ifstream getnumv( filename ); //ouvrir le fichier 
   unsigned long numv; 

   getnumv >> numv; 

   this->num_vertices = numv; //le nombre de noeud
	
   // this assumes there will be at least one vertex in the graph, so
   assert( num_vertices >= 1 );

   current_successor_index = -1;
  // cout << "here offline edge iterator" << endl;
   fetch_next_edge();

   
}

//copier l'iterateur rhs dans l'iterateur courant
offline_edge_iterator::offline_edge_iterator( const offline_edge_iterator& rhs ) {
   *this = rhs;
}

//redifinition de l'opérateur = entre deux itérateurs (copier l'un dans l'autre) 
offline_edge_iterator& offline_edge_iterator::operator = (const offline_edge_iterator& rhs ) {
   current_edge = rhs.current_edge;
   current_successor_index = rhs.current_successor_index;
   current_vertex = rhs.current_vertex;
   last_vertex = rhs.last_vertex;
   is_end_marker = rhs.is_end_marker;
   num_vertices = rhs.num_vertices;
   return *this;
}
                                                                   

//avancer l'itérateur et récupérer la prochaine paire de noeuds 
void offline_edge_iterator::fetch_next_edge() {
   assert( !is_end_marker );
  
  // cout << "fetch_next_edge" << endl;
   // Check to see if the current vertex has another successor.
   // current_successor++;
   if( current_successor_index < outdegree(current_vertex) ) {
      ++current_successor_index;
#ifdef DEBUG
      cerr << "fetch_next_edge\n"
           << "\tIncrementing current successor index. it's now " 
           << current_successor_index << endl
           << "\tThis is because vertex is : " << current_vertex.as_str()
           << "\toutdegree(vertex) : " << outdegree(current_vertex)
           << endl;
#endif
   }

   //----------- cout << "current vertex" << current_vertex.getCurrent_descriptor().get_label() << endl;

	//vérifier si le noeud courant a des successeurs. 
    //passer à traver les noeuds sans successeurs 
   while( current_successor_index == outdegree(current_vertex) ) {
      ++current_vertex;
		current_successor_index = 0;
      
		//si on est arrivé au dernier noeud du graphe alors on sort 
      if( current_vertex == last_vertex ) { 
         this->is_end_marker = true;
         return;	
      }
#ifdef DEBUG
      cerr << "fetch_next_edge:\n"
           << "Incrementing vertex. New vertex is " 
           << current_vertex.as_str()
           << endl;
#endif
   }
	
  // cout << "num vertices :" << num_vertices << endl;
   //cout << " successors(current_vertex)[0]  : " << successors(current_vertex)[0] << endl;
   
   
   // Sanity checks.
   assert( 0 <= successors(current_vertex)[0] );
   assert( successors(current_vertex)[0] < num_vertices );

   
   //mettre à jour la paire de noeud courante 
   current_edge = make_pair( *current_vertex, successors(current_vertex)[current_successor_index] );

#ifdef DEBUG
   cerr << "fetch_next_edge called.\n"
        << "\tcurrent_edge : <" << current_edge.first << ", " << current_edge.second << ">\n";
#endif
}


//destructeur
offline_edge_iterator::~offline_edge_iterator()
{
}

} } // end namespace
