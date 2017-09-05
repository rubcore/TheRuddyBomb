#ifndef THERUDDYBOMB_LIBRARY_H
#define THERUDDYBOMB_LIBRARY_H

void initTimers();

// Callback to refresh display during menu navigation, using parameter of type enum DisplayRefreshMode.
void refreshMenuDisplay (byte refreshMode);

// Addition or removal of menu items in MenuData.h will require this method
// to be modified accordingly.
byte processMenuCommand(byte cmdId);

#endif