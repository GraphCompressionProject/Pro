#ifndef OFFLINEASCIIGRAPHEDGEITERATOR_HPP_
#define OFFLINEASCIIGRAPHEDGEITERATOR_HPP_

#include <utility>
#include <sstream>
#include <boost/iterator/iterator_facade.hpp>
#include "offline_vertex_iterator.hpp"
#include "vertex.hpp"
#include "edge.hpp"
//#define DEBUG 0
#include <iostream>

namespace webgraph { namespace ascii_graph {
   using namespace std;

   class offline_edge_iterator : 
      public boost::iterator_facade< offline_edge_iterator, edge const, boost::forward_traversal_tag>
   {
   private:
      edge current_edge; //paire de noeud courrante  
      int current_successor_index; //indice des noeuds successeurs du noeud courant  
      offline_vertex_iterator current_vertex; //itérateur à partir du noeud courant 
      offline_vertex_iterator last_vertex; // itérateur au dernier noeud 
      bool is_end_marker; 
      unsigned long num_vertices; // nombre de noeuds 

	  //avancer l'itérateur et récupérer la prochaine paire de noeuds 
      void fetch_next_edge(); 

   public:
	  //constructeur : itérateur vide aucun fichier 
      offline_edge_iterator();
      
	  //constructeur : itérateur sur le fichier filename 
	  explicit offline_edge_iterator(const char* filename);
      
	  //constructeur : copier l'iterateur courant dans l'iterateur rhs
	  offline_edge_iterator(const offline_edge_iterator& rhs);

	  //destructeur 
      virtual ~offline_edge_iterator();

	  //redifinition de l'opérateur = entre deux itérateur (copier l'un dans l'autre) 
      offline_edge_iterator& operator = ( const offline_edge_iterator& rhs );

   private:
      friend class boost::iterator_core_access;

	  // récupérer le edge suivant à partir du fichier et le mettre dans l'itérateur
      void increment() {
         fetch_next_edge();
      }

	  //equal entre deux itérateurs de edge 
      bool equal( const offline_edge_iterator& other ) const {
         // this assumes they are from the same graph.
#ifdef DEBUG
         cerr << "Comparing for equality between\n"
              << this->as_str() << endl
              << " and " << endl
              << other.as_str() << endl;
         bool val = (this->is_end_marker && other.is_end_marker) || 
                    (current_edge == other.current_edge && !this->is_end_marker && !other.is_end_marker);
                    
         cerr << "will return " << val << endl;
#endif         
         
         return (this->is_end_marker && other.is_end_marker) || 
                (current_edge == other.current_edge && 
                 !this->is_end_marker && !other.is_end_marker);
      }

	  //retourner le edge courant 
      const edge& dereference() const {
         return current_edge;
      }

   public:

     
   };

} }

#endif /*OFFLINEASCIIGRAPHEDGEITERATOR_HPP_*/
