#pragma once
#include "Components.h"

bool SearchElement(string element, vector<Node>nodes, int &x, int &y);

double CalculateVD(int node1, int node2, vector<Node>nodes);

double ResistorCurrent(string resistor, vector<Node>nodes);

double CalculatePower(string element, vector<Node>nodes);

double VoltageSourceCurrent(string element, vector<Node>nodes, bool search);