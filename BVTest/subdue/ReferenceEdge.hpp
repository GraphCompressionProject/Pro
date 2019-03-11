//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_REFERENCEEDGE_HPP
#define SUBDUE_REFERENCEEDGE_HPP



    
    class ReferenceEdge {
    private:
        unsigned long   vertex1;  // source vertex index into vertices array
        unsigned long   vertex2;  // target vertex index into vertices array
        bool spansIncrement; //true if edge spans to or from a previous increment
        unsigned long    label;    // index into label list of edge's label
        bool directed; // TRUE if edge is directed
        bool used;     // flag for marking edge used at various times
        //   used flag assumed FALSE, so always reset when done
        bool failed;
        unsigned long   map;

    public:
        unsigned long getVertex1() const;

        void setVertex1(unsigned long vertex1);

        unsigned long getVertex2() const;

        void setVertex2(unsigned long vertex2);

        bool isSpansIncrement() const;

        void setSpansIncrement(bool spansIncrement);

        unsigned long getLabel() const;

        void setLabel(unsigned long label);

        bool isDirected() const;

        void setDirected(bool directed);

        bool isUsed() const;

        void setUsed(bool used);

        bool isFailed() const;

        void setFailed(bool failed);

        unsigned long getMap() const;

        void setMap(unsigned long map);

    };
    




#endif //SUBDUE_REFERENCEEDGE_HPP
