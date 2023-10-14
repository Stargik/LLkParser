#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE_NAME "input.txt"
#define DEFAULT_FIRSTS_COUNT 300
#define DEFAULT_PROD_COUNT 20
#define EPS 'e'
#define MAX_STACK 200
#define MAX_WORD_SIZE 200
#define MAX_PI_SEQUENCE 200

struct Production
{
    char currentSymbol;
    char* resultSequence;
};

struct Grammar
{
    int terminalCount;
    int nonterminalCount;
    int productionsCount;
    char startSymbol;
    struct Production* productions;
};

struct Stack
{
    char data[MAX_STACK];
    int size;
};

void push(struct Stack *stack, char* sequence){
    for (int i = 0; i < strlen(sequence); i++) {
        stack->data[stack->size] = sequence[strlen(sequence) - 1 - i];
        stack->size++;
    }
}

void pop(struct Stack *stack){
    stack->size--;
}

void readFromFile(FILE *fp, struct Grammar *grammar, char terminals[], char nonterminals[]){
    fscanf(fp, "%d\n", &(grammar->terminalCount));
    for (int i = 0; i < grammar->terminalCount; i++) {
        fscanf(fp, "%s ", &(terminals[i]));
    }
    fscanf(fp, "%d\n", &(grammar->nonterminalCount));
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        fscanf(fp, "%s ", &(nonterminals[i]));
    }
    fscanf(fp, "%s\n", &(grammar->startSymbol));
    fscanf(fp, "%d\n", &(grammar->productionsCount));
    struct Production *productions = malloc(sizeof(struct Production) * grammar->productionsCount);
    for (int i = 0; i < grammar->productionsCount; i++) {
        productions[i].resultSequence = malloc(sizeof(char) * DEFAULT_PROD_COUNT);
        fscanf(fp, "%s -> %s", &(productions[i].currentSymbol), productions[i].resultSequence);
    }
    grammar->productions = productions;
}

int isTerminalWord(char* word, char terminals[], int terminalsCount){
    for (int i = 0; i < strlen(word); i++) {
        int isTerminal = 0;
        for (int j = 0; j < terminalsCount; j++) {
            if (word[i] == terminals[j]) {
                isTerminal = 1;
                break;
            }
        }
        if (!isTerminal) {
            return 0;
        }
    }
    return 1;
}

int getNonterminalIndex(char symbol, char nonterminals[], int nonterminalsCount){
    int index = -1;
    for (int m = 0; m < nonterminalsCount; m++) {
        if (symbol == nonterminals[m]) {
            index = m;
            break;
        }
    }
    return index;
}

int isDefinedSequence(int firstsCount[], char nonterminals[], int nonterminalsCount, char* resultSequence){
    int res = 1;
    for (int l = 0; l < strlen(resultSequence); l++) {
        int index = getNonterminalIndex(resultSequence[l], nonterminals, nonterminalsCount);
        if (index != -1 && firstsCount[index] == 0) {
            res = 0;
            break;
        }
    }
    return res;
}

int isWordExist(char* words[], int wordsCount, char* word, int len){
    int res = 0;
    char* tempWord = malloc(sizeof(char) * len);
    strncpy(tempWord, word, len);
    for (int i = 0; i < wordsCount; i++) {
        if (!strcmp(words[i], tempWord)) {
            res = 1;
            break;
        }
    }
    return res;
}

void getWord(int k, int m, int i, int l, char* word, char nonterminals[], int nonterminalsCount, char** firsts[], int firstsCount[], int tmpFirstsCount[], char* sequence){
    //char word[k];
    //strncpy(word, word1, m);
    if (l >= strlen(sequence)) {
        if (m > 0) {
            if (!isWordExist(firsts[i], firstsCount[i], word, m)) {
                firsts[i][firstsCount[i]] = malloc(sizeof(char) * m);
                strncpy(firsts[i][firstsCount[i]], word, m);
                printf("%s ", firsts[i][firstsCount[i]]);
                firstsCount[i]++;
            }
        }
        return;
    }
    int index = getNonterminalIndex(sequence[l], nonterminals, nonterminalsCount);
    if (index == -1 && m < 2) {
        word[m] = sequence[l];
        m++;
        getWord(k, m, i, l + 1, word, nonterminals, nonterminalsCount, firsts, firstsCount, tmpFirstsCount, sequence);
    }else{
        for (int t = 0; t < tmpFirstsCount[index]; t++) {
            int tmpm = m;
            for (int s = 0; s < strlen(firsts[index][t]); s++) {
                if (tmpm < k) {
                    if (firsts[index][t][s] != EPS || (tmpm == 0 && l >= strlen(sequence) - 1)) {
                        word[tmpm] = firsts[index][t][s];
                        tmpm++;
                    }
                }
            }
            if (tmpm >= k) {
                if (!isWordExist(firsts[i], firstsCount[i], word, tmpm)) {
                    firsts[i][firstsCount[i]] = malloc(sizeof(char) * tmpm);
                    strncpy(firsts[i][firstsCount[i]], word, tmpm);
                    printf("%s ", firsts[i][firstsCount[i]]);
                    firstsCount[i]++;
                }
            }else{
                getWord(k, tmpm, i, l + 1, word, nonterminals, nonterminalsCount, firsts, firstsCount, tmpFirstsCount, sequence);
            }
        }
    }
}

void arrCopy(int dest[], int src[], int n){
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

int cmpArr(int arr1[], int arr2[], int n){
    int res = 1;
    for (int i = 0; i < n; i++) {
        if (arr1[i] != arr2[i]) {
            res = 0;
            break;
        }
    }
    return res;
}

int isExistInSequense(char s, char* sequence){
    int res = 0;
    for (int i = 0; i < strlen(sequence); i++) {
        if (s == sequence[i]) {
            res = 1;
            break;
        }
    }
    return res;
}

int getIndexInSequense(char s, char* sequence){
    int res = -1;
    for (int i = 0; i < strlen(sequence); i++) {
        if (s == sequence[i]) {
            res = i;
            break;
        }
    }
    return res;
}

void first(int k, struct Grammar *grammar, char nonterminals[], char terminals[], char** firsts[], int firstsCount[]){
    int tempFirstsCount[grammar->nonterminalCount];
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        firstsCount[i] = 0;
        tempFirstsCount[i] = 0;
        firsts[i] = malloc(sizeof(char) * DEFAULT_FIRSTS_COUNT);
        char nonterminal = nonterminals[i];
        printf("%s: ", &nonterminal);
        for (int j = 0; j < grammar->productionsCount; j++) {
            if (nonterminal == grammar->productions[j].currentSymbol && isTerminalWord(grammar->productions[j].resultSequence, terminals, grammar->terminalCount)) {
                char word[k];
                int c = 0;
                for (int l = 0; l < k; l++) {
                    if (l < strlen(grammar->productions[j].resultSequence)) {
                        word[l] = grammar->productions[j].resultSequence[l];
                        c++;
                    }
                }
                firsts[i][firstsCount[i]] = malloc(sizeof(char) * c);
                strncpy(firsts[i][firstsCount[i]], word, c);
                printf("%s ", firsts[i][0]);
                firstsCount[i]++;
            }
        }
        printf("\n");
    }
    
    while (!cmpArr(tempFirstsCount, firstsCount, grammar->nonterminalCount)) {
        arrCopy(tempFirstsCount, firstsCount, grammar->nonterminalCount);
        for (int i = 0; i < grammar->nonterminalCount; i++) {
            char nonterminal = nonterminals[i];
            printf("%s: ", &nonterminal);
            for (int j = 0; j < grammar->productionsCount; j++) {
                if (nonterminal == grammar->productions[j].currentSymbol) {
                    if (isDefinedSequence(firstsCount, nonterminals, grammar->nonterminalCount, grammar->productions[j].resultSequence)) {
                        char word[k];
                        int m = 0;
                        int l = 0;
                        getWord(k, m, i, l, word, nonterminals, grammar->nonterminalCount, firsts, firstsCount, tempFirstsCount, grammar->productions[j].resultSequence);
                    }
                }
            }
            printf("\n");
        }
    }
}

void getFollowWord(int k, int c, int i, int l, char* word, char nonterminals[], int nonterminalsCount, char** firsts[], int firstsCount[], char** follows[], int followsCount[], int tmpFollowsCount[], char* sequence, int currentSymbolIndex){
    if (l >= strlen(sequence)) {
        if (tmpFollowsCount[currentSymbolIndex] > 0 && c < k) {
            for (int m = 0; m < tmpFollowsCount[currentSymbolIndex]; m++) {
                char tempWord[k];
                strncpy(tempWord, word, k);
                int tempc = c;
                for (int t = 0; t < strlen(follows[currentSymbolIndex][m]); t++) {
                    if (tempc < k) {
                        if (follows[currentSymbolIndex][m][t] != EPS || (tempc == 0 && t >= strlen(follows[currentSymbolIndex][m]) - 1)) {
                            tempWord[tempc] = follows[currentSymbolIndex][m][t];
                            tempc++;
                        }
                    }
                }
                if (!isWordExist(follows[i], followsCount[i], tempWord, tempc)) {
                    follows[i][followsCount[i]] = malloc(sizeof(char) * tempc);
                    strncpy(follows[i][followsCount[i]], tempWord, tempc);
                    printf("%s ", follows[i][followsCount[i]]);
                    followsCount[i]++;
                }
            }
            return;
        }else{
            if (c > 0) {
                if (!isWordExist(follows[i], followsCount[i], word, c)) {
                    follows[i][followsCount[i]] = malloc(sizeof(char) * c);
                    strncpy(follows[i][followsCount[i]], word, c);
                    printf("%s ", follows[i][followsCount[i]]);
                    followsCount[i]++;
                }
            }else{
                char startword[1];
                startword[0] = EPS;
                follows[i][followsCount[i]] = malloc(sizeof(char));
                strncpy(follows[i][followsCount[i]], startword, 1);
                printf("%s ", follows[i][followsCount[i]]);
                followsCount[i]++;
            }
            return;
        }
    }
    if (c < k) {
        int sIndex = getNonterminalIndex(sequence[l], nonterminals, nonterminalsCount);
        if (sIndex == -1) {
            word[c] = sequence[l];
            c++;
            getFollowWord(k, c, i, l + 1, word, nonterminals, nonterminalsCount, firsts, firstsCount, follows, followsCount, tmpFollowsCount, sequence, currentSymbolIndex);
        }else{
            if (tmpFollowsCount[sIndex] == 0) {
                for (int t = 0; t < firstsCount[sIndex]; t++) {
                    int tmpc = c;
                    for (int p = 0; p < strlen(firsts[sIndex][t]); p++) {
                        if (tmpc < k) {
                            if (firsts[sIndex][t][p] != EPS) {
                                word[tmpc] = firsts[sIndex][t][p];
                                tmpc++;
                            }
                        }
                    }
                    if (tmpc >= k) {
                        if (!isWordExist(follows[i], followsCount[i], word, tmpc)) {
                            follows[i][followsCount[i]] = malloc(sizeof(char) * tmpc);
                            strncpy(follows[i][followsCount[i]], word, tmpc);
                            printf("%s ", follows[i][followsCount[i]]);
                            followsCount[i]++;
                        }
                    }else{
                        getFollowWord(k, tmpc, i, l + 1, word, nonterminals, nonterminalsCount, firsts, firstsCount, follows, followsCount, tmpFollowsCount, sequence, currentSymbolIndex);
                    }
                }
            }else{
                for (int t = 0; t < followsCount[sIndex]; t++) {
                    int tmpc = c;
                    for (int p = 0; p < strlen(follows[sIndex][t]); p++) {
                        if (tmpc < k) {
                            if (follows[sIndex][t][p] != EPS) {
                                word[tmpc] = follows[sIndex][t][p];
                                tmpc++;
                            }
                        }
                    }
                    if (tmpc >= k) {
                        if (!isWordExist(follows[i], followsCount[i], word, tmpc)) {
                            follows[i][followsCount[i]] = malloc(sizeof(char) * tmpc);
                            strncpy(follows[i][followsCount[i]], word, tmpc);
                            printf("%s ", follows[i][followsCount[i]]);
                            followsCount[i]++;
                        }
                    }else{
                        getFollowWord(k, tmpc, i, l + 1, word, nonterminals, nonterminalsCount, firsts, firstsCount, follows, followsCount, tmpFollowsCount, sequence, currentSymbolIndex);
                    }
                }
            }
        }
    }
}

void follow(int k, struct Grammar *grammar, char nonterminals[], char terminals[], char** firsts[], int firstsCount[], char** follows[], int followsCount[]){
    int tempFollowsCount[grammar->nonterminalCount];
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        follows[i] = malloc(sizeof(char) * DEFAULT_FIRSTS_COUNT);
        followsCount[i] = 0;
        tempFollowsCount[i] = 0;
    }
    int startIndex = getNonterminalIndex(grammar->startSymbol, nonterminals, grammar->nonterminalCount);
    char startword[1];
    startword[0] = EPS;
    follows[startIndex][0] = malloc(sizeof(char) * 1);
    strncpy(follows[startIndex][0], startword, 1);
    followsCount[startIndex]++;
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        char nonterminal = nonterminals[i];
        printf("%s: ", &nonterminal);
        for (int j = 0; j < followsCount[i]; j++) {
            printf("%s ", follows[i][j]);
        }
        printf("\n");
    }
    while (!cmpArr(tempFollowsCount, followsCount, grammar->nonterminalCount)) {
        arrCopy(tempFollowsCount, followsCount, grammar->nonterminalCount);
        for (int i = 0; i < grammar->nonterminalCount; i++) {
            char nonterminal = nonterminals[i];
            printf("%s: ", &nonterminal);
            for (int j = 0; j < grammar->productionsCount; j++) {
                int currentSymbolIndex = getNonterminalIndex(grammar->productions[j].currentSymbol, nonterminals, grammar->nonterminalCount);
                if (tempFollowsCount[currentSymbolIndex] != 0 && isExistInSequense(nonterminal, grammar->productions[j].resultSequence)) {
                    for (int m = 0; m < strlen(grammar->productions[j].resultSequence); m++) {
                        if (nonterminal == grammar->productions[j].resultSequence[m]) {
                            char word[k];
                            int c = 0;
                            getFollowWord(k, c, i, m + 1, word, nonterminals, grammar->nonterminalCount, firsts, firstsCount, follows, followsCount, tempFollowsCount, grammar->productions[j].resultSequence, currentSymbolIndex);
                        }
                    }
                }
            }
            printf("\n");
        }
    }
}

void firstFollowOneHelper(struct Grammar *grammar, char nonterminals[], char terminals[], char** firsts[], int firstsCount[], char** follows[], int followsCount[], char** firstFollowTable[], int firstFollowTableCount[]){
    for (int i = 0; i < grammar->productionsCount; i++) {
        firstFollowTable[i] = malloc(sizeof(char) * DEFAULT_FIRSTS_COUNT);
        firstFollowTableCount[i] = 0;
        int isEps = 1;
        if (strlen(grammar->productions[i].resultSequence)) {
            for (int j = 0; j < strlen(grammar->productions[i].resultSequence); j++) {
                if (isEps) {
                    isEps = 0;
                    int index = getNonterminalIndex(grammar->productions[i].resultSequence[j], nonterminals, grammar->nonterminalCount);
                    if (index == -1) {
                        if (grammar->productions[i].resultSequence[j] != EPS) {
                            firstFollowTable[i][firstFollowTableCount[i]] = malloc(sizeof(char));
                            firstFollowTable[i][firstFollowTableCount[i]][0] = grammar->productions[i].resultSequence[j];
                            firstFollowTableCount[i]++;
                            break;
                        }
                        if (grammar->productions[i].resultSequence[j] == EPS) {
                            isEps = 1;
                        }
                    }
                    char word[1];
                    for (int m = 0; m < firstsCount[index]; m++) {
                        if (firsts[index][m][0] != EPS) {
                            word[0] = firsts[index][m][0];
                            if (!isWordExist(firstFollowTable[i], firstFollowTableCount[i], word, 1)) {
                                firstFollowTable[i][firstFollowTableCount[i]] = malloc(sizeof(char));
                                strncpy(firstFollowTable[i][firstFollowTableCount[i]], word, 1);
                                firstFollowTableCount[i]++;
                            }
                        }
                        if (firsts[index][m][0] == EPS) {
                            isEps = 1;
                        }
                    }
                }
            }
            if (isEps) {
                int index = getNonterminalIndex(grammar->productions[i].currentSymbol, nonterminals, grammar->nonterminalCount);
                char word[1];
                for (int m = 0; m < followsCount[index]; m++) {
                    word[0] = follows[index][m][0];
                    if (!isWordExist(firstFollowTable[i], firstFollowTableCount[i], word, 1)) {
                        firstFollowTable[i][firstFollowTableCount[i]] = malloc(sizeof(char));
                        strncpy(firstFollowTable[i][firstFollowTableCount[i]], word, 1);
                        firstFollowTableCount[i]++;
                    }
                }
            }
        }
    }
}

void mTableOneHelper(struct Grammar *grammar, char nonterminals[], char terminals[], int nonterminalCount, int terminalCount, int mTable[nonterminalCount][terminalCount], char** firstFollowTable[], int firstFollowTableCount[]){
    for (int i = 0; i < nonterminalCount; i++) {
        for (int k = 0; k < terminalCount; k++) {
            mTable[i][k] = -1;
            for (int j = 0; j < grammar->productionsCount; j++) {
                if (grammar->productions[j].currentSymbol == nonterminals[i]) {
                    for (int m = 0; m < firstFollowTableCount[j]; m++) {
                        if (firstFollowTable[j][m][0] == terminals[k]) {
                            mTable[i][k] = j;
                        }
                    }
                }
            }
        }
    }
}

int llOneAnalyzer(struct Grammar *grammar, char nonterminals[], char terminals[], int nonterminalCount, int terminalCount, int mTable[nonterminalCount][terminalCount], char* word, int piSequence[], int *piSequenceCount, int* lexemIndex){
    int res = 1;
    int count = 0;
    struct Stack *stack = malloc(sizeof(struct Stack));
    char tempword[1];
    tempword[0] = grammar->startSymbol;
    push(stack, tempword);
    char currentLexem = word[0];
    int currentLexemIndex = 0;
    while (stack->size) {
        int index = getNonterminalIndex(stack->data[stack->size - 1], nonterminals, grammar->nonterminalCount);
        if (currentLexemIndex >= strlen(word) && index == -1) {
            *piSequenceCount = count;
            printf("Word is not valid.");
            *lexemIndex = currentLexemIndex;
            return 0;
        } else if(currentLexemIndex >= strlen(word) && index != -1){
            int isEpsAvailable = -1;
            for (int j = 0; j < terminalCount; j++) {
                if (terminals[j] == EPS && mTable[index][j] != -1) {
                    isEpsAvailable = j;
                    pop(stack);
                    piSequence[count] = mTable[index][isEpsAvailable];
                    count++;
                    break;
                }
            }
            if (isEpsAvailable == -1) {
                *piSequenceCount = count;
                printf("Word is not valid. Expected: ");
                for (int j = 0; j < terminalCount; j++) {
                    if (mTable[index][j] != -1) {
                        char terminal = terminals[j];
                        printf("%s ", &terminal);
                    }
                }
                *lexemIndex = currentLexemIndex;
                return 0;
            }
        } else if (index == -1) {
            if (stack->data[stack->size - 1] == currentLexem) {
                pop(stack);
                currentLexemIndex++;
                currentLexem = word[currentLexemIndex];
            }else{
                *piSequenceCount = count;
                printf("Word is not valid.");
                *lexemIndex = currentLexemIndex;
                return 0;
            }
        }else{
            int isRuleFind = 0;
            int isEpsAvailable = -1;
            for (int j = 0; j < terminalCount; j++) {
                if (terminals[j] == currentLexem && mTable[index][j] != -1) {
                    pop(stack);
                    if (grammar->productions[mTable[index][j]].resultSequence[0] != EPS) {
                        push(stack, grammar->productions[mTable[index][j]].resultSequence);
                    }
                    isRuleFind = 1;
                    piSequence[count] = mTable[index][j];
                    count++;
                    break;
                }
                if (terminals[j] == EPS && mTable[index][j] != -1) {
                    isEpsAvailable = j;
                }
            }
            if (!isRuleFind) {
                if (isEpsAvailable != -1) {
                    pop(stack);
                    piSequence[count] = mTable[index][isEpsAvailable];
                    count++;
                }else{
                    *piSequenceCount = count;
                    printf("Word is not valid. Expected: ");
                    for (int j = 0; j < terminalCount; j++) {
                        if (mTable[index][j] != -1) {
                            char terminal = terminals[j];
                            printf("%s ", &terminal);
                        }
                    }
                    *lexemIndex = currentLexemIndex;
                    return 0;
                }
            }
        }
    }
    if (currentLexemIndex < strlen(word)) {
        *piSequenceCount = count;
        printf("Word is not valid.");
        *lexemIndex = currentLexemIndex;
        return 0;
    }
    *piSequenceCount = count;
    *lexemIndex = currentLexemIndex;
    return res;
}

int main(int argc, const char * argv[]) {

    char * filename = DEFAULT_INPUT_FILE_NAME;
    FILE *fp = fopen(filename, "r");
    
    
    struct Grammar *grammar = malloc(sizeof(struct Grammar));
    char terminals[DEFAULT_FIRSTS_COUNT];
    char nonterminals[DEFAULT_FIRSTS_COUNT];
    
    readFromFile(fp, grammar, terminals, nonterminals);
    printf("%d\n", grammar->terminalCount);
    for (int i = 0; i < grammar->terminalCount; i++) {
        char c = terminals[i];
        printf("%s ", &c);
    }
    printf("\n");
    printf("%d\n", grammar->nonterminalCount);
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        char c = nonterminals[i];
        printf("%s ", &c);
    }
    printf("\n");
    printf("%s\n", &(grammar->startSymbol));
    printf("%d\n", grammar->productionsCount);
    for (int i = 0; i < grammar->productionsCount; i++) {
        char c = grammar->productions[i].currentSymbol;
        printf("%s -> %s\n", &c, grammar->productions[i].resultSequence);
    }
    
    int k = 1;
    
    char** firsts[grammar->nonterminalCount];
    int firstsCount[grammar->nonterminalCount];
    first(k, grammar, nonterminals, terminals, firsts, firstsCount);
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        char nonterminal = nonterminals[i];
        printf("%s: ", &nonterminal);
        for (int j = 0; j < firstsCount[i]; j++) {
            printf("%s ", firsts[i][j]);
        }
        printf("\n");
    }
    
    char** follows[grammar->nonterminalCount];
    int followsCount[grammar->nonterminalCount];
    follow(k, grammar, nonterminals, terminals, firsts, firstsCount, follows, followsCount);
    for (int i = 0; i < grammar->nonterminalCount; i++) {
        char nonterminal = nonterminals[i];
        printf("%s: ", &nonterminal);
        for (int j = 0; j < followsCount[i]; j++) {
            printf("%s ", follows[i][j]);
        }
        printf("\n");
    }
    
    char** firstFollowTable[grammar->productionsCount];
    int firstFollowTableCount[grammar->productionsCount];
    firstFollowOneHelper(grammar, nonterminals, terminals, firsts, firstsCount, follows, followsCount, firstFollowTable, firstFollowTableCount);
    for (int i = 0; i < grammar->productionsCount; i++) {
        printf("%d: ", i);
        for (int j = 0; j < firstFollowTableCount[i]; j++) {
            printf("%s ", firstFollowTable[i][j]);
        }
        printf("\n");
    }
    
    int mTable[grammar->nonterminalCount][grammar->terminalCount];
    mTableOneHelper(grammar, nonterminals, terminals, grammar->nonterminalCount, grammar->terminalCount, mTable, firstFollowTable, firstFollowTableCount);
    printf("   ");
    for (int j = 0; j < grammar->terminalCount; j++) {
        char terminal = terminals[j];
        printf(" %s ", &terminal);
    }
    printf("\n");
    for (int i = 0; i < grammar->nonterminalCount; i++) {
            char nonterminal = nonterminals[i];
            printf("%s: ", &nonterminal);
            for (int j = 0; j < grammar->terminalCount; j++) {
                if (mTable[i][j] + 1 == -1) {
                    printf("%d ", mTable[i][j] + 1);
                }else{
                    printf(" %d ", mTable[i][j] + 1);
                }
            }
        printf("\n");
    }
    
    int piSequence[MAX_PI_SEQUENCE];
    int piSequenceCount = 0;
    int lexemIndex = 0;
    char word[MAX_WORD_SIZE];
    scanf("%s", word);
    int res = llOneAnalyzer(grammar, nonterminals, terminals, grammar->nonterminalCount, grammar->terminalCount, mTable, word, piSequence, &piSequenceCount, &lexemIndex);
    if (res) {
        printf("pi = <");
        for (int i = 0; i < piSequenceCount; i++) {
             printf(" %d ", piSequence[i] + 1);
        }
        printf(">");
    }else{
        printf(" (error at position: %d)", lexemIndex);
    }
    printf("\n");
    return 0;
}
