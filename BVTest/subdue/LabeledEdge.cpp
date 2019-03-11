//
// Created by Macbook on 10/02/2019.
//

#include "stdafx.h"
#include "LabeledEdge.hpp"

 edge &LabeledEdge::getE()  {
    return e;
}

void LabeledEdge::setE( edge &e) {
    LabeledEdge::e = e;
}

unsigned long LabeledEdge::getLabel()  {
    return label;
}

void LabeledEdge::setLabel(unsigned long label) {
    LabeledEdge::label = label;
}

bool LabeledEdge::isDirected()  {
    return directed;
}

void LabeledEdge::setDirected(bool directed) {
    LabeledEdge::directed = directed;
}

bool LabeledEdge::isUsed()  {
    return used;
}

void LabeledEdge::setUsed(bool used) {
    LabeledEdge::used = used;
}

bool LabeledEdge::isSpanIncrement()  {
    return spanIncrement;
}

void LabeledEdge::setSpanIncrement(bool spanIncrement) {
    LabeledEdge::spanIncrement = spanIncrement;
}

bool LabeledEdge::isValidPath()  {
    return validPath;
}

void LabeledEdge::setValidPath(bool validPath) {
    LabeledEdge::validPath = validPath;
}





