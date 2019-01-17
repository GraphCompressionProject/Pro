#ifndef ASCIIGRAPH_HPP_
#define ASCIIGRAPH_HPP_

#include <string>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/graph/adjacency_iterator.hpp>
#include "webgraph_vertex.hpp"
#include "offline_edge_iterator.hpp"
#include "offline_vertex_iterator.hpp"
#include "vertex.hpp"
#include "edge.hpp"

using namespace std;

namespace webgraph { namespace ascii_graph {

//le graphe n'est pas chargé en mémoire 
//pour accéder à ses éléments, on utilise des itérateurs. 
class offline_graph {
   
private:
   
   // constructeur : mettre à zéro les liens et les noeuds du graphe 
   offline_graph();
   
protected:
   unsigned int n; // nombre de noeuds 
   string filename; // fichier contenant le graphe 
   unsigned int num_edges; //nombre de liens 
   string prefix;
  
   
public:

   //destructeur 
   virtual ~offline_graph();
          
   typedef offline_edge_iterator edge_iterator;   
   typedef offline_vertex_iterator vertex_iterator;
   typedef offline_vertex_iterator node_iterator;
   typedef vertex_descriptor vertex_type;
   typedef edge edge_type; 
   
   //
   static offline_graph load(const string& prefix, const string& basename, double& fileSize);

   //retourne deux itérateurs de noeud ( premier et dernier noeud du graphe) 
   pair<vertex_iterator, vertex_iterator> get_vertex_iterator(int from = 0) const;

   //retourne deux itérateurs de liens (edge) ( premier et dernier edge du graphe ) 
   pair<edge_iterator, edge_iterator> get_edge_iterator() const;
   
   //retourner le nombre de noeuds 
   unsigned int get_num_nodes() const {
      return n;
   }

   //retourner le nombre de liens 
   unsigned int get_num_edges() const {
      return num_edges;
   }

};
} } 

#endif /*ASCIIGRAPH_HPP_*/
