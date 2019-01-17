#ifndef OFFLINEVERTEXITERATOR_HPP_
#define OFFLINEVERTEXITERATOR_HPP_

#include "webgraph_vertex.hpp"
#include "vertex.hpp"
#include <string>
#include <sstream>
#include <boost/iterator/iterator_facade.hpp>
#include <fstream>
#include <cassert>
#include <iostream>

namespace webgraph { namespace ascii_graph {

   class offline_vertex_iterator : public boost::iterator_facade<offline_vertex_iterator,vertex_descriptor,boost::forward_traversal_tag, vertex_descriptor>
   {

   private:
     
      vertex_descriptor current_descriptor; //le noeud courant 
	  //std::vector<vertex> current_successors;
      std::ifstream back;	//le flux du fichier graphe  
      std::string filename; //le nom du fichier graphe 
      size_t get_pos;  // position du noeud courant dans le fichier 
      bool end_marker; //arrivé à la fin du fichier
      unsigned int num_vertices; //nombre de noeuds parcourus 
     
	  // mettre à zéro les attributs 
      void init() {
		 //current_descriptor = vertex_descriptor_t();
         get_pos = 0;
         end_marker = false;
         num_vertices = 0;   
      }
      
	  // copier l'itérateur other dans le courant
      void copy( const offline_vertex_iterator& other );	
   

   public:

	   vertex_descriptor getCurrent_descriptor() { return current_descriptor; }

	  // itérateur vide aucun fichier 
      offline_vertex_iterator();   

	  //copier l'iterateur courant dans l'iterateur that
      offline_vertex_iterator( const offline_vertex_iterator& that );
      
	  //itérateur sur le fichier filename 
	  explicit offline_vertex_iterator(const char* filename);
      
	  //destructeur 
	  virtual ~offline_vertex_iterator();

     //to string 
      string as_str() const;

      // redifinition operateur = entre l'iterateur rhs et le courant 
      offline_vertex_iterator& operator = ( const offline_vertex_iterator& rhs );
      
	  //retourner le outdegree
      friend int outdegree( const offline_vertex_iterator& me );

	  //retourner les successeurs 
      friend const std::vector<vertex_label_t>& successors( const offline_vertex_iterator& me );
	

   private:

      friend class boost::iterator_core_access;

	  // récupérer le noeud suivant à partir du fichier et le mettre dans l'itérateur 
      void increment();

	  //equal entre deux itérateurs de noeuds 
      bool equal( const offline_vertex_iterator& rhs ) const {
         // assumes they're both from the same graph.
         // TODO make this a bit more robust.
         
#if 0
         cerr << "Comparison between " << endl
              << this->as_str() << endl
              << " and " << endl
              << rhs.as_str() << endl;
              
         bool val = this->current_descriptor == rhs.current_descriptor || (this->end_marker && rhs.end_marker);
         
         cerr << "Will return : " << val << endl;
#endif
         
         return ((this->current_descriptor.get_label() == rhs.current_descriptor.get_label()) && 
                 !(this->end_marker || rhs.end_marker))
                || (this->end_marker && rhs.end_marker);
      }

	  //retourner le noeud courrant 
      vertex_descriptor dereference() const {
         return current_descriptor;
      }
   };

   int outdegree( const offline_vertex_iterator& me );
   const std::vector<vertex_label_t>& successors( const offline_vertex_iterator& me );

} }

#endif /*OFFLINEVERTEXITERATOR_HPP_*/
