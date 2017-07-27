#include "Server.h"

using namespace NixieServer;

int main()
{
	Server* pServer = new Server;

	if (!pServer->Start(1111))
	{
		cout << "Cannot start the server." << endl;
		Sleep(4000);
		return 0;
	}

	for (int i = 0; i < 100; i++)
	{
		pServer->Run();
	}

	return 0;
}