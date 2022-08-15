// Spindulys by Linas Beresna

#define STB_IMAGE_IMPLEMENTATION

#include "window.h"

int main(int argc, char** argv)
{
	spindulys::spindulysFrontend::spindulysBackendCPU::spindulysGUI::Window mainWindow;
	mainWindow.RenderWindow();

	return 0;
}
