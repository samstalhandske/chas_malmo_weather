#ifndef DATA_H
#define DATA_H

typedef struct {
    char name[32];
    double longitude;
    double latitude;
    char URL[256];
} City;

void buildDatabase();
void listCities();
int citySelect();
City* fetchCity(int choice);

#endif