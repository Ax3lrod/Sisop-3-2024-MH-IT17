#include <stdio.h>
#include <string.h>

char* DistanceFunction(float distance) {
    if (distance < 3.5)
        return "KeepGoing";
    else if (distance >= 3.5 && distance <= 10)
        return "Push";
    else
        return "DontMisbehave";
}

char* FuelFunction(int fuel) {
    if (fuel > 80)
        return "PushPushPush";
    else if (fuel >= 50 && fuel <= 80)
        return "CanGo";
    else
        return "ConserveFuel";
}

char* TireFunction(int tireCondition) {
    if (tireCondition > 80)
        return "PushPushPush";
    else if (tireCondition >= 50 && tireCondition < 80)
        return "GoodTireCondition";
    else if (tireCondition >= 30 && tireCondition < 50)
        return "ConserveYourTire";
    else
        return "PitPitPit";
}

char* TireChangeFunction(char* currentTire) {
    if (strcmp(currentTire, "Soft") == 0)
        return "PrepareMediumTire";
    else if (strcmp(currentTire, "Medium") == 0)
        return "PitForSoftTire";
    else
        return "InvalidTireType";
}
