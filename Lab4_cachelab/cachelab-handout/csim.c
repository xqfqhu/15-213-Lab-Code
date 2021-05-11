#include "cachelab.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

typedef struct {
    int LRU;
    char valid;
    long tag;
} Line;


typedef struct{
    int timeStamp;
    Line **set;
    int num_hit;
    int num_miss;
    int num_eviction;
    int association;
    int num_set;
} Cache;



void initCache(int s, int E, int b, Cache *cache);
void updateCache(char action, int s, long tag, Cache *cache);
void deleteCache(Cache *cache);
void simulate(char *fileName, int s, int E, int b, Cache *cache);


void initCache(int s, int E, int b, Cache *cache){
    int S = pow(2,s);
    cache->set = malloc(sizeof(Line *) * S);
    
    if(cache->set == NULL){
        perror("Error: Cache initialization failed. Memory unavailable.");
        return;
    }

    for(int i = 0; i < S; i++){
        (cache->set)[i] = malloc(sizeof(Line) * E);
        if((cache->set)[i] == NULL){
            for(int j = 0; j < i; j++) free((cache->set)[j]);
            free(cache->set);
            perror("Error: Cache initialization failed. Memory unavailable.");
            return;
        }
        for(int j = 0; j < E; j++){
            cache->set[i][j].LRU = 0;
            cache->set[i][j].valid = 'f';
        }
    }
    cache->association = E;
    cache->num_set = S;
    cache->timeStamp = 0;
    cache->num_hit = 0;
    cache->num_miss = 0;
    cache->num_eviction = 0;

    return;
}

void updateCache(char action, int s, long tag, Cache *cache){
    int association = cache->association;
    int freeLineIDX = -1;
    int LRU = cache->timeStamp;
    int LRU_lineIDX;
    for(int i = 0; i < association; i ++){
        if( ((cache->set)[s][i]).valid == 't' && ((cache->set)[s][i]).tag == tag){
            /* case 1 hit -> update */
            (cache->set)[s][i].LRU = cache->timeStamp;
            cache->timeStamp++;
            cache->num_hit++;
            if(action != 'M') return; //case 1-1: hit & L/S -> return
            else return updateCache('S', s, tag, cache);//case 1-1: hit & M -> recursion
        }
        
        if(freeLineIDX == -1 && (cache->set)[s][i].valid == 'f') freeLineIDX = i;
        if((cache->set)[s][i].LRU < LRU) {
            LRU_lineIDX = i;
            LRU = (cache->set)[s][i].LRU;
        }
    }
    /* case 2: miss */
    cache->num_miss++;
    if(freeLineIDX != -1){
        /* case 2-1: miss but not eviction -> update*/
        ((cache->set)[s][freeLineIDX]).LRU = cache->timeStamp;
        cache->timeStamp++;
        ((cache->set)[s][freeLineIDX]).tag = tag;
        ((cache->set)[s][freeLineIDX]).valid = 't';
        if(action != 'M') return; //case 2-1-1: miss but not eviction & L/S -> return
        else return updateCache('S', s, tag, cache); //case 2-1-2: miss but not eviction & M -> recursion
    }
    /* case 2-2: miss and eviction */
    ((cache->set)[s][LRU_lineIDX]).LRU = cache->timeStamp;
    cache->timeStamp++;
    cache->num_eviction++;
    ((cache->set)[s][LRU_lineIDX]).tag = tag;
    if(action != 'M') return; //case 2-2-1: miss and eviction & L/S -> return
    else return updateCache('S', s, tag, cache); //case 2-2-2: miss and eviction & M -> recursion

    
}

void deleteCache(Cache *cache){
    int num_set = cache->num_set;
    for(int i = 0; i < num_set; i++){
        free(cache->set[i]);
    }
    free(cache->set);
    return;
}

void simulate(char *fileName, int s, int E, int b, Cache *cache){
    FILE *pFile;
    pFile = fopen(fileName, "r"); // Open file for reading
    if(pFile == NULL){
        perror("File open failed.");
        return;
    }
    char action;
    unsigned long address;
    int size;

    initCache(s, E, b, cache);

    while (fscanf(pFile, " %c %lx, %d", &action, &address, &size) > 0) {
        if(action != 'I'){
            unsigned long tag_cur = address >> (b+s);
            unsigned long s_cur_long = address << (64-b-s);
            s_cur_long = s_cur_long >> (64-s);
            int s_cur = (int) s_cur_long;
            updateCache(action, s_cur, tag_cur, cache);
        }
    }

    fclose(pFile); // Clean up Resources
} 

int main(int argc, char *argv[])
{
    int opt;
    int s;
    int E;
    int b;
    char *fileName;
    while((opt = getopt(argc, argv, "s:E:b:t:")) != -1){
        switch(opt){
            case 's':
                s = atoi(optarg);
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                fileName = optarg;
                break;
            case ':':
                perror("option needs a value\n");
                break;
            case '?':
                perror("invalid option\n");
                break;
        }   

    }
    Cache cache;
    simulate(fileName, s, E, b, &cache);
    printSummary(cache.num_hit, cache.num_miss, cache.num_eviction);
    deleteCache(&cache);
    return 0;
}
