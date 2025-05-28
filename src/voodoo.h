
#ifndef VOODOO_H
#define VOODOO_H

void Voodoo_Initialize();
void Voodoo_Shut_Down();
void Voodoo_UpdateScreenStart();
void Voodoo_Output_Enable(bool enabled);
bool Voodoo_GetRetrace();
double Voodoo_GetVRetracePosition();
double Voodoo_GetHRetracePosition();

#endif
