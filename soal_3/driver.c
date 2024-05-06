#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rpc/rpc.h>
#include "actions.h"

#define PROG_NUMBER 0x31341712
#define VERSION_NUM 1

CLIENT *client;

void callAndPrint(const char functionName, char *(*rpc_function)(CLIENT *, ...), void *argument) {
    char *result;
    result = rpc_function(client, argument);
    printf("[Paddock] [%s]: [%s]\n", functionName, result);
    free(result);
}

int main(int argc, char *argv[]) {
    char *server;
    if (argc < 2) {
        printf("Usage: %s server_host\n", argv[0]);
        exit(1);
    }

    server = argv[1];
    client = clnt_create(server, PROG_NUMBER, VERSION_NUM, "tcp");
    if (client == NULL) {
        clnt_pcreateerror(server);
        exit(1);
    }

    // Example of using RPC for Distance
    float distance = 7.2;
    callAndPrint("Distance", distance_rpc_1, &distance);

    // Example of using RPC for Fuel
    int fuel = 65;
    callAndPrint("Fuel", fuel_rpc_1, &fuel);

    // Example of using RPC for Tire
    int tireCondition = 45;
    callAndPrint("Tire", tire_rpc_1, &tireCondition);

    // Example of using RPC for Tire Change
    char currentTire[] = "Soft";
    callAndPrint("Tire Change", tireChange_rpc_1, &currentTire);

    clnt_destroy(client);
    return 0;
}
