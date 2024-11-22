#include <stdbool.h>
#include "types.h"
#pragma once

Set setCreate(CompareFunc compare, DestroyFunc destroy_value);

int setSize(Set set);

void setInsert(Set set, Pointer value);

// Ελευθερώνει όλη τη μνήμη που δεσμεύει το σύνολο.
// Οποιαδήποτε λειτουργία πάνω στο set μετά το destroy είναι μη ορισμένη.

void set_destroy(Set set);

// Διάσχιση του set ////////////////////////////////////////////////////////////
//
// Η διάσχιση γίνεται με τη σειρά διάταξης.

// Οι σταθερές αυτές συμβολίζουν εικονικούς κόμβους _πριν_ τον πρώτο και _μετά_ τον τελευταίο κόμβο του set
#define SET_BOF (SetNode)0
#define SET_EOF (SetNode)0

// Επιστρέφουν τον πρώτο και τον τελευταίο κομβο του set, ή SET_BOF / SET_EOF αντίστοιχα αν το set είναι κενό

SetNode set_first(Set set);
SetNode set_last(Set set);

// Επιστρέφουν τον επόμενο και τον προηγούμενο κομβο του node, ή SET_EOF / SET_BOF
// αντίστοιχα αν ο node δεν έχει επόμενο / προηγούμενο.

SetNode set_next(Set set, SetNode node);
SetNode set_previous(Set set, SetNode node);

// Επιστρέφει το περιεχόμενο του κόμβου node

Pointer set_node_value(Set set, SetNode node);