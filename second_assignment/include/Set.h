#include <stdbool.h>
#include "types.h"
#pragma once

Set setCreate(CompareFunc compare, DestroyFunc destroyValue);

int setSize(Set set);

SetNode setInsert(Set set, Pointer value, SetNode subtreRoot);

void nodeDestroy(SetNode node, DestroyFunc destroyValue);
void setDestroy(Set set);

SetNode setMin(Set set);
SetNode setMax(Set set);

Pointer setNodeValue(Set set, SetNode node);

SetNode nodeFindPrevious(SetNode node, Set set, SetNode currentNode);
SetNode nodeFindMax(SetNode node);