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
#include "Touch_FT5x06.hpp"

#include "../platforms/common.hpp"
#include <i2c_manager.h>

namespace lgfx
{
 inline namespace v1
 {

//----------------------------------------------------------------------------

  static constexpr uint8_t FT5x06_VENDID_REG = 0xA8;
  static constexpr uint8_t FT5x06_POWER_REG  = 0xA5;
  static constexpr uint8_t FT5x06_PERIODACTIVE = 0x88;
  static constexpr uint8_t FT5x06_INTMODE_REG= 0xA4;

  static constexpr uint8_t FT5x06_MONITOR  = 0x01;
  static constexpr uint8_t FT5x06_SLEEP_IN = 0x03;

//----------------------------------------------------------------------------

  bool Touch_FT5x06::init(void)
  {
    _inited = false;
    if (isSPI()) return false;

    if (_cfg.pin_int >= 0)
    {
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pullup);
    }
    return i2c_manager_init ( _cfg.i2c_port ) ;
    return true;
  }

  void Touch_FT5x06::wakeup(void)
  {
    if (_cfg.pin_int >= 0)
    {
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pulldown);
      delayMicroseconds(512);
      lgfx::pinMode(_cfg.pin_int, pin_mode_t::input_pullup);
    }
    i2c_manager_write ( _cfg.i2c_port, _cfg.i2c_addr, FT5x06_POWER_REG, &FT5x06_MONITOR, sizeof (uint8_t) ) ;
  }

  void Touch_FT5x06::sleep(void)
  {
    i2c_manager_write ( _cfg.i2c_port, _cfg.i2c_addr, FT5x06_POWER_REG, &FT5x06_SLEEP_IN, sizeof (uint8_t) ) ;
  }

  uint_fast8_t Touch_FT5x06::getTouchRaw(touch_point_t *tp, uint_fast8_t count)
  {
    if (count == 0) return 0;
    if (_cfg.pin_int >= 0)
    {
      if (_flg_released != (bool)gpio_in(_cfg.pin_int))
      {
        _flg_released = !_flg_released;
      }
      if (_flg_released)
      {
        return 0;
      }
    }
    if (count > 5) count = 5;  // 最大５点まで
    size_t len = count * 6 - 1;

    uint8_t ** tmp = new uint8_t*[2] ;
    tmp[0] = new uint8_t[30] ;
    tmp[1] = new uint8_t[30] ;

    i2c_manager_read ( _cfg.i2c_port, _cfg.i2c_addr, 0x02, &tmp[0][0], len ) ;
    int32_t retry = 5;
    do
    {
      i2c_manager_read ( _cfg.i2c_port, _cfg.i2c_addr, 0x02, &tmp[retry & 1][0], len ) ;
    } while (memcmp(tmp[0], tmp[1], len) && --retry);

    if (count > tmp[0][0]) count = tmp[0][0];
    for (size_t idx = 0; idx < count; ++idx)
    {
      auto data = &tmp[0][idx * 6];
      tp[idx].size = 1;
      tp[idx].x = (data[1] & 0x0F) << 8 | data[2];
      tp[idx].y = (data[3] & 0x0F) << 8 | data[4];
      tp[idx].id = data[3] >> 4;
    }
    return count;
  }

//----------------------------------------------------------------------------

 }
}
