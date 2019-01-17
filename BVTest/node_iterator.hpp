#ifndef NODE_ITERATOR_HPP
#define NODE_ITERATOR_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <sstream>
#include <string>
#include "iterator_wrappers.hpp"
#include "input_bitstream.hpp"
#include "logger.hpp"

namespace webgraph { namespace bv_graph {

class graph;

class node_iterator : public boost::iterator_facade< node_iterator, int, boost::forward_traversal_tag, int> {

private:
	// = numNodes();
   int n; 
   
   // le bit stream 
   boost::shared_ptr<ibitstream> ibs;

   // la taille du buffer cyclique ( taille de la fenetre "window" + 1) 
   int cyclic_buffer_size; // = windowSize + 1;
   
   // la fen�tre "window", le outd (outdegree) et le block_outdegrees peuvent �tre pass�s � la m�thode successors � tout moment 
   std::vector< std::vector<int> > window; // = new int[ cyclicBufferSize ][ INITIAL_SUCCESSOR_LIST_LENGTH ];
   std::vector<int> outd; // = new int[ cyclicBufferSize ];
   std::vector<int> block_outdegrees; // = offsetStep > 1 ? new int[ offsetStep ] : null;

   // l'indice du noeud depuis lequel on a commenc� � it�r� 
   int from;
   
   // l'indice du noeud courrant 
   int curr;
  
   // le graphe qu'on it�re actuellement 
   const graph* owner;
  
   //marqueur de fin 
   bool end_marker;


public:
	//Constructeurs 
   node_iterator() {
      this->from = 0;
      this->cyclic_buffer_size = 0;
      this->owner = NULL;
      this->n = 0;
      this->end_marker = true;

#ifndef CONFIG_FAST
      logs::register_logger( "iterators", logs::LEVEL_MAX );
#endif
   }
   node_iterator( const graph* owner, boost::shared_ptr<ibitstream> is, int from, int window_size );
   node_iterator( const node_iterator& rhs ) {
      copy( rhs );
   }
   
   //destructeur 
   ~node_iterator() {
      // nothing needed.
   }
   
   //red�finition de l'affectation entre it�rateurs 
   node_iterator& operator = ( const node_iterator& rhs ) {
      copy( rhs );
      return *this;
   }

private:
   friend class boost::iterator_core_access;

   // Copier other dans l'it�rateur courrant 
   void copy( const node_iterator& other );

   // � chaque appel, on construit l'it�rateur des successeurs ( la m�thode successors) , et on l'it�re compl�tement tout chargeant les entr�e appropri�e dans le vecteur "window"
   void increment();
   
   // retourner l'indice du noeud courrant 
   int dereference() const {
      return curr;
   }
   
   //m�thode equal entre deux it�rateur de noeuds
   bool equal( const node_iterator& rhs ) const;

public:
   typedef webgraph::bv_graph::iterator_wrappers::java_to_cpp<int> succ_itor_wrapper;
   
   //it�rateur des successeurs d'un noeud
   friend std::pair<succ_itor_wrapper, succ_itor_wrapper> successors( node_iterator& rhs );

   //retourner les successeurs d'un noeud
   friend std::vector<int> successor_vector( const node_iterator& rhs );

   
   friend int outdegree( const node_iterator& rhs );
   friend class graph;
};

//retourner le outdegree d'un noeud
int outdegree( const node_iterator& itor );

} }

   
#endif
