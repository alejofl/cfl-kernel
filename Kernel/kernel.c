#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

//CAMBIAR ESTO
void * initializeKernelBinary()
{
	char buffer[10];

	ncPrint(1, "[x64BareBones]");
	ncNewline(1);

	ncPrint(1, "CPU Vendor:");
	ncPrint(1, cpuVendor(buffer));
	ncNewline(1);

	ncPrint(1, "[Loading modules]");
	ncNewline(1);
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint(1, "[Done]");
	ncNewline(1);
	ncNewline(1);

	ncPrint(1, "[Initializing kernel's binary]");
	ncNewline(1);

	clearBSS(&bss, &endOfKernel - &bss);

	ncPrint(1, "  text: 0x");
	ncPrintHex(1, (uint64_t)&text);
	ncNewline(1);
	ncPrint(1, "  rodata: 0x");
	ncPrintHex(1, (uint64_t)&rodata);
	ncNewline(1);
	ncPrint(1, "  data: 0x");
	ncPrintHex(1, (uint64_t)&data);
	ncNewline(1);
	ncPrint(1, "  bss: 0x");
	ncPrintHex(1, (uint64_t)&bss);
	ncNewline(1);

	ncPrint(1, "[Done]");
	ncNewline(1);
	ncNewline(1);
	return getStackBase();
}

void prueba() {
	
}

int main()
{	
	load_idt();
	ncPrint(1, "[Kernel Main]");
	ncNewline(1);
	ncPrint(1, "  Sample code module at 0x");
	ncPrintHex(1, (uint64_t)sampleCodeModuleAddress);
	ncNewline(1);
	ncPrint(1, "  Calling the sample code module returned: ");

	ncStarSplitScreen();
	for (int i = 0; i < 980; i++) {
		if (i == 973) {
			prueba();
		}
		ncPrintColorfulChar(3, i%10 + '0', BLACK, WHITE);
	}
	for (int i = 0; i < 10; i++) {
		ncPrintColorfulChar(5, 'b', BLACK, WHITE);
		ncNewline(5);
	}
	// ACÁ ESTÁ EL LLAMADO A MAIN. Se castea el puntero a función y se salta con (). Luego se imprime el valor de retorno.
	// ncPrintHex(1, ((EntryPoint)sampleCodeModuleAddress)());
	// ncNewline(1);
	// ncNewline(1);

	// ncPrint(1, "  Sample data module at 0x");
	// ncPrintHex(1, (uint64_t)sampleDataModuleAddress);
	// ncNewline(1);
	// ncPrint(1, "  Sample data module contents: ");
	// ncPrint(1, (char*)sampleDataModuleAddress);
	// ncNewline(1);

	// ncPrint(1, "[Finished]");
	return 0;
}
