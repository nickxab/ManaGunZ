#include "dynarec.h"

void patch_htab()
{	
	int i;
	for(i=0; i<10; i++) {
	
		// payload
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x00, 0x7C0802A6F8010010ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x08, 0xF821FF81F8410070ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x10, 0x3C40800060420000ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x18, 0x784207C664420000ULL | ( ((DYNAREC_PAYLOAD_ADDR + 0x98)>>16)&0xFFFF) );
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x20, 0x60420000E8020000ULL | ( ((DYNAREC_PAYLOAD_ADDR + 0x98))&0xFFFF)<<32 );
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x28, 0xE84200087C0903A6ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x30, 0x4E800421E8410070ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x38, 0x38210080E8010010ULL);// BCTR <htab_write_caller> desc
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x40, 0x7C0803A64E800020ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x48, 0x78C607647C0802A6ULL);// <htab_write_caller>
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x50, 0xF801001060C60002ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x58, 0xF821FF914800001DULL);// -> BL <lv1_write_htab>
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x60, 0x6000000038210070ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x68, 0x7C6307B4E8010010ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x70, 0x7C0803A64E800020ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x78, 0x7C0802A6F8010010ULL);// <lv1_write_htab>
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x80, 0x3960000144000022ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x88, 0x7C6307B4E8010010ULL);
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0x90, 0x7C0803A64E800020ULL);
		
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x98, DYNAREC_PAYLOAD_ADDR + 0x48);// htab _Custom call desc
        lv2poke(DYNAREC_PAYLOAD_ADDR + 0xA0, 0x8000000000700000ULL);
 		
		// @jump hooks: Replace hvsc calls by these jumps to the new functions
		lv2poke(HTAB_PATCH1, (0x480000012C230000ULL) | ( ((DYNAREC_PAYLOAD_ADDR-HTAB_PATCH1)&0xFFFFFF)<<32) );
        lv2poke(HTAB_PATCH2, (0x480000012C230000ULL) | ( ((DYNAREC_PAYLOAD_ADDR-HTAB_PATCH2)&0xFFFFFF)<<32) );
        lv2poke(HTAB_PATCH3, (0x480000012C230000ULL) | ( ((DYNAREC_PAYLOAD_ADDR-HTAB_PATCH3)&0xFFFFFF)<<32) ); 
	}
	
	__asm__("sync");
}

u8 HaveDynarec()
{
	if( lv2peek(DYNAREC_PAYLOAD_ADDR) == 0x7C0802A6F8010010ULL) return YES;
	return NO;
}

void RemoveDynarec()
{
	int i;
	for(i=0; i<10; i++) {
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x00, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x08, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x10, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x18, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x20, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x28, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x30, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x38, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x40, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x48, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x50, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x58, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x60, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x68, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x70, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x78, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x80, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x88, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x90, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0x98, 0ULL);
		lv2poke(DYNAREC_PAYLOAD_ADDR + 0xA0, 0ULL);
		
		lv2poke(HTAB_PATCH1, 0x440000222C230000ULL);	
		lv2poke(HTAB_PATCH2, 0x440000222C230000ULL);	
		lv2poke(HTAB_PATCH3, 0x440000222C230000ULL);		
	}
}

void InstallDynarec()
{	
	print_head("Installing Dynarec payload");
	
	print_load("Remove lv2 protection");
	remove_lv2_protection();
	
	print_load("Get htab r|w|x access");
	write_htab();
	
	print_load("Patch htab");
	patch_htab();
}
