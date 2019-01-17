#include "stdafx.h"
#include "offline_graph.hpp"
#include <sstream>
#include <iostream>
#include <boost/shared_ptr.hpp>


namespace webgraph { namespace ascii_graph {


offline_graph::offline_graph() : n(0), num_edges(0){}
offline_graph::~offline_graph(){}


// Charger le graphe en offline ( hors m�moire) en cr�ant un flux tokenizer qui lit le fichier en utilisant un it�rateur � l'aide des it�rateur offline
//mettre � jour le nombre de noeuds et de lien et le nom du fichier du graphe 
//retourner l'instance de graphe 
offline_graph offline_graph::load(const string& prefix, const string& basename, double& fileSize ) {

   offline_graph result;
  
  
   //result.filename = basename + ".graph-txt";
   result.filename = basename ;
   result.prefix = prefix; 
   
  
  string path = "..\\BVTest\\data\\" + basename + "\\" + prefix +basename + ".txt";

  // string path = "..\\BVTest\\data\\Test.txt";
   cout << path << endl;
   //ouvrir le fichier graphe 
   ifstream file( path.c_str() );

   file.seekg(0, file.end);
   fileSize = file.tellg();
   file.seekg(0, file.beg);
  
   //r�cup�rer le nombre de noeuds 
   string nextline;
   getline( file, nextline );
   istringstream nl( nextline );  
   nl >> result.n;
   
   //graphe non vide
   assert( result.n > 0 ); 
         
   //r�cup�rer le nombre de liens � l'aide de l'offline it�rateur 
   result.num_edges = 0;
   edge_iterator b, e;
  // cout << "here" << endl;
   tie( b, e ) = result.get_edge_iterator();
 //  cout << "here" << endl;
   for( ; b != e; ++b ) {
      result.num_edges++;
   }

   

   return result;
}


//retourne deux it�rateurs de noeud ( premier et dernier noeud du graphe)  
pair<offline_graph::vertex_iterator, offline_graph::vertex_iterator> offline_graph::get_vertex_iterator ( int from ) const  {
   if( from != 0 ) {
      cerr << "from is not implemented.\n";
   }
	return make_pair( offline_vertex_iterator( ("..\\BVTest\\data\\" + filename + "\\" + prefix + filename + ".txt").c_str() ), offline_vertex_iterator() );

//   return make_pair( offline_vertex_iterator( filename.c_str(), from ), offline_vertex_iterator(filename.c_str(), n));
}


//retourne deux it�rateurs de liens (edge) ( premier et dernier edge du graphe ) 
pair<offline_graph::edge_iterator, offline_graph::edge_iterator> offline_graph::get_edge_iterator() const {
   edge_iterator begin(("..\\BVTest\\data\\" + filename + "\\" + prefix + filename + ".txt").c_str() );
   edge_iterator end; 
   return make_pair( begin, end ); }

} }
