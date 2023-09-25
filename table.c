#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>
#include "set.h"

#define EMPTY 0
#define FILLED 1
#define DELETED 2

// coen 12 lab 3, table.c in generic
// 10/20/21
// implement the set data structure provided

typedef struct set SET;

// added array of flags (EMPTY, FILLED, DELETED)
struct set {
	int count; // number of elements
	int length; // max number of elements
	void **data; // pointer to array of words
	char *flags; // parallel array for flag of each word
	int (*compare)(); // compare and hash functions passed in
	unsigned (*hash)();
};

/*	*createSet function
 *	@param maxElts length of set
 *	user will not pass in more words than maxElts
 *	@return pointer to new set of length maxElts
 *	@notation all cases O(m) due to for loop (m is length of sp)
 */
SET *createSet(int maxElts, int (*compare)(), unsigned (*hash)()) {
	SET *sp;
	sp = malloc(sizeof(struct set));
	assert(sp != NULL);
	sp->count = 0;
	sp->length = maxElts;
	sp->data = malloc(sizeof(void*)*maxElts);
	sp->flags = malloc(sizeof(char)*maxElts);
	assert(sp->data != NULL);
	assert(sp->flags != NULL);

	sp->compare = compare;
	sp->hash = hash;

	int i;
	for(i=0; i<sp->length; i++)
		sp->flags[i] = EMPTY;
	return sp;
}

/*	destroySet function
 *	@param *sp pointer of set to be 'destroyed'
 *	deallocates memory used by sp in reverse order
 *	starting with pointers to flags and data
 *	and the set itself
 *	@notation all cases O(m) due to for loop
 */
void destroySet(SET *sp) {
	assert(sp != NULL);

	free(sp->data);
	free(sp->flags);
	free(sp);
}

/*	numElements function
 *	@param *sp pointer to set
 *	@return number of elements in sp
 *	@notation all cases O(1)
 */
int numElements(SET *sp) {
	assert(sp != NULL);
	return sp->count;
}

/*	search function
 *	@param *sp pointer to set
 *	@param *target pointer to word to search for in sp
 *	@param *found pointer to boolean, declare true if target found, false otherwise
 *	@return location of index if target matches
 *		location of empty slot if encountered
 *		location of first deleted (tombstone) slot if no empty slot encountered
 *		home location if no tombstone / empty slot encountered
 *	@notation average case O(1) due to hashing, O(m) worst case
 */
static int search(SET *sp, void *target, bool *found) {
	assert(sp != NULL);
	assert(target != NULL);
	assert(found != NULL);

	unsigned hash;
	int tombstone = -1;
	hash = (*sp->hash)(target) % sp->length;

	*found = false;
	int i;
	for(i=0; i<sp->length; i++) {
		if(sp->flags[hash] == FILLED) {
			if((*sp->compare)(target, sp->data[hash]) == 0) {
				*found = true;
				return hash;
			}
		} else if(sp->flags[hash] == DELETED) {
			if(tombstone == -1)
				tombstone = hash;
		} else { // slot must be empty
			break;
		}
		hash++;
		if(hash == sp->length)
			hash = 0;
	}
	if(tombstone != -1) // if found tombstone while searching
		return tombstone;
	// array is completely full
	return hash;
}

/*	addElement function
 *	@param *sp pointer to set
 *	@param *elt pointer to word to add to sp
 *	adds elt to sp if search returns found false
 *	@notation O(1) average case due to hashing, O(m) worst case
 */
void addElement(SET *sp, void *elt) {
	int locn;
	bool found;
	assert(sp != NULL);
	assert(elt != NULL);
	locn = search(sp, elt, &found);

	// if not found, insert element and change the index's flag
	if(!found) {
		assert(sp->count < sp->length); // ensures set is not full
		void *copy;
		copy = elt;
		assert(copy != NULL);
		sp->data[locn] = copy;
		sp->count++;
		sp->flags[locn] = FILLED;
	}
}

/*	removeElement function
 *	@param *sp pointer to set
 *	@param *elt pointer to word to remove from sp
 *	removes elt from sp if search returns found true
 *	@notation O(1) average case due to hashing, O(m) worst case
 */
void removeElement(SET *sp, void *elt) {
	int locn;
	bool found;
	assert(sp != NULL);
	assert(elt != NULL);
	locn = search(sp, elt, &found);

	// if found change flag to deleted
	if(found) {
		assert(sp->count > 0); // ensures set is not empty
		sp->flags[locn] = DELETED;
		sp->count--;
	}
}

/*	*findElement function
 *	@param *sp pointer to set
 *	@param *elt pointer to word to return if found in sp
 *	@return pointer to copy to elt if found, null otherwise
 *	@notation O(1) average case due to hashing, O(m) worst case
 */
void *findElement(SET *sp, void *elt) {
	int locn;
	bool found;
	assert(sp != NULL);
	assert(elt != NULL);
	locn = search(sp, elt, &found);
	if(found) {
		void *copy;
		copy = sp->data[locn];
		assert(copy != NULL);
		return copy;
	}
	return NULL;
}

/*	*getElements function
 *	@param *sp pointer to set
 *	@return pointer to copy of array of words in sp
 *	@notation all cases O(m) due to for loop
 */
void *getElements(SET *sp) {
	assert(sp != NULL);
	void **copy;
	copy = malloc(sizeof(void*)*sp->count);
	assert(copy != NULL);
	int i, j;
	j=0;
	for(i=0; i<sp->length; i++)
		if(sp->flags[i] == FILLED) {
			void *copy2;
			copy2 = sp->data[i];
			assert(copy2 != NULL);
			copy[j++] = copy2;
		}
	return copy;
}
