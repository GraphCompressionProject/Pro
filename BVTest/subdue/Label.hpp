//
// Created by Macbook on 10/02/2019.
//

#ifndef SUBDUE_LABEL_HPP
#define SUBDUE_LABEL_HPP
#include <iostream>
#include <string>

#include "Labval.hpp"

class Label {
    private:
        unsigned char labelType;       // one of STRING_LABEL or NUMERIC_LABEL
        Labval labelValue;
        bool used;
        
        public :
        /*
         * Getters of the provate Attributes
         */
        Label(){
            labelValue = * new Labval();
        }

        unsigned  char getLAbelType(){return labelType;}
         Labval getLabelValue(){return labelValue;};
        bool is_used(){return used;}
        
        void setStringLabel(char * s){
            labelValue.setStringLabel(s);
        }

        void setNumericLabel(unsigned long s){
            labelValue.setNumericLabel(s);
        }

        char *getStringLabel(){
                return labelValue.getStringLabel();
        }

        unsigned long getNumericLabel(){
                return labelValue.getNumericLabel();
        }


        /*
         * Setters for the different Attributes
         */
        
        void setLabelType(unsigned char l){ labelType =l;}
        void setLabelValue( Labval params){labelValue =params;};
        void setused(bool _used){used =_used;}

        void PrintLabel();
        void WriteLabelToFile(FILE *outFile, bool suppressQuotes);

        unsigned long SubLabelNumber();
        
        
    };




#endif //SUBDUE_LABEL_HPP
