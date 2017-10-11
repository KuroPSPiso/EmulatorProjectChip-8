#include "CartridgeLDR.h"

CartridgeLDR::CartridgeLDR()
{
	//Empty constructor
}

void CartridgeLDR::Dispose()
{
	delete this;
}

void CartridgeLDR::LoadIntoCartridgeSlot(unsigned char* cartridgeSlot)
{
	for (int b = 0; b < sizeof(this->cartridge); b++)
	{
		cartridgeSlot[b] = cartridge[b];
	}
}

BOOL CartridgeLDR::LoadCartridge(char* path)
{
	//open file
	FILE* appFile;
	errno_t err;
	if (err = fopen_s(&appFile, path, "rb")) //binary mode
	{
		try
		{
			char buf[4096]; //might requirefix
			strerror_s(buf, sizeof buf, err);
			fprintf_s(stderr, "cannot open file '%s': %s\n",
				path, buf);
		}
		catch (const std::exception& ex)
		{
			//error on errormessage
		}

		return FALSE;
	}

	if (appFile == NULL)
	{
		fputs("Failed to open file", stderr);
		return FALSE;
	}

	//check file size
	fseek(appFile, 0, SEEK_END);	//load full file
	long lSize = ftell(appFile);	//save loaded size
	rewind(appFile);				//reset file to position 0

	// Allocate memory to contain the whole file
	char * buffer = (char*)malloc(sizeof(char) * lSize);
	
	//if the buffer is left unset then fail
	if (buffer == NULL)
	{
		fputs("Memory error", stderr);
		return FALSE;
	}

	// Copy the file into the buffer
	size_t result = fread(buffer, 1, lSize, appFile);

	//if the result is not equal to the expected load then fail
	if (result != lSize)
	{
		fputs("Reading error", stderr);
		return FALSE;
	}
	
	//compare size of rom to slot capacity
	if ((sizeof(cartridge)) >= lSize)
	{
		for (int i = 0; i < lSize; ++i)
		{
			cartridge[i] = buffer[i];
		}
	}
	else
	{
		printf("Error: ROM is not a cartridge");
		return FALSE;
	}

	printf("Success: ROM has loaded");

	// Close file, free buffer
	fclose(appFile);
	free(buffer);

	return TRUE;
}
