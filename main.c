//
//  main.c
//  Roller Coaster Ride
//
//  Created by Alex Abrams on 5/3/14.
//  Copyright (c) 2014 Alex Abrams. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>

pthread_mutex_t lock;
pthread_cond_t self;

void passenger(int Id);
void cancarrun();
void cancarrunfinal();
void canpassengergetin(int Id);
void canpassengerleave(int Id);
void go();

int capacity = 2;
int totalpassengers = 6;
int incar = 0;

int ridetaken = 0;
int passengersleft = 6;
int passengersleaving = 0;

int main(int argc, const char * argv[]) {
    
    pthread_t passengerthread[totalpassengers];
    pthread_t car;
    
    int i=1;
    
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&self, NULL);
    
    if (pthread_create(&car, NULL, (void *)car, NULL) != 0){
        perror("pthread_create");
        exit(1);
    }
    
    for (i=1; i < totalpassengers+1; i++) {
        if (pthread_create(&passengerthread[i], NULL, (void *)passenger, &i) != 0){
            perror("pthread_create");
            exit(1);
        }
    }
    
    for (i=1; i < totalpassengers+1; i++) {
        pthread_join(passengerthread[i], NULL);
    }
    
    pthread_join(car, NULL);
    
    printf("All done!\n");
    
    return 0;

}

void car() {
    while (passengersleft - capacity != 0) {
        cancarrun();
        ridetaken = 1;
        printf("Car Running! \n");
        
        go();
        
        printf("Car Finished. \n");
        ridetaken = 0;
        passengersleaving = 1;
        
        pthread_mutex_unlock(&lock);
        pthread_cond_broadcast(&self);
        sleep(1);
        
    }
    
    cancarrun();
    ridetaken = 1;
    printf("Car running one last time! \n");
    
    go();
    
    printf("Car Done. \n");
    ridetaken = 0;
    //limitgoingdown = 1; //not sure yet
    
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&self);
    sleep(1);
    
    cancarrunfinal();
    ridetaken = 0;
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&self);
    printf("Final Ride Done. \n");
    pthread_exit(NULL);
}

void passenger(int Id) {
    canpassengergetin(Id);
    printf("Passenger %d in car! \n", Id);
    
    incar++;
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&self);
    sleep(1);
    
    canpassengerleave(Id);
    printf("Passenger %d leaving car. \n", Id);
    incar--;
    
    if (incar == 0){
        passengersleaving = 0;
    }
    printf("Passenger %d is done now. \n", Id);
    passengersleft--;
    
    pthread_mutex_unlock(&lock);
    pthread_cond_broadcast(&self);
    pthread_exit(NULL);
    
}

void cancarrun() {
    pthread_mutex_lock(&lock);
    while (incar != capacity) {
        printf("Car not yet full... \n");
        pthread_cond_wait(&self, &lock);
        sleep(1);
    }
    
    printf("Car is ready to run. \n");
    return;
}

void cancarrunfinal() {
    pthread_mutex_lock(&lock);
    while (incar != 0) {
        printf("Passengers still unloading... \n");
        pthread_cond_wait(&self, &lock);
        sleep(1);
    }
    
    printf("Passengers unloaded. \n");
    return;
}

void canpassengergetin(int Id) {
    pthread_mutex_lock(&lock);
    while (ridetaken != 0 || passengersleaving || capacity == incar) {
        printf("Passenger %d cannot get in. \n", Id);
        pthread_cond_wait(&self, &lock);
        sleep(1);
    }
    return;
}

void canpassengerleave(int Id) {
    pthread_mutex_lock(&lock);
    while (ridetaken != 0 || passengersleaving == 0) {
        printf("passenger %d must wait and cant get out of car \n", Id);
        pthread_cond_wait(&self, &lock);
        sleep(1);
    }
    return;
}

void go() {
    sleep(2);
    return;
}


    
    
    
        
        
        
        
        
        
        
        
        
        
        
