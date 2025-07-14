#ifndef NVS_FLASH
#define NVS_FLASH


#include <nvs_flash.h>

void flash_nvs() // call this when the nvs flash is full
{
  nvs_flash_erase();
  nvs_flash_init();
}


#endif