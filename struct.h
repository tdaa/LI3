typedef struct Contributor{
  int id; //id of the contributor
  char name[256]; //name of the contributor
  int ncontributions; //number of contributions of this author
  int isOccupied; //0 is not occupied and 1 is occupied
}Contributor; //hence, a block called Contributor;


typedef struct RevisionList{
  int id; //id of the revision
  char *title; //title of the revision
  char *date; //date of the creation of the revisions
  int idcontributor; //id of the contributor that wrote the
  int nwords; //number of words of the revision's article
  int nchars; //number of characters of the revison's article
  struct RevisionList *next; //pointer to the next element of the revision list
}RevList; //pretty much connected lists (exactly that)

typedef struct Article{
  int nrevisions; //number of revisions of the article
  struct RevisionList *revisions;
  int isOccupied; // 0 is not occupied and 1 is occupied
  int id; //the value of the id that was hashed -> -1 if it's empty
} Article; //hence, an Article

typedef struct TCD_istruct{
  int hashsize; //size of the hash tables (-1 if the hash tables have not been allocated)
  Article *articles; //declaring a hash table for the articles called articles
  Contributor *authors; //declaring a hash table for the contributors called author
  int occupied_articles; //number of elements inside the articles's hash table
}TCD_istruct;

typedef struct  TCD_istruct * TAD_istruct;

int hash(int id, TAD_istruct qs);

void insertID(int articleID, int revisionID, int contributorID, char* revtitle, char* contrname, char* revdate, char* text, TAD_istruct qs);

void insertWordsChars(int nwords, int nchars, int articleid, int revisionid, TAD_istruct qs);

int increaseHashedID(int hashedID, int hashSize);
