#ifndef WEBGRAPH_H_
#define WEBGRAPH_H_

#include <string>
#include <vector>
#include <utility>
#include <climits>

#include <boost/progress.hpp>
#include <boost/shared_ptr.hpp>
#include "types.hpp"
#include "offline_graph.hpp"
#include "compression_flags.hpp"
#include "utility_iterator_base.hpp"
#include "iterator_wrappers.hpp"
#include "node_iterator.hpp"
#include "input_bitstream.hpp"
#include "output_bitstream.hpp"
#include "logger.hpp"


namespace webgraph { namespace bv_graph {

   namespace utility_iterators {
      template<class val_type>
      class residual_iterator;
   }

 // le graphe à compresser 
class graph  
{
  
private:
   

   const static int STD_BUFFER_SIZE = 1024 * 1024;
        
   // les falgs utilisé pour la compression 
   int flags;
   
   //pointeur d'un itérateur interne de successeurs : les itérateur interne sont polymorphiques et peuvent être manipulé tous à l'aide d'un pointeur 
   typedef boost::shared_ptr<utility_iterators::utility_iterator_base<int> > internal_succ_itor_ptr;

public:
   friend class node_iterator;
   friend class utility_iterators::residual_iterator<int>;
   
   typedef webgraph::bv_graph::node_iterator node_iterator;
   typedef std::pair<node_iterator, node_iterator> node_itor_pair;

   typedef webgraph::bv_graph::iterator_wrappers::java_to_cpp<int> successor_iterator;
   typedef std::pair<successor_iterator, successor_iterator> succ_itor_pair;
   
   const static int BVGRAPH_VERSION = 0;

   //nombre par défaut des références 
   const static int DEFAULT_MAX_REF_COUNT; // initialized in .cpp
   
   // taille par défaut de la fenêtre "window"
   const static int DEFAULT_WINDOW_SIZE = 10; //7; TODO CHANGE THIS
   
   //taille par défaut d'un intervalle 
   const static int DEFAULT_MIN_INTERVAL_LENGTH = 3;
  
   //valeur par défaut de l'offset
   const static int DEFAULT_OFFSET_STEP = 1;
        
   //valeur par défaut du facteur de codage zéta 
   const static int DEFAULT_ZETA_K = 5; //3; TODO CHANGE THIS

   //écrire le outdegree en utilisant le codage gamma 
   const static int OUTDEGREES_GAMMA = compression_flags::GAMMA;

   //écrire le outdegree en utilisant delta
   const static int OUTDEGREES_DELTA = compression_flags::DELTA;

   //écrire les copy-block en utilisant gamma 
   const static int BLOCKS_GAMMA = compression_flags::GAMMA << 4;

   //écrire les copy-block en utilisant delta
   const static int BLOCKS_DELTA = compression_flags::DELTA << 4;

   //écrire les résidus en utilisant gamma
   const static int RESIDUALS_GAMMA = compression_flags::GAMMA << 8;

   //écrire les résidus en utilisant le codage zeta 
   const static int RESIDUALS_ZETA = compression_flags::ZETA << 8;

   //écrire les résidus en utilisant le codage delta 
   const static int RESIDUALS_DELTA = compression_flags::DELTA << 8;

   //écrire les résidus en utilisant le codage nibble
   const static int RESIDUALS_NIBBLE = compression_flags::NIBBLE << 8;

   //écrire les références en utilisant le codage gamma 
   const static int REFERENCES_GAMMA = compression_flags::GAMMA << 12;

   //écrire les références en utilisant le codage delta 
   const static int REFERENCES_DELTA = compression_flags::DELTA << 12;

   //écrire les références en utilisant le codage unaire 
   const static int REFERENCES_UNARY = compression_flags::UNARY << 12;

   // écrire le block count en utilisant le codage gamma 
   const static int BLOCK_COUNT_GAMMA = compression_flags::GAMMA << 16;

   //écrire le block  count en utilisant le codage delta 
   const static int BLOCK_COUNT_DELTA = compression_flags::DELTA << 16;

   //écrire le block count en utilsant le codage unaire
   const static int BLOCK_COUNT_UNARY = compression_flags::UNARY << 16;

   //écrire les offset en utilisant le codage gamma
   const static int OFFSETS_GAMMA = compression_flags::GAMMA << 20;

   //écrire les offsets en utilisant le codage delta 
   const static int OFFSETS_DELTA = compression_flags::DELTA << 20;


protected:
   
	// taille initial du tableau qui va contenir une liste de successeurs 
   const static int INITIAL_SUCCESSOR_LIST_LENGTH = 1024;
        
   // une valeur pour MIN_INTERVAL _LENGTH qui signifie que la taille est infinie 
   const static int NO_INTERVALS = 0;

   //nom du graphe : en chargeant le graphe avec un offset -1 => causera une exception 
   std::string basename;
        
   // le nombre de noeuds dans le graphe
   long n;

   //le nombre de liens dans le graphe
   long m;

   //si le offsestep n'est pas -1 : le graphe est chargé directement en mémoire
   bool in_memory;

   // le vecteur qui sauvegarde le graphe compressé si in_mémory= true et offsetStep != -1 
   // This variable is loaded with a copy of the graph file, or with a rearrangement of the latter, depending on whether {@link #offsetStep} 
   // is smaller than or equal to one. If {@link #offsetStep} is -1, this variable is <code>null</code>, and node iterators are generated 
   // by opening streams directly on the graph file.
   boost::shared_ptr< std::vector<byte> > graph_memory_ptr;
   std::vector<byte>& graph_memory;

   //cette valeur est null ssi offset_step <=0 ( i.e : pas de chargement d'offset), 
   // sinon : chaque entrée i représente l'offset (déplacement) qui représente le début du i'ème bloc de listes de successeurs 
   //la dernière entrée représente la taille en bits du fichier graphe 
   std::vector<long> offset;

   //nombre de reférence max 
   int max_ref_count;

   // taille de "window" : valeur == 0 veut dire pas de codage par références 
   int window_size;

   // taille minimale des intervalles 
   int min_interval_length;

   // le pas de l'offset, valeurs : 0 : pour pas d'offset et -1 : pour pas d'offset et pas de graph_memory
   int offset_step;

   // le facteur k pour le codage zeta 
   int zeta_k;

   //le codage des outdegree, par défaut : gamma
   int outdegree_coding;

   //le codage des copy-block, par défaut : gamma
   int block_coding;

   //le codage des résidus, par défaut : zeta
   int residual_coding;

   //le codage des références, par défaut : unary
   int reference_coding;

   //le codage des block_count, par défaut : gamma
   int block_count_coding;

   //le codage des offset , par défaut : gamma
   int offset_coding;

   //un flux qui encapsule graph_memory ou bien graph_stream. utilisé par la fct outdegree
   mutable ibitstream outdegree_ibs;

   //un cache qui contient les outdegrees [outdegree_cache_start .. outdegree_cache_end[
   mutable std::vector<int> outdegree_cache;
   mutable int outdegree_cache_start; // premier noeud dans le cache 
   mutable int outdegree_cache_end; // dernier noeud + 1 

   // un cache qui contient les offset : [outdegree_cache_start .. offset_cache_end[ /  l'accés se fait pas position 
   mutable std::vector<int> offset_cache;
   mutable int offset_cache_end; // dernier noeud dans le cache +1 < outdegree_cache_end 
   
   // vecteurs utilisé par la méthode difrentially_compress 
   std::vector<int> extras;
   std::vector<int> blocks;
   std::vector<int> len;
   std::vector<int> left;
   std::vector<int> residuals;

#ifndef CONFIG_FAST
   static logs::module_logger& lg() {
      return logs::logger( "webgraph" );
   }
#endif

protected:
   //constructeur : initialisation du graphe 
   graph() : graph_memory_ptr( new std::vector<byte> ), graph_memory( *graph_memory_ptr ), max_ref_count(DEFAULT_MAX_REF_COUNT),
             window_size(DEFAULT_WINDOW_SIZE), min_interval_length(DEFAULT_MIN_INTERVAL_LENGTH), offset_step(DEFAULT_OFFSET_STEP),
             zeta_k(DEFAULT_ZETA_K), outdegree_coding(webgraph::compression_flags::GAMMA), block_coding(webgraph::compression_flags::GAMMA),
             residual_coding(webgraph::compression_flags::ZETA), reference_coding(webgraph::compression_flags::UNARY), block_count_coding(webgraph::compression_flags::GAMMA),
             offset_coding(webgraph::compression_flags::GAMMA), outdegree_cache_start(INT_MAX), outdegree_cache_end(INT_MAX), offset_cache_end(INT_MAX) 
		   {
		#ifndef CONFIG_FAST
			  logs::register_logger( "webgraph", logs::LEVEL_MAX );
		#endif
		   }
        
public:
   //Destructeur 
   virtual ~graph() {
   }
   // getters 
   long get_num_nodes() const;
   long get_num_arcs() const;  
   std::string get_basename() const;
   int get_max_ref_count() const;
   int get_window_size() const;
   int get_offset_step() const;

protected:

	// lire un offset > ibs : l'offset file , retourne : le prochaine offset
   int read_offset( ibitstream& ibs ) const;

   // écrit un offset > obs : offset file , x : l'offset à écrire , retourne : le nombre de bits écrits 
   int write_offset( obitstream& obs, int x ) const;
   
   // lit un outdegree > ibs : graphe file , retourne : le prochain outdegree 
   int read_outdegree( ibitstream& ibs ) const;

   //écrit un outdegree > obs graphe file , d : l'outdegree à écrire, retourne : le nb de bits écrits 
   int write_outdegree( obitstream& obs, int d ) const;
   
   // lit une référence 
   int read_reference( ibitstream& ibs ) const;

   //écrit une référence 
   int write_reference( obitstream& obs, int ref ) const;

   //lit un block count 
   int read_block_count( ibitstream& ibs ) const;
   
   //écrit un block count 
   int write_block_count( obitstream& obs, int count ) const;
   
   //lit un block 
   int read_block( ibitstream& ibs ) const;

   //écrit un block
   int write_block( obitstream& obs, int block ) const;

   //lit un résidu 
   int read_residual( ibitstream& ibs ) const;

   //écrit un résidu 
   int write_residual( obitstream& obs, int residual ) const;

public:
	// retourner le outdegree d'un noeud aléatoire 
   int outdegree( int x ) const;

protected:

	//skip la partie de la liste des successeurs d'un noeud qui vient après le outdegree > ibs : le flux positionné au début de la liste des successeurs
	// x : le noeud dont la liste des sucesseurs va être sauté (skiped) , outd:  d(x). 
   void skip_node( ibitstream& ibs, int x, int outd ) const;

   // positionne le flux juste avant la liste des successeurs du noeud donnée , juste aprés son outdegree qui est retourné 
   int position( ibitstream& ibs, int x ) const;

public:

	//retourne un itérateur de successeurs d'un certain noeud x
   succ_itor_pair get_successors( int x ) const;
   
private:

	//wrapper pour la fct get_successors_internal 
   internal_succ_itor_ptr get_successors_internal( int x ) const;

   //donnant un flux qui représente un fichier graphe, la fct retourne un itérateur des sucesseurs d'un noeud x 
   internal_succ_itor_ptr get_successors_internal( int x, boost::shared_ptr<ibitstream> ibs,  std::vector< std::vector<int> >& window, std::vector<int>& outd,  std::vector<int>& blockOutdegrees ) const;

public:
	// retourne un itérateur de noeuds pour scanner le graphe séquentiellement , commençant du noeud donné 
   std::pair<node_iterator, node_iterator> get_node_iterator( int from ) const;
        
private:

	// mise à jour des flags et des attributs de compressio
   void set_flags( int flags );

   // produit une chaine qui représente les valeurs codé dans le flag mask donné 
   static std::string flags_to_string( int flags );

   // produit un flag mask qui correspond à la chaine donnée 
   static int string_to_flags( std::string flag_string );

public:

   typedef boost::shared_ptr<graph> graph_ptr;
   //crée un nouveau graphe en chargeant le graphe compressé à partir du disque > offset_step =0 : ne pas charger les offset , offset_step=-1 ne pas charger le fichier graphe
   // pm : le progress mètre utilisé lors du chargement ou bien null , retourne le graphe spécifié 
   static graph_ptr load( std::string basename, int offset_step, std::ostream* log = NULL );

   // crée un nouveau graphe en chargeant un graphe compressé du disque avec tous ses offsets
   static graph_ptr load( std::string basename, std::ostream* log = NULL );

   // crée un nouveau graphe en chargeant un graphe compressé du disque sans ses offsets
   static graph_ptr load_sequential( std::string basename, std::ostream* log = NULL );

   // crée un nouveau graphe en chargeant les métédata du fichier du graphe compressé
   static graph_ptr load_offline( std::string basename, std::ostream* log = NULL );

protected:
	//  charge le fichier du graphe compressé depuis le disque dans ce graphe. cette méthode est appelée seulement sur les graphe nouvellement créés 
   void load_internal( std::string basename, int offset_step, std::ostream* log = NULL );

   //methode de compression par intervalle ( extraire les listes d'intervales des lstes des noeuds x 
   static int intervalize( const std::vector<int>& x, int min_interval, std::vector<int>& left, std::vector<int>& len,  std::vector<int>& residuals );
                
private: 
	// compression differentielle de la liste des successeurs d'un noeud donnée 
   int differentially_compress( obitstream& obs, int current_node, int ref, std::vector<unsigned int>& ref_list, int ref_length,  std::vector<unsigned int>& current_list,  int current_len, bool for_real );
        
public:
	// écrire un graphe offline en tulisant son nom 'basename' 
   static void store_offline_graph( webgraph::ascii_graph::offline_graph graph, double fileSize, std::string prefix_compress, std::string prefix_order, std::string basename, int window_size, int max_ref_count, int min_interval_length, int zeta_k, int flags, std::ostream* log = NULL );

private:
	// écrire le graphe donné en utilisant son nom "basename", les paramètres de compression et les flags 
   void store_offline_graph_internal( webgraph::ascii_graph::offline_graph graph, double fileSize, std::string prefix_compress, std::string prefix_order ,std::string basename, std::ostream* log = NULL );
        
public:
	// Ecrire le fichier offset dans un flux obs, obs : le flux dans lequel on écrit les offset, pm : le progresse-metre 
   void write_offsets( obitstream& obs, std::ostream* log = NULL );


   
};

} }
#endif 
