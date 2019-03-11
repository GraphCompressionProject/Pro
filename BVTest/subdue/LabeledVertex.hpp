//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_LABELEDVERTEX_HPP
#define SUBDUE_LABELEDVERTEX_HPP

    class LabeledVertex {
    private:
        
        unsigned long label;    // index into label list of vertex's label
        unsigned long  numEdges; // number of edges defined using this vertex
        unsigned long *edges;   // indices into edge array of edges using this vertex
        unsigned long map;      // used to store mapping of this vertex to corresponding
        //   vertex in another graph
        bool used;// flag for marking vertex used at various times
        //   used flag assumed FALSE, so always reset when done

    public:
        unsigned long getLabel() const;

        void setLabel(unsigned long label);

        unsigned long getNumEdges() const;

        void setNumEdges(unsigned long numEdges);

        unsigned long *getEdges() const;

        void setEdges(unsigned long *edges);

        unsigned long getMap() const;

        void setMap(unsigned long map);

        bool isUsed() const;

        void setUsed(bool used);
        
        

    };





#endif //SUBDUE_LABELEDVERTEX_HPP
