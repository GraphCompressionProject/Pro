Les commandes sont :
	- k2-trees :
		- Liste Adjacence : BVTest 3 graph K2_List k is_Directed
		- Matrice Adjacence : BVTest 3 graph K2_Matrix k is_Directed
		- From Graph Directly : BVTest 3 graph Graph k
		- With Reordering First : BVTest 5 graph k order_Type
			NB : Order_Type can take following values ( 1 : BFS ) , ( 2 : DFS ) , ( 3 : GRAY ) , ( 4 : LEXICAL ) , ( 5 : RANDOM ) 
	- Extraction De Motifs:
		- GCUPM : BVTest 4 graph GCUPM taille_du_pattern model_du_pattern
		- Subdue : BVTEST 4 graph Subdue -compress graph
		- DSM : BVTest 4 graph DSM number_Of_Hash_Function
		- CanDenSe :

Notre methode :
		- BVTest 7 graph DDSM number_Of_Hash_Function


Pour les testes le graph "web-polblogs" donne un bon resultat.
