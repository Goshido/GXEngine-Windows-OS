//version 1.0

#include <GXCommon/GXTime.h>
#include <Windows.h>


GXDouble GXCALL GXGetProcessorTicks ()
{
	GXULong lo;
	GXULong hi;

     __asm
     {
          push ebx
          xor eax, eax
          rdtsc
          mov lo, eax
          mov hi, edx
          pop ebx
     }

     return (GXDouble)lo + (GXDouble)0xFFFFFFFF * hi;
}

GXDouble GXCALL GXGetProcessorTicksPerSecond ()
{
	static GXDouble ticks = -1.0;

	if ( ticks <= 0.0 )
	{
		HKEY hKey;
		DWORD procSpeed;
		DWORD buflen;
		DWORD ret;

		if ( !RegOpenKeyExW ( HKEY_LOCAL_MACHINE, L"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey ) )
		{
			procSpeed = 0;
			buflen = sizeof ( procSpeed );

			ret = RegQueryValueExW ( hKey, L"~MHz", 0, 0, (LPBYTE)&procSpeed, &buflen );

			if ( ret != ERROR_SUCCESS )
				ret = RegQueryValueExW ( hKey, L"~Mhz", 0, 0, (LPBYTE)&procSpeed, &buflen );

			if ( ret != ERROR_SUCCESS )
				ret = RegQueryValueExW ( hKey, L"~mhz", 0, 0, (LPBYTE)&procSpeed, &buflen );

			RegCloseKey ( hKey );

			if ( ret == ERROR_SUCCESS )
				ticks = (GXDouble)procSpeed * 1000000.0;
		}
	}

	return ticks;
}