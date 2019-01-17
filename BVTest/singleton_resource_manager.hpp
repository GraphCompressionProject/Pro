#ifndef FSTREAM_MANAGER_HPP
#define FSTREAM_MANAGER_HPP

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <fstream>
#include <boost/utility.hpp>


//Cette classe gère les ressources associées à des clés, elle assure l'existance d'une seule ressource par clé, 
//elle est appliquée aux fichiers et aux flux de données
namespace utils {
namespace resource_manager {


//Une fonction qui appelle un constructeur avec l'argument comme paramètre, elle retourne un pointeur du résultat (on the heap)
template<class arg_type, class class_type> struct constructor_caller {
   class_type* operator() ( const arg_type& a ) {
      return new class_type( a );
   }
};

// Specialize for ofstreams and strings, since (annoyingly) the ofstream ctor wants an old-style string.
template<> struct constructor_caller<std::string, std::ofstream> {
   std::ofstream* operator() ( const std::string& a ) {
      return new std::ofstream( a.c_str() );
   }
};


template< class key_type, class resource_type, class generator_type = constructor_caller<key_type, resource_type> >
class singleton_resource_manager : public boost::noncopyable {
   
	// la ressource (partagée)
	typedef boost::shared_ptr< resource_type > rp;
   //map entre la ressource et sa clé 
   std::map< key_type, rp > registry;
   
   //generateur de nouvelles ressources , il prend en entrée la clé et il renvoie la ressource 
   generator_type generator;

   singleton_resource_manager(generator_type gen) : generator(gen) {}
   singleton_resource_manager() {}

public:
   // retourner la ressource associée à la clé k dans la map registry
   resource_type& get( const key_type& k ) {
      // créer une nouvelle si elle n'existe pas.
      if( registry.find( k ) == registry.end() )
	 registry[k].reset( generator(k) );

      return *registry[k]; 
   }
    
 //instance du ressouce manager
    static singleton_resource_manager<key_type, resource_type, generator_type>& get_instance() {
    static singleton_resource_manager<key_type, resource_type, generator_type> me;
    return me;
   }
};

} }
#endif
