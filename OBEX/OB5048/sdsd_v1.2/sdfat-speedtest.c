
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>


enum {
//	_xinfreq = 20_000_000,
	_xtlfreq = 20_000_000,
	_clkfreq = 200_000_000,

//	HEAPSIZE = 4400,

//
// Prop2 boot pins, set in the Prop2 ROM
//
    MISO_EVAL = 58, MOSI_EVAL, CS_EVAL, CLK_EVAL,

//
// Roger Loh's 4-bit add-on board
//
    BASE_PIN_RL = 8,
    DAT0_RL     = BASE_PIN_RL+0,    // MISO, DO
    DAT1_RL     = BASE_PIN_RL+1,
    DAT2_RL     = BASE_PIN_RL+2,
    DAT3_RL     = BASE_PIN_RL+3,    // CS
    CMD_RL      = BASE_PIN_RL+4,    // MOSI, DI
    CLK_RL      = BASE_PIN_RL+5,    // SCLK
    LED_RL      = BASE_PIN_RL+6,    // output to red LED
    PWR_RL      = BASE_PIN_RL+7,    // Card-Detect input, and power switch, and green LED

    CS_RL       = DAT3_RL,
    MOSI_RL     = CMD_RL,
    MISO_RL     = DAT0_RL,

//
// My hand wired full sized 4-bit SD slot
//
    BASE_PIN_EH = 40,
    CMD_EH      = BASE_PIN_EH+2,    // MOSI, DI
    CLK_EH      = BASE_PIN_EH+3,    // SCLK
    DAT0_EH     = BASE_PIN_EH+4,    // MISO, DO
    DAT1_EH     = BASE_PIN_EH+5,
    DAT2_EH     = BASE_PIN_EH+6,
    DAT3_EH     = BASE_PIN_EH+7,    // CS

    CS_EH       = DAT3_EH,
    MOSI_EH     = CMD_EH,
    MISO_EH     = DAT0_EH,

};



static void  emithex( uint8_t *addr, size_t size )
{
    do {
        printf(" %02x", *addr++);
    } while( --size );
    puts("\n");
}



static uint32_t  randfill( uint32_t *addr, size_t size, uint32_t state )    // "size" is number of longwords
{
    __asm volatile {    // "const" enforces XIP, "volatile" enforces Fcache
		wrfast	#0, addr
		rep	@.rend, size
		xoro32	state
		mov	pb, 0-0
		wflong	pb
.rend
    }

    return state;    // for potential continuation of psuedo random sequence
}



static size_t  randcmp( const uint32_t *addr, size_t size, uint32_t state )    // "size" is number of longwords
{
    uint32_t  count = 0;

    __asm volatile {    // "const" enforces XIP, "volatile" enforces Fcache
		rdfast	#0, addr
		rep	@.rend, size
		rflong	pb
		xoro32	state
		cmp	pb, 0-0   wz
	if_z	add	count, #1
.rend
    }

    return  count;    // pass longword count
}



static void  tester( const char *filename, size_t kbytes, size_t repeats )
{
    FILE  *fh;
    size_t  count, i;
    uint32_t  tmr, seed, rstate, *buff;
    const size_t  bytes = kbytes * 1024;

    buff = __builtin_alloca(bytes);    // auto-frees upon return from tester()
    if( !buff ) {    // auto-frees upon return from tester()
        printf(" malloc() failed!\n");
        exit(1);
    }

//-----------------------
// Write file
//-----------------------
    seed = _rnd();
    rstate = seed;
    rstate = randfill(buff, kbytes * 256, rstate);

    _seterror(0);
    fh = fopen(filename, "wb");
    if( !fh ) {
        printf(" fopen() for writing failed!   errno = %d: %s\n", errno, strerror(errno));
        exit(3);
    }
    printf(" Buffer = %d kB, ", kbytes);

    count = 0;
    tmr = _getus();
    for( i = 0; i < repeats; i++ ) {
//        rstate = randfill(buff, kbytes * 256, rstate);
        count += fwrite(buff, 1, bytes, fh) / 1024;
    }
    fclose(fh);
    tmr = _getus() - tmr;
    printf(" Written %d kB at %d kB/s, ", count,
            _muldiv64(count, 1_000_000, tmr));
    if( count != kbytes * repeats ) {
        printf(" Error: File not complete!   errno = %d: %s\n", errno, strerror(errno));
        exit(4);
    }

//-----------------------
// Read back file
//-----------------------
    _seterror(0);
    fh = fopen(filename, "rb");
    if( !fh ) {
        printf(" fopen() for readback failed!   errno = %d: %s\n", errno, strerror(errno));
        exit(5);
    }
    count = kbytes * repeats;
    for( i = 0; i < repeats; i++ ) {
//        rstate = seed;
//        seed = randfill(buff, kbytes * 256, rstate);
        if( bytes != fread(buff, 1, bytes, fh) )
            break;
        count -= randcmp(buff, kbytes * 256, seed) / 256;
//        count -= randcmp(buff, kbytes * 256, rstate) / 256;
    }
    if( count )
        printf(" Mis-match! ");
    else
        printf(" Verified, ");

    rewind(fh);    // read the file again, this time purely for speed check
    tmr = _getus();
    for( i = 0; i < repeats; i++ )
        count += fread(buff, 1, bytes, fh) / 1024;
    fclose(fh);
    tmr = _getus() - tmr;
    printf(" Read %d kB at %d kB/s\n", count,
           _muldiv64(count, 1_000_000, tmr));
}


//===================================================================================

    // none compiles to 51084 bytes
    // internal compiles to 53232 bytes
static struct __using("blkdrvr/sdmm.cc") DRV;    // compiles to 54804 bytes
//static struct __using("blkdrvr/sdmm_bashed.cc") DRV;    // compiles to 56904 bytes (w/retries)
//static struct __using("blkdrvr/sdsd.cc") DRV;    // compiles to 60596 bytes


static FILE * mountsd( void )
{
    FILE *handle;
    int  rc, clkdiv;
    uint32_t  part;

    umount("/sd");

    _seterror(0);
//    handle = _sdmm_open(CLK_RL, CS_RL, MOSI_RL, MISO_RL);
//    handle = _sdmm_open(CLK_EVAL, CS_EVAL, MOSI_EVAL, MISO_EVAL);

//    handle = DRV._sdmm_open(CLK_RL, CS_RL, MOSI_RL, MISO_RL);
//    handle = DRV._sdmm_open(CLK_EH, CS_EH, MOSI_EH, MISO_EH);
    handle = DRV._sdmm_open(CLK_EVAL, CS_EVAL, MOSI_EVAL, MISO_EVAL);

//    handle = DRV._sdsd_open(CLK_RL, CMD_RL, DAT0_RL, PWR_RL, LED_RL);
//    handle = DRV._sdsd_open(CLK_EH, CMD_EH, DAT0_EH, -1, -1);

    if( !handle )  {
        printf(" device open failed!   errno = %d: %s\n", errno, strerror(errno));
        exit(1);
    }
//    printf(" driver handle = %08x\n", handle);

//    rc = _ioctl(handle, 71, buff);    // get CLK_DIV value
//    printf(" ioctl 71 CLKDIV=%d rc=%d errno=%d:%s\n", buff[0], rc, errno, strerror(errno));
    clkdiv = 0;
//    _ioctl(handle, 70, &clkdiv);    // disable read-block CRC processing
    clkdiv = 3;
//    _ioctl(handle, 72, &clkdiv);    // set CLK_DIV value

    mount("/sd", _vfs_open_fat_handle(handle));

    struct stat  *buff = __builtin_alloca(sizeof(struct stat));
    if( !buff ) {
        printf(" malloc() failed!\n");
        exit(1);
    }
    _seterror(0);
    if( stat("/sd", buff) )
        printf(" stat() failed!   errno = %d: %s\n", errno, strerror(errno));
    else {
        printf(" cluster size = %d\n", buff->st_blksize);
    }

/*
    if( fread(buff, 1, 512, handle) ) {
//        emithex(buff, 512);
        part = *(uint16_t *)&buff[0x1fe];
        printf(" MBR signature = $%x ", part);
        part = *(uint32_t *)&buff[0x1b8];
        printf(" disc signature = $%08x ", part);
        part = *(uint32_t *)&buff[0x1be + 8];    // LBA entry, CHS was always broken from first principles
        printf(" partition start = $%x\n", part);

        fseek(handle, part*512, SEEK_SET);
        do {
            fread(buff, 1, 512, handle);
        } while(--part);
        printf(" cluster size = %d kB\n", buff[0x0d] / 2);
    } else
        printf(" fread() failed!\n");
*/

    return handle;
}



static void  shutdown( void )
{
    umount("/sd");
    puts("\nexit\n");  // blank line for re-run separation
    _waitms(500);
    _clkset(1, 20_000);  // cool running
}



//===================================================================================

void  main( void )
{
    int  i;

    atexit(shutdown);
    printf("   clkfreq = %d   clkmode = 0x%x\n", _clockfreq(), _clockmode());
    printf("  Compiled with FlexC v%s\n", __VERSION__);

    mountsd();

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed1.bin", 2, 256 );    // 256 x 2 KB = 0.5 MB

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed2.bin", 4, 256 );    // 256 x 4 KB = 1 MB

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed3.bin", 8, 256 );    // 256 x 8 KB = 2 MB
/*
    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed4.bin", 16, 256 );    // 256 x 16 KB = 4 MB

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed5.bin", 32, 128 );    // 128 x 32 KB = 4 MB

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed6.bin", 64, 64 );    // 64 x 64 KB = 4 MB

    puts("");
    for( i = 0; i < 3; i++ )
        tester( "/sd/speed7.bin", 128, 32 );    // 32 x 128 KB = 4 MB
*/
    exit(0);
}
