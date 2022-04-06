/*----------------------------------------------------------------------------/
  Lovyan GFX - Graphics library for embedded devices.

Original Source:
 https://github.com/lovyan03/LovyanGFX/

Licence:
 [FreeBSD](https://github.com/lovyan03/LovyanGFX/blob/master/license.txt)

Author:
 [lovyan03](https://twitter.com/lovyan03)

Contributors:
 [ciniml](https://github.com/ciniml)
 [mongonta0716](https://github.com/mongonta0716)
 [tobozo](https://github.com/tobozo)
/----------------------------------------------------------------------------*/
#pragma once

#include "../../Light.hpp"
#include <i2c_manager.h>

//----------------------------------------------------------------------------

/*** LED driver definitions ***/

/* LM27965 LED driver-related definitions */
#define LM27965_BL_GP_REG       0x10    // LM27965 General Purpose Register
#define LM27965_BANK_A_BL_CTRL  0xA0    // Bank A brightness control register
#define LM27965_BANK_B_BL_CTRL  0xB0    // Bank B brightness control register
#define LM27965_BANK_C_BL_CTRL  0xC0    // Bank C brightness control register

#define LM27965_DRIVER_STEPS    31      // LM27965 LED driver brightness level steps
#define INPUT_STEPS			    100     // resolution of the brightness regulation input (adjust according to your LVGL object configured range)

//----------------------------------------------------------------------------

namespace lgfx
{
 inline namespace v1
 {

//----------------------------------------------------------------------------

  class Light_I2C : public ILight
  {
  public:
	enum led_driver_t
	{
		LM27965
	};
	
	struct config_t
    {
        bool 		 invert = false ;
		bool		 i2c_port ;			// I2C port address
		uint8_t		 i2c_addr ;			// LED driver I2C chip address
		led_driver_t led_driver ;		// LED driver type selection

    };

    const config_t& config (void) const { return _cfg; }

    void config ( const config_t &cfg ) { _cfg = cfg; }

    bool    init                ( uint8_t brightness ) override ;
    void    setBrightness       ( uint8_t brightness ) override ;
    uint8_t calculateBrightness ( uint8_t input ) ;    // translate any backlight input range to LED driver's 32-step range

  private:
    config_t _cfg ;
    uint8_t   buf ; // R/W buffer for I2C manager
  };

//----------------------------------------------------------------------------
 }
}
