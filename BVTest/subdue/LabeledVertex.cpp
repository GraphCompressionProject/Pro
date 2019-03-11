//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "LabeledVertex.hpp"

unsigned long LabeledVertex::getLabel() const {
    return label;
}

void LabeledVertex::setLabel(unsigned long label) {
    LabeledVertex::label = label;
}

unsigned long LabeledVertex::getNumEdges() const {
    return numEdges;
}

void LabeledVertex::setNumEdges(unsigned long numEdges) {
    LabeledVertex::numEdges = numEdges;
}

unsigned long *LabeledVertex::getEdges() const {
    return edges;
}

void LabeledVertex::setEdges(unsigned long *edges) {
    LabeledVertex::edges = edges;
}

unsigned long LabeledVertex::getMap() const {
    return map;
}

void LabeledVertex::setMap(unsigned long map) {
    LabeledVertex::map = map;
}

bool LabeledVertex::isUsed() const {
    return used;
}

void LabeledVertex::setUsed(bool used) {
    LabeledVertex::used = used;
}
