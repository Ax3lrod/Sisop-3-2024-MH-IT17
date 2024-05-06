#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <rpc/rpc.h>
#include "actions.h"

#define PROG_NUMBER 0x31341712
#define VERSION_NUM 1
#define DISTANCE_PROC 1
#define FUEL_PROC 2
#define TIRE_PROC 3
#define TIRE_CHANGE_PROC 4

void logRecord(const char source, const char command, const char *info) {
    FILE *logFile = fopen("logRecord.log", "a");
    if (logFile != NULL) {
        time_t currentTime;
        time(&currentTime);
        fprintf(logFile, "[%s] [%s]: [%s] [%s]\n", source, ctime(&currentTime), command, info);
        fclose(logFile);
    }
}

char **distance_rpc_1_svc(float *distance, struct svc_req *rqstp) {
    static char *result;
    result = DistanceFunction(*distance);
    logRecord("Paddock", "Distance", result);
    return (&result);
}

char **fuel_rpc_1_svc(int *fuel, struct svc_req *rqstp) {
    static char *result;
    result = FuelFunction(*fuel);
    logRecord("Paddock", "Fuel", result);
    return (&result);
}

char **tire_rpc_1_svc(int *tireCondition, struct svc_req *rqstp) {
    static char *result;
    result = TireFunction(*tireCondition);
    logRecord("Paddock", "Tire", result);
    return (&result);
}

char **tireChange_rpc_1_svc(char **currentTire, struct svc_req *rqstp) {
    static char *result;
    result = TireChangeFunction(*currentTire);
    logRecord("Paddock", "Tire Change", result);
    return (&result);
}

int main() {
    if (freopen("logRecord.log", "a", stdout) == NULL) {
        perror("freopen");
        exit(1);
    }

    registerrpc(PROG_NUMBER, VERSION_NUM, DISTANCE_PROC, distance_rpc_1_svc, xdr_float, xdr_wrapstring);
    registerrpc(PROG_NUMBER, VERSION_NUM, FUEL_PROC, fuel_rpc_1_svc, xdr_int, xdr_wrapstring);
    registerrpc(PROG_NUMBER, VERSION_NUM, TIRE_PROC, tire_rpc_1_svc, xdr_int, xdr_wrapstring);
    registerrpc(PROG_NUMBER, VERSION_NUM, TIRE_CHANGE_PROC, tireChange_rpc_1_svc, xdr_wrapstring, xdr_wrapstring);

    svc_run();
    return 0;
}
