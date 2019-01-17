#ifndef INTERVAL_SEQUENCE_ITERATOR_HPP_
#define INTERVAL_SEQUENCE_ITERATOR_HPP_

#include <cassert>
#include <boost/iterator/iterator_facade.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <exception>
#include <stdexcept>
#include "utility_iterator_base.hpp"
#include "logger.hpp"

//#define HARDCORE_DEBUG_INT_SEQ_ITOR

namespace webgraph { namespace bv_graph { namespace utility_iterators {


// itérateur des entiers contenus dans les séquences d'intervalles défini par : les extrèmes gauches et les tailles des intervalles  
template< class integral_type >
class interval_sequence_iterator : public utility_iterator_base<integral_type> {
private:
   
	// les bornes gauches des intervalles
   std::vector<integral_type> left;

   //les tailles des intervalles
   std::vector<integral_type> len;

   // l'indice de l'intervalle courrant
   unsigned int curr_interval;

 
   // la borne gauche de l'intervalle courrant
   unsigned int curr_left;

   // la position courrante dans l'intervalle courant 
   unsigned int curr_index;
   
   // le nombre d'intervalles restants
   unsigned n;
   
   //initialisation des paramètres 
   void init() {
      /** takes care of initializing the default stuff */
      curr_interval = 0;
      curr_left = 0;
      curr_index = 0;
   }

public:
   
	//Constructeurs 
   interval_sequence_iterator( const std::vector<integral_type>& lf,  const std::vector<integral_type>& ln ) : left( lf ), len(ln), n(lf.size()) {
      init();
      advance();
   } 
   interval_sequence_iterator( const std::vector<integral_type>& lf,  const std::vector<integral_type>& ln, const int sz ) : left(lf), len(ln), n(sz) {
      init();
      advance();
   }
	
public:


#ifdef HARDCORE_DEBUG_INT_SEQ_ITOR
   integral_type next() {
      integral_type result = next_internal();
      logs::logger( "iterators" ) << logs::LEVEL_MAX
                                  << "interval sequence iterator next about to return "
                                  << result << "\n";
      return result;
   }
   
   integral_type next_internal();
#else
   integral_type next();
#endif

   //cloner l'itérateur 
   interval_sequence_iterator<integral_type>* clone() const {
      return new interval_sequence_iterator<integral_type>( *this );
   }

   //vrai s'il y'a d'autre intervalles 
   bool has_next() const {
      return n != 0;
   }
   
   // passer un certains nombre d'éléments
   int skip( int how_many );

   //to_string
   std::string as_str() const {
      using namespace std;

      ostringstream oss;

      oss << "interval_sequence_iterator:\n"
          << "\tleft.size() = " << left.size() << "\n"
          << "\tlen.size() = " << len.size() << "\n"
          << "\tcur_interval = " << curr_interval << "\n"
          << "\tcur_index = " << curr_index << "\n"
          << "\tn = " << n << "\n";
      
      return oss.str();
   }

private:
	// avancer dans la liste jusqu'au prochain élément (intervalle) 
   void advance();
};

template<typename integral_type>
#ifdef HARDCORE_DEBUG_INT_SEQ_ITOR
integral_type interval_sequence_iterator<integral_type>::next_internal() {
#else
// retourner le prochain élément
integral_type interval_sequence_iterator<integral_type>::next() {
#endif
   if ( ! has_next() ) 
      throw std::logic_error("Attempt to fetch next element of empty integral_type iterator.");
   
   const int next = curr_left + curr_index++;
   advance();
   return next;
}

// passer un certain nombre d'éléments
template<typename integral_type> int interval_sequence_iterator<integral_type>::skip( int how_many ) {
   int skipped = 0;
   
   while( skipped < how_many && how_many != 0 ) {
      if ( how_many - skipped < len[ curr_interval ] - (int)curr_index ) {
         curr_index += ( how_many - skipped );
         return how_many;
      } else {
         skipped += len[ curr_interval ] - curr_index;
         curr_index = len[ curr_interval ];
         advance();
      }
   }
   
   return skipped;
}

// avancer dans la liste jusqu'au prochain élément (intervalle) 
template<typename integral_type> void interval_sequence_iterator<integral_type>::advance() {
   while( n != 0 ) {
      curr_left = left[ curr_interval ];
      if ( (int)curr_index < len[ curr_interval ] ) 
         break;
      n--;
      curr_interval++;
         curr_index = 0;
   }
}
   
} } }

#endif /*INTERVAL_SEQUENCE_ITERATOR_HPP_*/
