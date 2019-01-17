#ifndef WEBGRAPHVERTEX_HPP_
#define WEBGRAPHVERTEX_HPP_

#include <utility>
#include <vector>
#include <algorithm>
#include <sstream>
#include <iterator>

namespace webgraph
{

   using namespace std;

   class webgraph_vertex
   {
   public:
      typedef unsigned int node_type; // type d'un noeud : un entier 

   private:
      vector<node_type> successors; // liste des successeurs 
      int label;					// le label d'un noeud 
	
   public:
	  
	   //Constructeurs et successeurs 
      webgraph_vertex() : successors(0), label(-1) {	
      }
      webgraph_vertex( node_type l, const vector<node_type>& s ) : successors(s), label(l) {
      }
      virtual ~webgraph_vertex() {}
	
	  // type de l'itérateur des successeurs 
      typedef vector<node_type>::const_iterator successor_iterator; 
	
	  //redéfinition de l'affectation entre deux vertex 
      webgraph_vertex& operator = ( const webgraph_vertex& that ) {
         this->successors = that.successors;
         this->label = that.label;
         return *this;
      }
      
	  // getter de l'itérateur des successeurs 
      pair<successor_iterator, successor_iterator> get_successor_iterator() {
         return make_pair(successors.begin(), successors.end() );	
      }

 
	  // getter de l'itérateur des successeurs (commençant par un certain noeud) 
      pair<successor_iterator, successor_iterator> get_successor_iterator( node_type successor_label ) { 
         successor_iterator start, end;
         start = find( successors.begin(), successors.end(), successor_label );
         end = successors.end();
         return make_pair( start, end );
      }

	  //redéfinition de l'égalité entre deux vertex
      bool operator == ( const webgraph_vertex & other ) {
         return this->label == other.label;
      }
	 
	  //getter de label
      node_type get_label() {
         return label;
      }

      // to_string
      string as_str() const {
         ostringstream o;

         o << "<" << label << "> : ";

         copy( successors.begin(), successors.end(), ostream_iterator<int>( o, " " ) );

         return o.str();
      }
   };
}

#endif 
