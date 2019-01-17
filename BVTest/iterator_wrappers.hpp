#ifndef ITERATOR_WRAPPERS_HPP
#define ITERATOR_WRAPPERS_HPP

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
#include "utility_iterator_base.hpp"
#include <iostream>
#include <utility>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "logger.hpp"

namespace webgraph { namespace bv_graph { namespace iterator_wrappers {

//accéder à un CPP wrapper à travers un java idiom, et comme une sousclasse de utility_iterator_base
template<class itor_type, class val_type>
class cpp_to_java : public webgraph::bv_graph::utility_iterators::utility_iterator_base<val_type> {
   itor_type itor;
   itor_type itor_end;
//   int num_to_return;
//   int num_returned;

public:

   cpp_to_java( itor_type i_begin, itor_type i_end ) : itor( i_begin ), itor_end( i_end ) {
   }
   ~cpp_to_java() {}

   //récupérer le prochain élément 
   val_type next() {
      if( !has_next() ) 
         throw std::logic_error( "Attempt to dereference empty STL iterator." );
      
      val_type val = *itor;
      ++itor;
//      ++num_returned;
      return val;
   }
   
   //vrai si il y'a un prochain élément 
   bool has_next() const {
//      return (itor != itor_end) && ( num_to_return == -1 || (num_returned < num_to_return) );
      return itor != itor_end;
   }

   //passer 'n' élément 
   int skip( int n ); 
   
   //to_string
   std::string as_str() const {
      return "not implemented yet.";
   }

   //cloner l'itérateur
   cpp_to_java* clone() const {
      return new cpp_to_java( *this );
   }
};

//passer 'n' élément 
template<class itor_type, class val_type>
int cpp_to_java<itor_type, val_type>::skip( int n ) {
   int num_skipped = 0;
   while( itor != itor_end && num_skipped < n ) {
      ++itor;
      ++num_skipped;
   }
   
//   num_returned += num_skipped;
   
   return num_skipped;
}




//Acceder à un itérateur java à travers un idiom C++, 
template<class val_type>
class java_to_cpp :  public boost::iterator_facade<java_to_cpp<val_type>, val_type, boost::forward_traversal_tag, val_type> {

public:
   typedef utility_iterators::utility_iterator_base<val_type> underlying_t;
   typedef boost::shared_ptr< underlying_t > underlying_ptr;
   
private:
   underlying_ptr underlying;
   val_type curr_val;
   bool end_marker;
         
public:

   java_to_cpp() : curr_val(), end_marker(true) {}
   java_to_cpp( underlying_ptr u ) : underlying( u->clone() ), end_marker(false) {
      if( underlying->has_next() )
         increment();
      else
         end_marker = true;
   }
   java_to_cpp( underlying_t& u ) : underlying( u.clone() ), end_marker(false) {
      if( underlying->has_next() )
         increment();
      else
         end_marker = true;
   }
   java_to_cpp( const java_to_cpp<val_type>& other ) {
      if( other.underlying != NULL ) {
         underlying.reset(other.underlying->clone());
      }
      
      curr_val = other.curr_val;
      end_marker = other.end_marker;
   }
   ~java_to_cpp() {}
   java_to_cpp<val_type>& operator = ( const java_to_cpp<val_type>& other ) {
      if( other.underlying == NULL ) {
         underlying.reset();
      } else {
         underlying.reset(other.underlying->clone());
      }
      
      curr_val = other.curr_val;
      end_marker = other.end_marker;

      return *this;
   }
   std::string as_str() const { 
      using namespace std;
      ostringstream oss;

      oss << "successor_iterator_wrapper wrapping :\n";
      if( underlying == NULL ) {
         oss << "\tNULL";
      } else {
         oss << "\t" << underlying->as_str();
      }

      oss << ", curr_val = " << curr_val;

      return oss.str();
   }


////////////////////////////////////////////////////////////////////////////////
// iterator facade access

private:
  
   friend class boost::iterator_core_access;
   
   void increment() {
      if( !underlying->has_next() ) {
         end_marker = true;
      } else {
         curr_val = underlying->next();
      }
   }

   val_type dereference() const {
      return curr_val;
   }

   bool equal( const java_to_cpp& other ) const {
      // the only thing I'll worry about is comparisons with the end marker
      if( (underlying == NULL || end_marker) &&
          (other.underlying == NULL || other.end_marker ) ) {
         return true;
      } else {
         return false;
      }
   }
};


template<class itor_type, class val_type>
class itor_capture_wrapper :  public webgraph::bv_graph::utility_iterators::utility_iterator_base<val_type> {
private:
   std::vector<val_type> backing;
   unsigned cur_pos;

public:

   itor_capture_wrapper( itor_type i_begin, unsigned begin_index, unsigned length ) : backing(length), cur_pos(0) {
//      if( length != 0 ) {
         // we need a +1 because the last one is not copied.
         std::copy( i_begin, i_begin + length, backing.begin() );
//      }
   }

   val_type next() {
      if( !has_next() ) {
         throw std::logic_error( "Trying to dereference empty container_wrapper." );
      }

      return backing[cur_pos++];
   }

   bool has_next() const {
      return cur_pos < backing.size();
   }

   itor_capture_wrapper* clone() const {
      return new itor_capture_wrapper( *this );
   }

   std::string as_str() const {
      std::ostringstream oss;

      oss << "itor_capture_wrapper with size = " << backing.size() << " and cur_pos = " << cur_pos << "\n";

      return oss.str();
   }

   int skip( int how_many ) {
      if( cur_pos + how_many < backing.size() ) {
         cur_pos += how_many;
         return how_many;
      } else {
         // TODO might be off-by-one?
         int num_skipped = backing.size() - cur_pos;
         cur_pos = backing.size();
         return num_skipped;
      }
   }
};

}}}

#endif /*WEBGRAPH_INTERNAL_SUCCESSOR_ITERATORS_HPP_*/
