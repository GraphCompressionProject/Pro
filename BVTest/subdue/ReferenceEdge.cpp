//
// Created by Macbook on 10/02/2019.
//
#include "stdafx.h"
#include "ReferenceEdge.hpp"

unsigned long ReferenceEdge::getVertex1() const {
    return vertex1;
}

void ReferenceEdge::setVertex1(unsigned long vertex1) {
    ReferenceEdge::vertex1 = vertex1;
}

unsigned long ReferenceEdge::getVertex2() const {
    return vertex2;
}

void ReferenceEdge::setVertex2(unsigned long vertex2) {
    ReferenceEdge::vertex2 = vertex2;
}

bool ReferenceEdge::isSpansIncrement() const {
    return spansIncrement;
}

void ReferenceEdge::setSpansIncrement(bool spansIncrement) {
    ReferenceEdge::spansIncrement = spansIncrement;
}

unsigned long ReferenceEdge::getLabel() const {
    return label;
}

void ReferenceEdge::setLabel(unsigned long label) {
    ReferenceEdge::label = label;
}

bool ReferenceEdge::isDirected() const {
    return directed;
}

void ReferenceEdge::setDirected(bool directed) {
    ReferenceEdge::directed = directed;
}

bool ReferenceEdge::isUsed() const {
    return used;
}

void ReferenceEdge::setUsed(bool used) {
    ReferenceEdge::used = used;
}

bool ReferenceEdge::isFailed() const {
    return failed;
}

void ReferenceEdge::setFailed(bool failed) {
    ReferenceEdge::failed = failed;
}

unsigned long ReferenceEdge::getMap() const {
    return map;
}

void ReferenceEdge::setMap(unsigned long map) {
    ReferenceEdge::map = map;
}
